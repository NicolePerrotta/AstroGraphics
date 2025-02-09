// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"


std::vector<SingleText> outText = {
	{2, {"Press SPACE to start the liftoff", "","",""}, 0, 0},
	{1, {"Launch in: 10"}, 0, 0},
	{1, {"Launch in: 9"}, 0, 0},
	{1, {"Launch in: 8"}, 0, 0},
	{1, {"Launch in: 7"}, 0, 0},
	{1, {"Launch in: 6"}, 0, 0},
	{1, {"Launch in: 5"}, 0, 0},
	{1, {"Launch in: 4"}, 0, 0},
	{1, {"Launch in: 3"}, 0, 0},
	{1, {"Launch in: 2"}, 0, 0},
	{1, {"Launch in: 1"}, 0, 0},
	{1, {""}, 0, 0}
};

#define NSHIP 1
#define NPLANETS 8
struct BlinnUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct BlinnMatParUniformBufferObject {
	alignas(4)  float Power;
};

struct EmissionUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct EngineUniformBufferObject {
    alignas(16) glm::vec3 lightDir[1];
    alignas(16) glm::vec3 lightPos[1];
    alignas(16) glm::vec4 lightColor[1];
    alignas(4) float cosIn;
    alignas(4) float cosOut;
    alignas(16) glm::vec3 eyePos;
    alignas(16) glm::vec4 eyeDir;
    alignas(4) float lightOn;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

struct skyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct NormalUniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct earthUniformBufferObject{
    alignas(4) float pow;
    alignas(4) float ang;
    alignas(4) float showCloud;
    alignas(4) float showTexture;
};


struct BlinnVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct EmissionVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

struct skyBoxVertex {
	glm::vec3 pos;
};

struct earthVertex{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 UV;
    glm::vec4 tan;
};

// MAIN ! 
class CG_Proj : public BaseProject {
	protected:
	
	// Descriptor Layouts ----------------------------------------------------------------------------------------------
	DescriptorSetLayout DSLGlobal;	    // For Global values

	DescriptorSetLayout DSLBlinn;	    // For Blinn Objects
	DescriptorSetLayout DSLEmission;    // For Emission Objects
	DescriptorSetLayout DSLskyBox;	    // For skyBox
    DescriptorSetLayout DSL_Normal;     //For the earth

	// Vertex formats --------------------------------------------------------------------------------------------------
	VertexDescriptor VDBlinn;
	VertexDescriptor VDEmission;
	VertexDescriptor VDskyBox;
    VertexDescriptor VD_Normal;

	// Pipelines -------------------------------------------------------------------------------------------------------
	Pipeline PBlinn;
	Pipeline PEmission;
	Pipeline PskyBox;
    Pipeline P_Normal;

	// Scenes and texts ------------------------------------------------------------------------------------------------
    TextMaker txt;

	// Models, textures and Descriptor Sets (values assigned to the uniforms) ------------------------------------------
	DescriptorSet DSGlobal;

	Model Mship;
	Texture Tship;
	DescriptorSet DSship;
	
	Model Msun;
	Texture Tsun;
	DescriptorSet DSsun;
	
	Model MskyBox;
	Texture TskyBox, Tstars;
	DescriptorSet DSskyBox;

    Model M_earth;
    Texture T_earth_diffuse, T_earth_specular, T_earth_normalMap, T_earth_emission, T_earth_clouds;
    DescriptorSet DS_earth;

    //Planets
	Model M_Planet;
    Texture T_Mercury, T_Venus, T_Moon, T_Mars, T_Jupiter, T_Saturn, T_Uranus, T_Neptune;
    DescriptorSet DS_Mercury, DS_Venus, DS_Moon, DS_Mars, DS_Jupiter, DS_Saturn, DS_Uranus, DS_Neptune;

    //Satellite and asteroids
    Model M_Asteroid, M_Satellite1, M_Satellite2;
    Texture T_Asteroid, T_Satellite1, T_Satellite2;
    DescriptorSet DS_Asteroid, DS_Satellite1, DS_Satellite2;


	// Other application parameters
	int currScene = 0;
	int subpass = 0; //FIXME: eliminare parte del prof

	glm::vec3 CamPos = glm::vec3(0.0, 0.1, 5.0); //fixme: eliminare parte prof
	glm::mat4 ViewMatrix;

