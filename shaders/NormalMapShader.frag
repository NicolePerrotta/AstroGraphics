#version 450#extension GL_ARB_separate_shader_objects : enable// this defines the variable received from the Vertex Shader// the locations must match the one of its out variableslayout(location = 0) in vec3 fragPos;layout(location = 1) in vec3 fragNorm;layout(location = 2) in vec2 fragUV;layout(location = 3) in vec4 fragTan;// This defines the color computed by this shader. Generally is always location 0.layout(location = 0) out vec4 outColor;// Here the Uniform buffers are defined. In this case, the Global Uniforms of Set 0// The texture of Set 1 (binding 1), and the Material parameters (Set 1, binding 2)// are used. Note that each definition must match the one used in the CPP codelayout(set = 0, binding = 0) uniform NormalMapUniformBufferObject {	vec3 lightDir;	vec4 lightColor;	vec3 eyePos;} gubo;layout(set = 1, binding = 6) uniform NormalMapParUniformBufferObject {	float Pow;	float Ang;	float ShowCloud;	float ShowTexture;} mubo;const int NENGINE=1;layout(set=1, binding = 7) uniform EngineUniformBufferObject {	vec3 lightDir[NENGINE];	vec3 lightPos[NENGINE];	vec4 lightColor[NENGINE];	float cosIn;	float cosOut;	vec3 eyePos;	vec4 eyeDir;	float lightOn;} eubo;layout(set = 1, binding = 1) uniform sampler2D texDiff;layout(set = 1, binding = 2) uniform sampler2D texSpec;layout(set = 1, binding = 3) uniform sampler2D texNM;layout(set = 1, binding = 4) uniform sampler2D texEmit;layout(set = 1, binding = 5) uniform sampler2D texClouds;vec3 spot_light_dir(vec3 pos, int i) {	// Spot light - direction vector	// Direction of the light in <gubo.lightDir[i]>	// Position of the light in <gubo.lightPos[i]>	vec3 p = eubo.lightPos[0];	vec3 x = pos;	return normalize(p - x);}vec3 spot_light_color(vec3 pos, int i) {	// Spot light - color	// Color of the light in <gubo.lightColor[i].rgb>	// Scaling factor g in <gubo.lightColor[i].a>	// Decay power beta: constant and fixed to 2.0	// Position of the light in <gubo.lightPos[i]>	// Direction of the light in <gubo.lightDir[i]>	// Cosine of half of the inner angle in <gubo.cosIn>	// Cosine of half of the outer angle in <gubo.cosOut>	vec3 l = eubo.lightColor[i].rgb;	float g = eubo.lightColor[i].a;	float beta = 2.0f;	vec3 p = eubo.lightPos[i];	vec3 x = pos;	vec3 d = eubo.lightDir[i];	float c_in = eubo.cosIn;	float c_out = eubo.cosOut;	return 	/*light color*/		l	/*Decay factor*/	* pow(g/length(p-x) , beta)	/*Dimming effect*/	* clamp( ( dot( normalize(p-x), d ) -  c_out ) / ( c_in - c_out ), 0,  1 );}vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD) {	// Compute the BRDF, with a given color <Albedo>, in a given position characterized bu a given normal vector <Norm>,	// for a light direct according to <LD>, and viewed from a direction <EyeDir>	vec3 Diffuse;	vec3 Specular;	Diffuse = Albedo * max(dot(Norm, LD),0.0f);	Specular = vec3(pow(max(dot(EyeDir, -reflect(LD, Norm)),0.0f), 160.0f));	return Diffuse + Specular;}// The main shader, implementing a simple Blinn + Lambert + constant Ambient BRDF model// The scene is lit by a single Spot Lightvoid main() {	vec3 Norm = normalize(fragNorm);	vec3 Tan = normalize(fragTan.xyz - Norm * dot(fragTan.xyz, Norm));	vec3 Bitan = cross(Norm, Tan) * fragTan.w;	mat3 tbn = mat3(Tan, Bitan, Norm);	vec4 nMap = texture(texNM, fragUV);	vec3 N = normalize(tbn * (vec3(-1, 1, -1) + nMap.rgb * vec3(2, -2, 2)));		vec3 EyeDir = normalize(gubo.eyePos - fragPos);		vec3 lightDir = normalize(gubo.lightDir);	vec3 lightColor = gubo.lightColor.rgb;		float DiffInt = max(dot(N, lightDir),0.0);	float CloudInt = max(dot(Norm, lightDir),0.0);	float EmitInt = pow(1 - DiffInt,20);	float SpecInt = pow(max(dot(N, normalize(lightDir + EyeDir)),0.0), mubo.Pow);	vec3 DiffColor = texture(texDiff, fragUV).rgb; 	vec3 SpecColor = texture(texSpec, fragUV).rgb; 	vec3 CloudCol = texture(texClouds, fragUV + vec2(mubo.Ang,0)).rgb * mubo.ShowCloud;	vec3 Diffuse = (DiffColor * DiffInt + CloudCol * CloudInt ) * (1-EmitInt);	vec3 Specular = (SpecColor + CloudCol.rgb * 0.05) * SpecInt;	vec3 Emit = texture(texEmit, fragUV).rgb * EmitInt * (1 - CloudCol.g) + 0.01 * CloudCol.g;		vec3 col  = ((Diffuse + Specular) * lightColor + Emit) * mubo.ShowTexture +				(DiffInt + SpecInt) * vec3(1) * (1 - mubo.ShowTexture);	//calcolo riflesso luci motori	vec3 fragToEngineLight = spot_light_dir(fragPos, 0);	vec3 engineLightContribution = spot_light_color(fragPos, 0) * eubo.lightOn;	vec3 RenderEqEngine = BRDF(DiffColor, Norm, EyeDir, fragToEngineLight) * engineLightContribution;//	vec3 col  = Diffuse * lightColor;		outColor = vec4(RenderEqEngine, 1.0f);//	outColor = vec4(vec3(mubo.Ang), 1.0f);//	outColor = vec4(gubo.eyePos/5.0+vec3(0.5),1.0);//	outColor = vec4(0.5*N+vec3(0.5),1.0);//	outColor = vec4(fragPos/5.0+vec3(0.5),1.0);}