	float Ar;
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, title and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "AstroGraphics";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Textures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]------------------------------------------------------
		DSLGlobal.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
				});
		DSLBlinn.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(BlinnUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(BlinnMatParUniformBufferObject), 1}
				});
		DSLEmission.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(EmissionUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
				});
		DSLskyBox.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(skyBoxUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1}
				  });
        DSL_Normal.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(NormalUniformBufferObject), 1},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
                {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1},
                {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2, 1},
                {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 3, 1},
                {5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 4, 1},
                {6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(earthUniformBufferObject), 1},
                {7, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(EngineUniformBufferObject), 1}
        });

		// Vertex descriptors ------------------------------------------------------------------------------------------

		VDBlinn.init(this, {
				  {0, sizeof(BlinnVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BlinnVertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BlinnVertex, norm),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(BlinnVertex, UV),
				         sizeof(glm::vec2), UV}
				});
		VDEmission.init(this, {
				  {0, sizeof(EmissionVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(EmissionVertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(EmissionVertex, UV),
				         sizeof(glm::vec2), UV}
				});
		VDskyBox.init(this, {
				  {0, sizeof(skyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(skyBoxVertex, pos),
				         sizeof(glm::vec3), POSITION}
				});
        VD_Normal.init(this, {
                            {0, sizeof(earthVertex), VK_VERTEX_INPUT_RATE_VERTEX}
                        }, {
                            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(earthVertex, pos),sizeof(glm::vec3), POSITION},
                            {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(earthVertex, norm),sizeof(glm::vec3), NORMAL},
                            {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(earthVertex, UV),sizeof(glm::vec2), UV},
                            {0, 3, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(earthVertex, tan),sizeof(glm::vec4), TANGENT}
                    });

		// Pipelines [Shader couples] ----------------------------------------------------------------------------------
        PBlinn.init(this, &VDBlinn, "shaders/BlinnVert.spv", "shaders/BlinnFrag.spv", {&DSLGlobal, &DSLBlinn});
		PEmission.init(this, &VDEmission, "shaders/EmissionVert.spv", "shaders/EmissionFrag.spv", {&DSLEmission});
		PskyBox.init(this, &VDskyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLskyBox});
		PskyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_BACK_BIT, false);
        P_Normal.init(this, &VD_Normal, "shaders/NormalMapVert.spv", "shaders/NormalMapFrag.spv", {&DSLGlobal, &DSL_Normal});

		// Create models -----------------------------------------------------------------------------------------------
		Mship.init(this, &VDBlinn, "models/Luminaris.obj", OBJ);
		Msun.init(this, &VDEmission, "models/Sphere.obj", OBJ);
		MskyBox.init(this, &VDskyBox, "models/SkyBoxCube.obj", OBJ);
        M_earth.init(this, &VD_Normal, "models/Sphere.gltf", GLTF);
        M_Planet.init(this, &VDBlinn, "models/Sphere.obj", OBJ);
        M_Asteroid.init(this, &VDBlinn, "models/10464_Asteroid_v1_Iterations-2.obj", OBJ);
        M_Satellite1.init(this, &VDBlinn, "models/Satellite1.obj", OBJ);
        M_Satellite2.init(this, &VDBlinn, "models/Satellite2.obj", OBJ);


		// Create the textures -----------------------------------------------------------------------------------------
		Tship.init(this, "textures/Luminaris Diffuse.tga");
		Tsun.init(this, "textures/2k_sun.jpg");
		TskyBox.init(this, "textures/sky-night-star-milky-way-texture-atmosphere-153102-pxhere.com.jpg");
		Tstars.init(this, "textures/constellation_figures.png");
		// Diffuse color of the planet
        T_earth_diffuse.init(this, "textures/2k_earth_daymap.jpg");
		// Specular color of the planet
        T_earth_specular.init(this, "textures/2k_earth_specular_map.png");
		// Normal map of the planet
		// note that it must add a special feature to support the normal map, in particular
		// the init function should be the following: .init(this, "textures/2k_earth_normal_map.png", VK_FORMAT_R8G8B8A8_UNORM);
        T_earth_normalMap.init(this, "textures/2k_earth_normal_map.png", VK_FORMAT_R8G8B8A8_UNORM);
		// Emission map
        T_earth_emission.init(this, "textures/2k_earth_nightmap.jpg");
		// Clouds map
        T_earth_clouds.init(this, "textures/2k_earth_clouds.jpg");

        T_Mercury.init(this, "textures/8k_mercury.jpg");
        T_Venus.init(this, "textures/8k_venus_surface.jpg");
        T_Moon.init(this, "textures/8k_moon.jpg");
        T_Mars.init(this, "textures/8k_mars.jpg");
        T_Jupiter.init(this, "textures/8k_jupiter.jpg");
        T_Saturn.init(this, "textures/8k_saturn.jpg");
        T_Uranus.init(this, "textures/2k_uranus.jpg");
        T_Neptune.init(this, "textures/2k_neptune.jpg");

        T_Asteroid.init(this, "textures/10464_Asteroid_v1_diffuse.jpg");
        T_Satellite1.init(this, "textures/Satellite1.jpg");
        T_Satellite2.init(this, "textures/Satellite2.jpg");

		// Descriptor pool sizes
		// WARNING!!!!!!!!
		// Must be set before initializing the text and the scene

		DPSZs.uniformBlocksInPool = 8;
		DPSZs.texturesInPool = 20;
		DPSZs.setsInPool = 16;

        std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
        PBlinn.create();
		PEmission.create();
		PskyBox.create();
        P_Normal.create();

		// Here you define the data set
		DSship.init(this, &DSLBlinn, {&Tship});
		DSsun.init(this, &DSLEmission, {&Tsun});
		DSskyBox.init(this, &DSLskyBox, {&TskyBox, &Tstars});

        // Textures should be passed in the diffuse, specular, normal map, emission and clouds order.
        DS_earth.init(this, &DSL_Normal, {&T_earth_diffuse, &T_earth_specular, &T_earth_normalMap, &T_earth_emission, &T_earth_clouds});

        DS_Mercury.init(this, &DSLBlinn, {&T_Mercury});
        DS_Venus.init(this, &DSLBlinn, {&T_Venus});
        DS_Moon.init(this, &DSLBlinn, {&T_Moon});
        DS_Mars.init(this, &DSLBlinn, {&T_Mars});
        DS_Jupiter.init(this, &DSLBlinn, {&T_Jupiter});
        DS_Saturn.init(this, &DSLBlinn, {&T_Saturn});
        DS_Uranus.init(this, &DSLBlinn, {&T_Uranus});
        DS_Neptune.init(this, &DSLBlinn, {&T_Neptune});

        DS_Asteroid.init(this, &DSLBlinn, {&T_Asteroid});
        DS_Satellite1.init(this, &DSLBlinn, {&T_Satellite1});
        DS_Satellite2.init(this, &DSLBlinn, {&T_Satellite2});

		DSGlobal.init(this, &DSLGlobal, {});

		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
        PBlinn.cleanup();
		PEmission.cleanup();
		PskyBox.cleanup();
        P_Normal.cleanup();

		DSship.cleanup();
		DSsun.cleanup();
		DSskyBox.cleanup();
		DSGlobal.cleanup();
        DS_earth.cleanup();

        DS_Mercury.cleanup();
        DS_Venus.cleanup();
        DS_Moon.cleanup();
        DS_Mars.cleanup();
        DS_Jupiter.cleanup();
        DS_Saturn.cleanup();
        DS_Uranus.cleanup();
        DS_Neptune.cleanup();

        DS_Asteroid.cleanup();
        DS_Satellite1.cleanup();
        DS_Satellite2.cleanup();

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {

		Tship.cleanup();
		Mship.cleanup();

		Tsun.cleanup();
		Msun.cleanup();

		TskyBox.cleanup();
		Tstars.cleanup();
		MskyBox.cleanup();

        T_earth_diffuse.cleanup();
        T_earth_specular.cleanup();
        T_earth_normalMap.cleanup();
        T_earth_emission.cleanup();
        T_earth_clouds.cleanup();
        M_earth.cleanup();
		
		// Cleanup descriptor set layouts
        DSLBlinn.cleanup();
		DSLEmission.cleanup();
		DSLGlobal.cleanup();
		DSLskyBox.cleanup();
        DSL_Normal.cleanup();

        T_Mercury.cleanup();
        T_Venus.cleanup();
        T_Moon.cleanup();
        T_Mars.cleanup();
        T_Jupiter.cleanup();
        T_Saturn.cleanup();
        T_Uranus.cleanup();
        T_Neptune.cleanup();
        M_Planet.cleanup();

        T_Asteroid.cleanup();
        T_Satellite1.cleanup();
        T_Satellite2.cleanup();
        M_Asteroid.cleanup();
        M_Satellite1.cleanup();
        M_Satellite2.cleanup();

		// Destroys the pipelines
        PBlinn.destroy();
		PEmission.destroy();
		PskyBox.destroy();
        P_Normal.destroy();

		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

        // binds the pipeline
        PBlinn.bind(commandBuffer);
		
		// The models (both index and vertex buffers)
        Mship.bind(commandBuffer);
		
		// The descriptor sets, for each descriptor set specified in the pipeline
		DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);	// The Global Descriptor Set (Set 0)
        DSship.bind(commandBuffer, PBlinn, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)
					
		// The actual draw call.
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mship.indices.size()), NSHIP, 0, 0, 0);

        PBlinn.bind(commandBuffer);
        DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);
        M_Planet.bind(commandBuffer);

        DS_Mercury.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Planet.indices.size()), 1, 0, 0, 0);
        DS_Venus.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Planet.indices.size()), 1, 0, 0, 0);
        DS_Moon.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Planet.indices.size()), 1, 0, 0, 0);
        DS_Mars.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Planet.indices.size()), 1, 0, 0, 0);
        DS_Jupiter.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Planet.indices.size()), 1, 0, 0, 0);
        DS_Saturn.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Planet.indices.size()), 1, 0, 0, 0);
        DS_Uranus.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Planet.indices.size()), 1, 0, 0, 0);
        DS_Neptune.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Planet.indices.size()), 1, 0, 0, 0);


        PBlinn.bind(commandBuffer);
        DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);
        M_Asteroid.bind(commandBuffer);
        DS_Asteroid.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Asteroid.indices.size()), 1, 0, 0, 0);

        PBlinn.bind(commandBuffer);
        DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);
        M_Satellite1.bind(commandBuffer);
        DS_Satellite1.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Satellite1.indices.size()), 1, 0, 0, 0);

        PBlinn.bind(commandBuffer);
        DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);
        M_Satellite2.bind(commandBuffer);
        DS_Satellite2.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Satellite2.indices.size()), 1, 0, 0, 0);


        PEmission.bind(commandBuffer);
		Msun.bind(commandBuffer);
		DSsun.bind(commandBuffer, PEmission, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Msun.indices.size()), 1, 0, 0, 0);	


		PskyBox.bind(commandBuffer);
		MskyBox.bind(commandBuffer);
		DSskyBox.bind(commandBuffer, PskyBox, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MskyBox.indices.size()), 1, 0, 0, 0);


        P_Normal.bind(commandBuffer);
        M_earth.bind(commandBuffer);
        DSGlobal.bind(commandBuffer, P_Normal, 0, currentImage);
        DS_earth.bind(commandBuffer, P_Normal, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(M_earth.indices.size()), 1, 0, 0, 0);


        txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
    }

    // Here is where you update the uniforms.
    void updateUniformBuffer(uint32_t currentImage) {
        static bool debounce = false;
        static int curDebounce = 0;
        static int keyPressed = 0;

        float deltaT;
        glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
        bool fire = false;
        getSixAxis(deltaT, m, r, fire);

        static float autoTime = true;
        static float cTime = 0.0;
        const float turnTime = 72.0f;
        const float angTurnTimeFact = 2.0f * M_PI / turnTime;

        if(autoTime) {
            cTime = cTime + deltaT;
            cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
        }

        static float tTime = 0.0;
        const float TturnTime = 60.0f;
        const float TangTurnTimeFact = 1.0f / TturnTime;

        if(autoTime) {
            tTime = tTime + deltaT;
            tTime = (tTime > TturnTime) ? (tTime - TturnTime) : tTime;
        }

        //Ship position info--------------------------------------------------------------------------------------------
        static float shipPos_x  = 0;
        static float shipPos_y  = 4.1;
        static float shipPos_z  = 0;
        static float shipRoll   = glm::radians(00.0f);
        static float shipPitch  = glm::radians(90.0f);
        static float shipYaw    = glm::radians(-90.0f);

        glm::vec3 shipPosition = glm::vec3(shipPos_x, shipPos_y, shipPos_z);
        glm::vec3 shipForwardDirection  = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 shipRightDirection    = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 shipUpDirection       = glm::vec3(0.0f, 1.0f, 0.0f);

        static glm::vec3 cameraOffset = glm::vec3(0.0f, 0.2f, 0.35f);
        glm::vec3 cameraPosition = shipPosition + cameraOffset;
        //0: look-At (default), 1: look-In
        static int cameraMode = 0;
        //1: forward (default), -1: backward
        static float cameraDirectionLookIn = 1.0;

        //Launch countdown----------------------------------------------------------------------------------------------
        static float LCTime = 0.0;

        static float countdown = false;
        static float liftOff = false;

        float tremorFrequency = 20.0f;
        float tremorAngle = 0;

        if(countdown) {

            int oldTime = static_cast<int>(LCTime);
            LCTime = LCTime + deltaT;
            int newTime = static_cast<int>(LCTime);

            if (newTime == oldTime + 1){
            	currScene = newTime;
            	RebuildPipeline();
                newTime = 11 - newTime;
                std::cout << "countdown    = " << newTime    << ";\n";
            }

            float tremorIntensity = 0.02f * LCTime;
            tremorAngle = tremorIntensity * sin(tremorFrequency * LCTime);

            if (newTime == 11){
                std::cout << "Liftoff start \n";
            	currScene = 11;
            	RebuildPipeline();
                countdown = false;
                liftOff = true;
                std::cout << "Ship is moving \n";
            }


        }

        //Flight timer--------------------------------------------------------------------------------------------------
        static float flightTimer = 0;
        float flightTimerSpeedFactor = 0;

        if (liftOff){

            int oldTime = static_cast<int>(flightTimer);
            flightTimerSpeedFactor = flightTimer + (deltaT) * 1/2;
            flightTimer = flightTimer + deltaT;
            int newTime = static_cast<int>(flightTimer);

            if (newTime == oldTime + 1){
                std::cout << "flightTimer    = " << newTime    << ";\n";
                switch (newTime) {
                    case 1:
                    case 2:
                        keyPressed = 4;
                        break;
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        keyPressed = 2;
                        break;
                    case 7:
                        keyPressed = 3;
                        break;

                }
            }

            int flightDuration = 7;
            if (newTime == flightDuration){
                std::cout << "Liftoff ended\n";
                liftOff = false;
                flightTimerSpeedFactor = 0;
            }

        }

        const float ROT_SPEED = glm::radians(120.0f);
        float MOVE_SPEED = 2.0f;

        static float ShowCloud = 1.0f; //fixme: togliere
        static float ShowTexture = 1.0f; //fixme: togliere

        // The Fly model update proc.
        ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.x * deltaT,glm::vec3(1, 0, 0)) * ViewMatrix;
        ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.y * deltaT,glm::vec3(0, 1, 0)) * ViewMatrix;
        ViewMatrix = glm::rotate(glm::mat4(1), -ROT_SPEED * r.z * deltaT,glm::vec3(0, 0, 1)) * ViewMatrix;
        ViewMatrix = glm::translate(glm::mat4(1),-glm::vec3(  MOVE_SPEED * m.x * deltaT,
                                                              MOVE_SPEED * m.y * deltaT,
                                                              MOVE_SPEED * m.z * deltaT)
        )* ViewMatrix;


        if(glfwGetKey(window, GLFW_KEY_SPACE)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_SPACE;

                std::cout << "Space pressed, countdown started\n";
                countdown = true;

            }
        } else {
            if((curDebounce == GLFW_KEY_SPACE) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        // Standard procedure to quit when the ESC key is pressed
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }


        if(glfwGetKey(window, GLFW_KEY_V)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_V;

                printMat4("ViewMatrix  ", ViewMatrix);
                std::cout << "cTime    = " << cTime    << ";\n";
                std::cout << "tTime    = " << tTime    << ";\n";
                std::cout << "ShowCloud    = " << ShowCloud    << ";\n";
                std::cout << "ShowTexture    = " << ShowTexture    << ";\n";

                std::cout << "m.x    = " << m.x    << ";\n";
                std::cout << "m.y    = " << m.y    << ";\n";
                std::cout << "m.z    = " << m.z    << ";\n";
                std::cout << "r.x    = " << r.x    << ";\n";
                std::cout << "r.y    = " << r.y    << ";\n";
                std::cout << "r.z    = " << r.z    << ";\n\n";

            }
        } else {
            if((curDebounce == GLFW_KEY_V) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }


        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            // executed only when the key is pressed
            std::cout << "LEFT_SHIFT pressed: Speed increased\n";
            MOVE_SPEED = 15.0f;
        }


        if(glfwGetKey(window, GLFW_KEY_1) || keyPressed == 1) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_1;

                std::cout << "Pressed 1: look-at (1)\n";
                cameraMode = 0;
                cameraOffset = glm::vec3(0.0f, 0.1f, 0.15f);
            }
        } else {
            if((curDebounce == GLFW_KEY_1) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_2) || keyPressed == 2) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_2;

                std::cout << "Pressed 2: look-at (2)\n";
                cameraMode = 0;
                cameraOffset = glm::vec3(0.0f, 0.2f, 0.35f);
            }
        } else {
            if((curDebounce == GLFW_KEY_2) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }


        glm::mat4 Mp = glm::perspective(glm::radians(45.0f), Ar, 0.001f, 300.0f);
        Mp[1][1] *= -1;

        glm::mat4 baseTr = glm::mat4(1.0f);

        // objects------------------------------------------------------------------------------------------------------
        BlinnUniformBufferObject blinnUbo{};
        BlinnMatParUniformBufferObject blinnMatParUbo{};

        shipPitch   += ROT_SPEED * r.x * deltaT;
        shipRoll    += -ROT_SPEED * r.z * deltaT;
        shipYaw     += -ROT_SPEED * r.y * deltaT;

        //Update the direction of the ship and of the camera

        //rotationMatrix accumulated from previous frame, that is updated at each frame
        static glm::mat4 rotationMatrix =
                glm::rotate(glm::mat4(1), shipYaw,    glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1), shipRoll,  glm::vec3(1, 0, 0))
                * glm::rotate(glm::mat4(1), shipPitch,   glm::vec3(0, 0, 1));

        //local axes
        shipForwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
        shipRightDirection   = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 0, 1, 0)));
        shipUpDirection      = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 1, 0, 0)));

        //Rotation matrix on local axes
        glm::mat4 incrementalRotation =
                glm::rotate(glm::mat4(1), -ROT_SPEED * r.y * deltaT, shipUpDirection) *    // Yaw attorno all'asse Up locale
                glm::rotate(glm::mat4(1), ROT_SPEED * r.x * deltaT, shipRightDirection) * // Pitch attorno all'asse Right locale
                glm::rotate(glm::mat4(1), -ROT_SPEED * r.z * deltaT, shipForwardDirection); // Roll attorno all'asse Forward locale

        //Update rotationMatrix combining the new rotation with the previous one
        rotationMatrix = incrementalRotation * rotationMatrix;

        //Update again the local axes from the updated rotation matrix
        shipForwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
        shipRightDirection   = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 0, 1, 0)));
        shipUpDirection      = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 1, 0, 0)));

        //Update the position using the new directions------------------------------------------------------

        glm::vec3 autoMovement = shipForwardDirection * flightTimerSpeedFactor * deltaT;
        glm::vec3 movement =
                (shipForwardDirection * (-m.z) +
                 shipRightDirection * m.x +
                 shipUpDirection * m.y)
                * (MOVE_SPEED * deltaT);

        glm::vec3 totalMovement = movement + autoMovement;

        //Update ship position
        shipPos_x += totalMovement.x;
        shipPos_y += totalMovement.y;
        shipPos_z += totalMovement.z;

        blinnUbo.mMat =
                glm::translate(glm::mat4(1), glm::vec3(shipPos_x, shipPos_y, shipPos_z))
                * rotationMatrix
                * glm::scale(glm::mat4(1), glm::vec3(0.0025))
                * baseTr;

        if(glfwGetKey(window, GLFW_KEY_3) || keyPressed == 3) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_3;

                std::cout << "Pressed 3: look-in (1 - forward)\n";
                cameraMode = 1;

                //Offset in local coordinates of the ship
                cameraOffset = glm::vec3(0.0,0.02,0.0);

                cameraDirectionLookIn = 1.0;
            }
        } else {
            if((curDebounce == GLFW_KEY_3) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_4) || keyPressed == 4) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_4;

                std::cout << "Pressed 4: look-in (2 - backward)\n";
                cameraMode = 1;

                //Offset in local coordinates of the ship
                cameraOffset = glm::vec3(0.0,0.02,0.0);

                cameraDirectionLookIn = -1.0;
            }
        } else {
            if((curDebounce == GLFW_KEY_4) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        //Update camera position only in look-in mode
        if (cameraMode == 1){
            cameraPosition = shipPosition + glm::vec3(rotationMatrix * glm::vec4(cameraOffset, 1.0f));
        }

        glm::mat4 cameraMatrix =
                glm::translate( glm::mat4(1.0f), cameraPosition) *
                glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f) * cameraDirectionLookIn, shipUpDirection) *
                rotationMatrix;

        glm::mat4 Mv_lookAt = glm::rotate(glm::mat4(1.0f), tremorAngle, glm::vec3(0, 0, 1)) *
                              glm::lookAt(cameraPosition, shipPosition, shipUpDirection);

        glm::mat4 Mv_lookIn = glm::inverse(cameraMatrix);

        glm::mat4 Mv = (cameraMode == 0) ? Mv_lookAt : Mv_lookIn;

        glm::mat4 ViewPrj = Mp * Mv;


        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));

        DSship.map(currentImage, &blinnUbo, 0);

        blinnMatParUbo.Power = 200.0;
        DSship.map(currentImage, &blinnMatParUbo, 2);

        // updates global uniforms--------------------------------------------------------------------------------------
        // Global
        GlobalUniformBufferObject gubo{};
        gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)) * cos(cTime * angTurnTimeFact),
                                  sin(glm::radians(135.0f)),
                                  cos(glm::radians(135.0f)) * sin(cTime * angTurnTimeFact)
        );
        gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        gubo.eyePos = cameraPosition;
        DSGlobal.map(currentImage, &gubo, 0);


        if(glfwGetKey(window, GLFW_KEY_B)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_B;

                std::cout << "blinnUbo.mMat_x    = " << glm::vec3(blinnUbo.mMat[3])[0]    << ";\n";
                std::cout << "blinnUbo.mMat_y    = " << glm::vec3(blinnUbo.mMat[3])[1]    << ";\n";
                std::cout << "blinnUbo.mMat_z    = " << glm::vec3(blinnUbo.mMat[3])[2]    << ";\n";

                std::cout << "shipPos_x    = " << shipPos_x    << ";\n";
                std::cout << "shipPos_y    = " << shipPos_y    << ";\n";
                std::cout << "shipPos_z    = " << shipPos_z    << ";\n";
                std::cout << "shipRoll     = " << shipRoll     << ";\n";
                std::cout << "shipPitch    = " << shipPitch    << ";\n";
                std::cout << "shipYaw      = " << shipYaw      << ";\n";

                std::cout << "shipForwardDirection_x      = " << shipForwardDirection.x      << ";\n";
                std::cout << "shipForwardDirection_y      = " << shipForwardDirection.y      << ";\n";
                std::cout << "shipForwardDirection_z      = " << shipForwardDirection.z      << ";\n\n";

                std::cout << "shipRightDirection_x      = " << shipRightDirection.x      << ";\n";
                std::cout << "shipRightDirection_y      = " << shipRightDirection.y      << ";\n";
                std::cout << "shipRightDirection_z      = " << shipRightDirection.z      << ";\n\n";

                std::cout << "shipUpDirection_x      = " << shipUpDirection.x      << ";\n";
                std::cout << "shipUpDirection_y      = " << shipUpDirection.y      << ";\n";
                std::cout << "shipUpDirection_z      = " << shipUpDirection.z      << ";\n\n";

                std::cout << "cameraPosition_x      = " << cameraPosition.x      << ";\n";
                std::cout << "cameraPosition_y      = " << cameraPosition.y      << ";\n";
                std::cout << "cameraPosition_z      = " << cameraPosition.z      << ";\n\n";

            }
        } else {
            if((curDebounce == GLFW_KEY_B) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }


        //Engine light uniform -----------------------------------------------------------------------------------------
        EngineUniformBufferObject engineUbo{};

        glm::vec3 engineLightPosition = shipPosition;

        engineUbo.lightDir[0]    = shipForwardDirection;
        engineUbo.lightPos[0]    = engineLightPosition;
        engineUbo.lightColor[0]  = glm::vec4(1.0f, 0.2f, 0.0f, 1.0f);
        engineUbo.cosIn          = glm::cos(glm::radians(20.0f));
        engineUbo.cosOut         = glm::cos(glm::radians(35.0f));
        engineUbo.eyePos         = cameraPosition;
        engineUbo.eyeDir         = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
        engineUbo.lightOn        = (m.z > 0 || -m.z > 0 || flightTimerSpeedFactor!=0) ? 1.0f : 0.0f;

        DS_earth.map(currentImage, &engineUbo, 7);


        //Planet uniforms ----------------------------------------------------------------------------------------------
        BlinnUniformBufferObject planetUbo[NPLANETS];

        // Planet position on x axes in comparison to earth position
        std::unordered_map<std::string, float> planetPositions = {
                {"Mercury", -66.56569f},
                {"Venus",   -33.89986f},
                {"Earth",    0.0f},
                {"Moon",     0.0f},
                {"Mars",    33.06404f},
                {"Jupiter",    33.06404f + 1.06404f + 60.0f + 21.9463f},
                {"Saturn",  33.06404f + 1.06404f + 60.0f + 21.9463f + 120.0f + 18.2797f},
                {"Uranus",    33.06404f + 1.06404f + 60.0f + 21.9463f + 120.0f + 18.2797f + 50.0f + 7.9617f},
                {"Neptune",  33.06404f + 1.06404f + 60.0f + 21.9463f + 120.0f + 18.2797f + 50.0f + 7.9617f + 30.0f + 7.7294f}
        };

        // Planet rotation duration proportional to earth rotation
        std::unordered_map<std::string, float> planetTurnTimes = {
                {"Mercury", 72.0f * (1407.6f / 24.0f)},
                {"Venus",   72.0f * (-5832.5f / 24.0f)},
                {"Earth",    72.0f},
                {"Moon",     72.0f * (656.7f / 24.0f)},
                {"Mars",    72.0f * (24.6f / 24.0f)},
                {"Jupiter",    72.0f * (9.9f / 24.0f)},
                {"Saturn",  72.0f * (10.7f / 24.0f)},
                {"Uranus",    72.0f * (-17.2f / 24.0f)},
                {"Neptune",  72.0f * (16.1f / 24.0f)}
        };

        //Rotation timer for each planet
        static float planetTimes[NPLANETS] = {0.0f};
        static std::unordered_map<std::string, float> planetRotationAngles;

        for (int i = 0; i < NPLANETS; i++) {
            glm::vec3 planetPosition;
            float scale, radius;
            std::string planetName;

            switch (i) {
                case 0: // Mercury
                    planetName = "Mercury";
                    scale = 0.38f * 4.0f;
                    break;
                case 1: // Venus
                    planetName = "Venus";
                    scale = 0.95f * 4.0f;
                    break;
                case 2: // Moon
                    planetName = "Moon";
                    scale = 1.08f;
                    planetPosition = glm::vec3(planetPositions["Moon"], 6.0f, -7.0f);
                    break;
                case 3: // Mars
                    planetName = "Mars";
                    scale = 0.53f * 4.0f;
                    break;
                case 4: // Jupiter
                    planetName = "Jupiter";
                    scale = 10.97f * 4.0f;
                    break;
                case 5: // Saturn
                    planetName = "Saturn";
                    scale = 9.14f * 4.0f;
                    break;
                case 6: // Uranus
                    planetName = "Uranus";
                    scale = 3.98f * 4.0f;
                    break;
                case 7: // Neptune
                    planetName = "Neptune";
                    scale = 3.86f * 4.0f;
                    break;
            }

            //Update rotation time
            planetTimes[i] += deltaT;
            if (planetTimes[i] > planetTurnTimes[planetName]) {
                planetTimes[i] -= planetTurnTimes[planetName];
            }

            //Calculate rotation angle
            float planetAngTurnTimeFact = 2.0f * M_PI / planetTurnTimes[planetName];
            planetRotationAngles[planetName] = planetTimes[i] * planetAngTurnTimeFact;

            // Planet positioning except moon
            if (i != 2) {
                planetPosition = glm::vec3(planetPositions[planetName], 0.0f, 0.0f);
            }

            planetUbo[i].mMat = glm::translate(glm::mat4(1.0f), planetPosition) *
                                glm::rotate(glm::mat4(1.0f), planetRotationAngles[planetName], glm::vec3(0, 1, 0)) *
                                glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
                                baseTr;
            planetUbo[i].mvpMat = ViewPrj * planetUbo[i].mMat;
            planetUbo[i].nMat = glm::inverse(glm::transpose(planetUbo[i].mMat));
        }


        DS_Mercury.map( currentImage, &planetUbo[0], 0);
        DS_Mercury.map( currentImage, &blinnMatParUbo, 2);
        DS_Venus.map(   currentImage, &planetUbo[1], 0);
        DS_Venus.map(   currentImage, &blinnMatParUbo, 2);
        DS_Moon.map(    currentImage, &planetUbo[2], 0);
        DS_Moon.map(    currentImage, &blinnMatParUbo, 2);
        DS_Mars.map(    currentImage, &planetUbo[3], 0);
        DS_Mars.map(    currentImage, &blinnMatParUbo, 2);
        DS_Jupiter.map( currentImage, &planetUbo[4], 0);
        DS_Jupiter.map( currentImage, &blinnMatParUbo, 2);
        DS_Saturn.map(  currentImage, &planetUbo[5], 0);
        DS_Saturn.map(  currentImage, &blinnMatParUbo, 2);
        DS_Uranus.map(  currentImage, &planetUbo[6], 0);
        DS_Uranus.map(  currentImage, &blinnMatParUbo, 2);
        DS_Neptune.map( currentImage, &planetUbo[7], 0);
        DS_Neptune.map( currentImage, &blinnMatParUbo, 2);


        //Asteroid -----------------------------------------------------------------------------------------------------
        BlinnUniformBufferObject asteroidUbo{};
        asteroidUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(3,3,5)) *
                           /*glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 0)) **/
                           glm::scale(glm::mat4(1.0f), glm::vec3(0.1)) *
                           baseTr;
        asteroidUbo.mvpMat = ViewPrj * asteroidUbo.mMat;
        asteroidUbo.nMat = glm::inverse(glm::transpose(asteroidUbo.mMat));

        DS_Asteroid.map(currentImage, &asteroidUbo, 0);
        DS_Asteroid.map(currentImage, &blinnMatParUbo, 2);

        //Satellite ----------------------------------------------------------------------------------------------------
        BlinnUniformBufferObject satellite1Ubo{};
        satellite1Ubo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(2,7,-5)) *
                           /*glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 0)) **/
                           glm::scale(glm::mat4(1.0f), glm::vec3(0.15)) *
                           baseTr;
        satellite1Ubo.mvpMat = ViewPrj * satellite1Ubo.mMat;
        satellite1Ubo.nMat = glm::inverse(glm::transpose(satellite1Ubo.mMat));

        DS_Satellite1.map(currentImage, &satellite1Ubo, 0);
        DS_Satellite1.map( currentImage, &blinnMatParUbo, 2);

        BlinnUniformBufferObject satellite2Ubo{};
        satellite2Ubo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-4,5,7)) *
                             /*glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 0)) **/
                             glm::scale(glm::mat4(1), glm::vec3(0.5)) *
                             baseTr;
        satellite2Ubo.mvpMat = ViewPrj * satellite2Ubo.mMat;
        satellite2Ubo.nMat = glm::inverse(glm::transpose(satellite2Ubo.mMat));

        DS_Satellite2.map(currentImage, &satellite2Ubo, 0);
        DS_Satellite2.map(currentImage, &blinnMatParUbo, 2);

        //Sun ----------------------------------------------------------------------------------------------------------
        EmissionUniformBufferObject emissionUbo{};
        emissionUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), gubo.lightDir * 40.0f) * baseTr;
        DSsun.map(currentImage, &emissionUbo, 0);

        skyBoxUniformBufferObject sbubo{};
        sbubo.mvpMat = Mp * glm::mat4(glm::mat3(Mv));
        DSskyBox.map(currentImage, &sbubo, 0);

        //Earth --------------------------------------------------------------------------------------------------------
        NormalUniformBufferObject nUbo{};

        nUbo.mMat = glm::scale(glm::mat4(1), glm::vec3(4));
        nUbo.nMat = glm::mat4(1.0f);
        nUbo.mvpMat = ViewPrj * nUbo.mMat;

        DS_earth.map(currentImage, &nUbo, 0);


        earthUniformBufferObject earthUbo{};

        // The specular power of the uniform buffer
        earthUbo.pow = 200.0f;

        // The texture angle parameter of the uniform buffer
        earthUbo.ang = tTime * TangTurnTimeFact;

        // The selector for showing the clouds of the uniform buffer
        earthUbo.showCloud = ShowCloud;

        // The selector for showing the clouds of the uniform buffer
        earthUbo.showTexture = ShowTexture;

        DS_earth.map(currentImage, &earthUbo, 6);

        keyPressed = 0;
    }
};

// This is the main: probably you do not need to touch this!
int main() {
    CG_Proj app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
