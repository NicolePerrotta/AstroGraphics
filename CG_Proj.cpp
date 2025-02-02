// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"


std::vector<SingleText> outText = {
	{2, {"Adding an object", "Press SPACE to save the screenshots","",""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.", "", "",""}, 0, 0},
	{1, {"Launch in: 10"}, 0, 0},
	{1, {"Launch in: 9"}, 0, 0},
	{1, {"Launch in: 8"}, 0, 0},
	{1, {"Launch in: 7"}, 0, 0},
	{1, {"Launch in: 6"}, 0, 0},
	{1, {"Launch in: 5"}, 0, 0},
	{1, {"Launch in: 4"}, 0, 0},
	{1, {"Launch in: 3"}, 0, 0},
	{1, {"Launch in: 2"}, 0, 0},
	{1, {"Launch in: 1"}, 0, 0}
};

// The uniform buffer object used in this example
#define NSHIP 1
struct BlinnUniformBufferObject { //FIXME: ship
//	alignas(16) glm::mat4 mvpMat[NSHIP];
//	alignas(16) glm::mat4 mMat[NSHIP];
//	alignas(16) glm::mat4 nMat[NSHIP];
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct BlinnMatParUniformBufferObject { //FIXME: ship
	alignas(4)  float Power;
};

struct EmissionUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

struct skyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

// **A10** Place here the CPP struct for the uniform buffer for the matrices
struct NormalUniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
// **A10** Place here the CPP struct for the uniform buffer for the parameters
struct earthUniformBufferObject{
    alignas(4) float pow;
    alignas(4) float ang;
    alignas(4) float showCloud;
    alignas(4) float showTexture;
};



// The vertices data structures
struct BlinnVertex { //FIXME: ship
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

// **A10** Place here the CPP struct for the vertex definition
struct earthVertex{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 UV;
    glm::vec4 tan;
};

float finalPosition = 0;

// MAIN ! 
class CG_Proj : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGlobal;	// For Global values

	DescriptorSetLayout DSLBlinn;	// For Blinn Objects  //FIXME: ship
	DescriptorSetLayout DSLEmission;	// For Emission Objects
	DescriptorSetLayout DSLskyBox;	// For skyBox

// **A10** Place here the variable for the DescriptorSetLayout
    DescriptorSetLayout DSL_Normal; //For the earth

	// Vertex formats
	VertexDescriptor VDBlinn; //FIXME: ship
	VertexDescriptor VDEmission;
	VertexDescriptor VDskyBox;
// **A10** Place here the variable for the VertexDescriptor
    VertexDescriptor VD_Normal;

	// Pipelines [Shader couples]
	Pipeline PBlinn; //FIXME: ship
	Pipeline PEmission;
	Pipeline PskyBox;
// **A10** Place here the variable for the Pipeline
    Pipeline P_Normal;

	// Scenes and texts
    TextMaker txt;

	// Models, textures and Descriptor Sets (values assigned to the uniforms)
	DescriptorSet DSGlobal;

    //FIXME: ship
	Model Mship;
	Texture Tship;
	DescriptorSet DSship;
	
	Model Msun;
	Texture Tsun;
	DescriptorSet DSsun;
	
	Model MskyBox;
	Texture TskyBox, Tstars;
	DescriptorSet DSskyBox;

// **A10** Place here the variables for the Model, the five texture (diffuse, specular, normal map, emission and clouds) and the Descrptor Set
    Model M_earth;
    Texture T_earth_diffuse, T_earth_specular, T_earth_normalMap, T_earth_emission, T_earth_clouds;
    DescriptorSet DS_earth;
	
	// Other application parameters
	int currScene = 0;
	int subpass = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 0.1, 5.0);
	glm::mat4 ViewMatrix;

	float Ar;
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A10 - Adding an object";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLGlobal.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
				});
        //FIXME: per ship
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
// **A10** Place here the initialization of the DescriptorSetLayout
        DSL_Normal.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(NormalUniformBufferObject), 1},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
                {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1},
                {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2, 1},
                {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 3, 1},
                {5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 4, 1},
                {6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(earthUniformBufferObject), 1}
        });

		// Vertex descriptors
        //FIXME: ship
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
// **A10** Place here the initialization for the VertexDescriptor
        VD_Normal.init(this, {
                            {0, sizeof(earthVertex), VK_VERTEX_INPUT_RATE_VERTEX}
                        }, {
                            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(earthVertex, pos),sizeof(glm::vec3), POSITION},
                            {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(earthVertex, norm),sizeof(glm::vec3), NORMAL},
                            {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(earthVertex, UV),sizeof(glm::vec2), UV},
                            {0, 3, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(earthVertex, tan),sizeof(glm::vec4), TANGENT}
                    });

		// Pipelines [Shader couples]
        //FIXME: ship
        PBlinn.init(this, &VDBlinn,  "shaders/BlinnVert.spv",    "shaders/BlinnFrag.spv", {&DSLGlobal, &DSLBlinn});
		PEmission.init(this, &VDEmission,  "shaders/EmissionVert.spv",    "shaders/EmissionFrag.spv", {&DSLEmission});
		PskyBox.init(this, &VDskyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLskyBox});
		PskyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_BACK_BIT, false);
// **A10** Place here the initialization of the pipeline. Remember that it should use shaders in files
//		"shaders/NormalMapVert.spv" and "shaders/NormalMapFrag.spv", it should receive the new VertexDescriptor you defined
//		And should receive two DescriptorSetLayout, the first should be DSLGlobal, while the other must be the one you defined
        P_Normal.init(this, &VD_Normal,  "shaders/NormalMapVert.spv",    "shaders/NormalMapFrag.spv", {&DSLGlobal, &DSL_Normal});

		// Create models
		Mship.init(this, &VDBlinn, "models/X-WING-baker.obj", OBJ);
		Msun.init(this, &VDEmission, "models/Sphere.obj", OBJ);
		MskyBox.init(this, &VDskyBox, "models/SkyBoxCube.obj", OBJ);
// **A10** Place here the loading of the model. It should be contained in file "models/Sphere.gltf", it should use the
//		Vertex descriptor you defined, and be of GLTF format.
        M_earth.init(this, &VD_Normal, "models/Sphere.gltf", GLTF);

		// Create the textures
		Tship.init(this, "textures/XwingColors.png");
		Tsun.init(this, "textures/2k_sun.jpg");
		TskyBox.init(this, "textures/starmap_g4k.jpg");
		Tstars.init(this, "textures/constellation_figures.png");
// **A10** Place here the loading of the four textures
		// Diffuse color of the planet in: "2k_earth_daymap.jpg"
        T_earth_diffuse.init(this, "textures/2k_earth_daymap.jpg");
		// Specular color of the planet in: "2k_earth_specular_map.png"
        T_earth_specular.init(this, "textures/2k_earth_specular_map.png");
		// Normal map of the planet in: "2k_earth_normal_map.png"
		// note that it must add a special feature to support the normal map, in particular
		// the init function should be the following: .init(this, "textures/2k_earth_normal_map.png", VK_FORMAT_R8G8B8A8_UNORM);
        T_earth_normalMap.init(this, "textures/2k_earth_normal_map.png", VK_FORMAT_R8G8B8A8_UNORM);
		// Emission map in: "2k_earth_nightmap.jpg"
        T_earth_emission.init(this, "textures/2k_earth_nightmap.jpg");
		// Clouds map in: "2k_earth_clouds.jpg"
        T_earth_clouds.init(this, "textures/2k_earth_clouds.jpg");

		// Descriptor pool sizes
		// WARNING!!!!!!!!
		// Must be set before initializing the text and the scene
// **A10** Update the number of elements to correctly size the descriptor sets pool
        //TODO: check the correct number after deletion of ships
		DPSZs.uniformBlocksInPool = 7;
		DPSZs.texturesInPool = 9;
		DPSZs.setsInPool = 5;

std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
		
		ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
        //FIXME: ship
        PBlinn.create();
		PEmission.create();
		PskyBox.create();
// **A10** Add the pipeline creation
        P_Normal.create();

		// Here you define the data set
		DSship.init(this, &DSLBlinn, {&Tship});
		DSsun.init(this, &DSLEmission, {&Tsun});
		DSskyBox.init(this, &DSLskyBox, {&TskyBox, &Tstars});
// **A10** Add the descriptor set creation
// Textures should be passed in the diffuse, specular, normal map, emission and clouds order.
        DS_earth.init(this, &DSL_Normal, {&T_earth_diffuse, &T_earth_specular, &T_earth_normalMap, &T_earth_emission, &T_earth_clouds});
			
		DSGlobal.init(this, &DSLGlobal, {});

		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
        //FIXME: rimuovere ship
        PBlinn.cleanup();
		PEmission.cleanup();
		PskyBox.cleanup();
// **A10** Add the pipeline cleanup
        P_Normal.cleanup();

		DSship.cleanup();
		DSsun.cleanup();
		DSskyBox.cleanup();
		DSGlobal.cleanup();
// **A10** Add the descriptor set cleanup
        DS_earth.cleanup();

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
        //FIXME: rimuovere ship
		Tship.cleanup();
		Mship.cleanup();

		Tsun.cleanup();
		Msun.cleanup();

		TskyBox.cleanup();
		Tstars.cleanup();
		MskyBox.cleanup();
// **A10** Add the cleanup for models and textures
        T_earth_diffuse.cleanup();
        T_earth_specular.cleanup();
        T_earth_normalMap.cleanup();
        T_earth_emission.cleanup();
        T_earth_clouds.cleanup();
        M_earth.cleanup();
		
		// Cleanup descriptor set layouts
        //FIXME: rimuovere ship
        DSLBlinn.cleanup();
		DSLEmission.cleanup();
		DSLGlobal.cleanup();
		DSLskyBox.cleanup();
// **A10** Add the cleanup for the descriptor set layout
        DSL_Normal.cleanup();
		
		// Destroies the pipelines
        //FIXME: rimuovere ship
        PBlinn.destroy();
		PEmission.destroy();
		PskyBox.destroy();
// **A10** Add the cleanup for the pipeline
        P_Normal.destroy();

		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
        //FIXME: ship
        PBlinn.bind(commandBuffer);
		
		// The models (both index and vertex buffers)
        //FIXME: ship
        Mship.bind(commandBuffer);
		
		// The descriptor sets, for each descriptor set specified in the pipeline
		DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);	// The Global Descriptor Set (Set 0)
        //FIXME: ship
        DSship.bind(commandBuffer, PBlinn, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)
					
		// The actual draw call.
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mship.indices.size()), NSHIP, 0, 0, 0);




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

// **A10** Add the commands to bind the pipeline, the mesh its two descriptor setes, and the draw call of the planet
        P_Normal.bind(commandBuffer);
        M_earth.bind(commandBuffer);
        DSGlobal.bind(commandBuffer, P_Normal, 0, currentImage);
        DS_earth.bind(commandBuffer, P_Normal, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(M_earth.indices.size()), 1, 0, 0, 0);


        txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
    }

    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage) {
        static bool debounce = false;
        static int curDebounce = 0;

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
        static float shipPos_y  = 4.05;
        static float shipPos_z  = 0;
        static float shipRoll   = glm::radians(00.0f);
        static float shipPitch  = glm::radians(90.0f);
        static float shipYaw    = glm::radians(0.0f);

        glm::vec3 shipPosition = glm::vec3(shipPos_x, shipPos_y, shipPos_z);
        glm::vec3 shipForwardDirection  = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 shipRightDirection    = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 shipUpDirection       = glm::vec3(0.0f, 1.0f, 0.0f);

        static glm::vec3 cameraOffset = glm::vec3(0.0f, 0.1f, 0.15f);
        glm::vec3 cameraPosition = shipPosition + cameraOffset;
        glm::vec3 cameraUpVector = glm::vec3(0, 1, 0);
        //0: look-At (default), 1: look-In
        static int cameraMode = 0;
        //1: forward (default), -1: backward
        static float cameraDirectionLookIn = 1.0;

        //Launch countdown----------------------------------------------------------------------------------------------
        static float LCTime = 0.0;

        static float countdown = false;
        static float liftOff = false;


        if(countdown) {

            int oldTime = static_cast<int>(LCTime);
            LCTime = LCTime + deltaT;
            int newTime = static_cast<int>(LCTime);

            if (newTime == oldTime + 1){
            	currScene = newTime+1;
            	RebuildPipeline();
                newTime = 11 - newTime;
                std::cout << "countdown    = " << newTime    << ";\n";
            }

            if (newTime == 11){
                std::cout << "Inizio il lancio \n";
            	currScene = 0;
            	RebuildPipeline();
                countdown = false;
                liftOff = true;
                std::cout << "La navicella si muove \n";
            }
        }

        //Flight timer--------------------------------------------------------------------------------------------------
        static float flightTimer = 0;
        float flightTimerSpeedFactor = 0;


        if (liftOff){

            //TODO: print flightTimer su schermo

            int oldTime = static_cast<int>(flightTimer);
//            float flightTimerSpeedFactor = flightTimer + (deltaT) * 1/2;
            flightTimerSpeedFactor = flightTimer + (deltaT) * 1/2;
            flightTimer = flightTimer + deltaT;
            int newTime = static_cast<int>(flightTimer);

            if (newTime == oldTime + 1){
                std::cout << "flightTimer    = " << newTime    << ";\n";
            }

            //TODO: decidere durata volo
            int flightDuration = 5;
            if (newTime == flightDuration){
                std::cout << "Termino il volo \n";
                liftOff = false;
                finalPosition = flightTimerSpeedFactor;
            }

        }

        const float ROT_SPEED = glm::radians(120.0f);
        const float MOVE_SPEED = 2.0f;

        static float ShowCloud = 1.0f;
        static float ShowTexture = 1.0f;

        // The Fly model update proc.
        ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.x * deltaT,glm::vec3(1, 0, 0)) * ViewMatrix;
        ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.y * deltaT,glm::vec3(0, 1, 0)) * ViewMatrix;
        ViewMatrix = glm::rotate(glm::mat4(1), -ROT_SPEED * r.z * deltaT,glm::vec3(0, 0, 1)) * ViewMatrix;
        ViewMatrix = glm::translate(glm::mat4(1),-glm::vec3(  MOVE_SPEED * m.x * deltaT,
                                                              MOVE_SPEED * m.y * deltaT,
                                                              MOVE_SPEED * m.z * deltaT)
        )* ViewMatrix;


        static float subpassTimer = 0.0;

        if(glfwGetKey(window, GLFW_KEY_SPACE)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_SPACE;

                std::cout << "Premuto spazio, inizio il countdown\n";
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

        if(glfwGetKey(window, GLFW_KEY_C)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_C;

                ShowCloud = 1.0f - ShowCloud;
            }
        } else {
            if((curDebounce == GLFW_KEY_C) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_T)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_T;

                ShowTexture = 1.0f - ShowTexture;
            }
        } else {
            if((curDebounce == GLFW_KEY_T) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }


        if(currScene == 1) {
            switch(subpass) {
                case 0:
                    ViewMatrix   = glm::mat4(-0.0656882, -0.162777, 0.984474, 0, 0.0535786, 0.984606, 0.166374, 0, -0.996401, 0.0636756, -0.0559558, 0, 0.0649244, -0.531504, -3.26128, 1);
                    cTime    = 22.3604;
                    tTime    = 22.3604;
                    ShowCloud    = 1;
                    ShowTexture    = 1;
                    autoTime = false;
                    break;
                case 1:
                    ViewMatrix   = glm::mat4(-0.312507, -0.442291, 0.840666, 0, 0.107287, 0.862893, 0.493868, 0, -0.943837, 0.24453, -0.222207, 0, -0.0157694, -0.186147, -1.54649, 1);
                    cTime    = 38.9919;
                    tTime    = 38.9919;
                    ShowCloud    = 0;
                    ShowTexture    = 1;
                    break;
                case 2:
                    ViewMatrix   = glm::mat4(-0.992288, 0.00260993, -0.12393, 0, -0.0396232, 0.940648, 0.337063, 0, 0.117454, 0.339374, -0.93329, 0, 0.0335061, -0.0115242, -2.99662, 1);
                    cTime    = 71.0587;
                    tTime    = 11.0587;
                    ShowCloud    = 1;
                    ShowTexture    = 1;
                    break;
                case 3:
                    ViewMatrix   = glm::mat4(0.0942192, -0.242781, 0.965495, 0, 0.560756, 0.814274, 0.150033, 0, -0.822603, 0.527272, 0.212861, 0, -0.567191, -0.254532, -1.79143, 1);
                    cTime    = 55.9355;
                    tTime    = 7.93549;
                    ShowCloud    = 1;
                    ShowTexture    = 0;
                    break;
            }
        }

        if(currScene == 1) {
            subpassTimer += deltaT;
            if(subpassTimer > 4.0f) {
                subpassTimer = 0.0f;
                subpass++;
                std::cout << "Scene : " << currScene << " subpass: " << subpass << "\n";
                char buf[20];
                sprintf(buf, "A10_%d.png", subpass);
                saveScreenshot(buf, currentImage);
                if(subpass == 4) {
                    ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
                    cTime    = 0;
                    tTime    = 0;
                    ShowCloud    = 1;
                    ShowTexture    = 1;
                    autoTime = true;


                    currScene = 0;
                    std::cout << "Scene : " << currScene << "\n";
                    RebuildPipeline();
                }
            }
        }

        if(glfwGetKey(window, GLFW_KEY_1)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_1;

                std::cout << "Premuto 1: look-at (1)\n";
                cameraMode = 0;
                cameraOffset = glm::vec3(0.0f, 0.1f, 0.15f);
            }
        } else {
            if((curDebounce == GLFW_KEY_1) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_2)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_2;

                std::cout << "Premuto 2: look-at (2)\n";
                cameraMode = 0;
                cameraOffset = glm::vec3(0.0f, 0.2f, 0.35f);
            }
        } else {
            if((curDebounce == GLFW_KEY_2) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }


        // Here is where you actually update your uniforms
        glm::mat4 Mp = glm::perspective(glm::radians(45.0f), Ar, 0.001f, 160.0f);
        Mp[1][1] *= -1;

//        glm::mat4 Mv_lookAt = glm::rotate(glm::mat4(1.0f), /*shipRoll*/0.0f, glm::vec3(0, 0, 1)) *
//                              glm::lookAt(cameraPosition, shipPosition, /*glm::vec3(0,1,0)*/cameraUpVector);
//
////		glm::mat4 Mv = ViewMatrix;
//        glm::mat4 Mv = Mv_lookAt;
//
//        glm::mat4 ViewPrj = Mp * Mv;
        glm::mat4 baseTr = glm::mat4(1.0f);

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

        // objects------------------------------------------------------------------------------------------------------
        //FIXME: ship
        BlinnUniformBufferObject blinnUbo{};
        BlinnMatParUniformBufferObject blinnMatParUbo{};

        {
//		for(int j = 0; j < 4; j++) {
//			for(int k = 0; k < 4; k++) {
//				int i = j*4+k;
//				blinnUbo.mMat[i] = glm::translate(glm::mat4(1),glm::vec3((k-1)*5+((j+5)*3+10)*cos(j*1.4),(j-1)*5+((k+5)*2.5+10)*sin(j*1.4)*cos(i*0.2),((k+2.5)*3.2+8)*sin(j*1.4)*sin(k*0.2))) * glm::scale(glm::mat4(1), glm::vec3(0.5,0.5,0.5)) * baseTr;
//				blinnUbo.mvpMat[i] = ViewPrj * blinnUbo.mMat[i];
//				blinnUbo.nMat[i] = glm::inverse(glm::transpose(blinnUbo.mMat[i]));
//			}
//		}
        }

        shipPitch   += ROT_SPEED * r.x * deltaT;
        shipRoll    += -ROT_SPEED * r.z * deltaT;
        shipYaw     += -ROT_SPEED * r.y * deltaT;

        //Update the direction of the ship and of the camera

        // Usa la rotationMatrix accumulata dal frame precedente, che viene aggiornata a ogni frame
        static glm::mat4 rotationMatrix =
                glm::rotate(glm::mat4(1), shipYaw,    glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1), shipRoll,  glm::vec3(1, 0, 0))
                * glm::rotate(glm::mat4(1), shipPitch,   glm::vec3(0, 0, 1));

        //assi locali
        shipForwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
        shipRightDirection   = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 0, 1, 0)));
        shipUpDirection      = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 1, 0, 0)));

        //Rotation matrix attorno agli assi locali
        glm::mat4 incrementalRotation =
                glm::rotate(glm::mat4(1), -ROT_SPEED * r.y * deltaT, shipUpDirection) *    // Yaw attorno all'asse Up locale
                glm::rotate(glm::mat4(1), ROT_SPEED * r.x * deltaT, shipRightDirection) * // Pitch attorno all'asse Right locale
                glm::rotate(glm::mat4(1), -ROT_SPEED * r.z * deltaT, shipForwardDirection); // Roll attorno all'asse Forward locale

        //Aggiorna la rotationMatrix combinando la nuova rotazione con la precedente
        rotationMatrix = incrementalRotation * rotationMatrix;

        //Aggiorna nuovamente gli assi locali con la nuova matrice aggiornata
        shipForwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
        shipRightDirection   = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 0, 1, 0)));
        shipUpDirection      = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 1, 0, 0)));

        // Step 3: Aggiorna la posizione usando le nuove direzioni------------------------------------------------------

        // Movimento indipendente dal giocatore
        glm::vec3 autoMovement = shipForwardDirection * flightTimerSpeedFactor * deltaT;

        // Movimento lungo gli assi della ship
        glm::vec3 movement =
                (shipForwardDirection * (-m.z) + // Movimento avanti/indietro nella direzione frontale
                 shipRightDirection * m.x +      // Movimento laterale
                 shipUpDirection * m.y)          // Movimento verticale
                * (MOVE_SPEED * deltaT);         // Scala con velocità e tempo

        // Somma il movimento automatico al movimento controllato
        glm::vec3 totalMovement = movement + autoMovement;

        // Aggiorna la posizione della ship
        shipPos_x += totalMovement.x;
        shipPos_y += totalMovement.y;
        shipPos_z += totalMovement.z;

        // Step 4: Costruisci la matrice finale con la nuova posizione aggiornata---------------------------------------
        blinnUbo.mMat =
                glm::translate(glm::mat4(1), glm::vec3(shipPos_x, shipPos_y, shipPos_z))
                * rotationMatrix
                * glm::scale(glm::mat4(1), glm::vec3(0.025))
                * baseTr;

        // Step 5: Aggiorna la direzione della camera
        cameraUpVector = glm::normalize(glm::cross(shipRightDirection, shipForwardDirection));
        glm::mat4 cameraMatrix =
                glm::translate( glm::mat4(1.0f), cameraPosition /*+ glm::vec3(-0.2,0.2,0)*/) *
                glm::rotate(    glm::mat4(1.0f), shipYaw - glm::radians(90.0f)*cameraDirectionLookIn, shipUpDirection) *
                glm::rotate(    glm::mat4(1.0f), shipPitch,  shipRightDirection) *
                glm::rotate(    glm::mat4(1.0f), shipRoll, shipForwardDirection);

        //FIXME: upVector è stato scelto constante verso y globale per un'effetto visivo migliore,
        // ma usando la direzione data da cameraUpVector, è possibile avere una "vera" look-At che segue l'upVector del oggetto
        glm::mat4 Mv_lookAt = glm::rotate(glm::mat4(1.0f), /*shipRoll*/0.0f, glm::vec3(0, 0, 1)) *
                              glm::lookAt(cameraPosition, shipPosition, glm::vec3(0,1,0)/*cameraUpVector*/);

        glm::mat4 Mv_lookIn = glm::inverse(cameraMatrix);

//		glm::mat4 Mv = ViewMatrix;
        //Default view


        if(glfwGetKey(window, GLFW_KEY_3)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_3;

                std::cout << "Premuto 3: look-in (1 - forward)\n";
                cameraMode = 1;
                cameraOffset = glm::vec3(0.0);
                cameraDirectionLookIn = 1.0;
//                Mv = Mv_lookIn;
            }
        } else {
            if((curDebounce == GLFW_KEY_3) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_4)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_4;

//                cameraOffset = glm::vec3(0, 0.1, 0);
                std::cout << "Premuto 4: look-in (2 - backward)\n";
                cameraMode = 1;
                cameraOffset = /*glm::vec3(0.0,0.5,0.0)*/ shipForwardDirection * (-0.5f);
                cameraDirectionLookIn = -1.0;
            }
        } else {
            if((curDebounce == GLFW_KEY_4) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        glm::mat4 Mv = (cameraMode == 0) ? Mv_lookAt : Mv_lookIn;

        glm::mat4 ViewPrj = Mp * Mv;


        if(glfwGetKey(window, GLFW_KEY_L)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_L;

                std::cout << "---\n";
                std::cout << "cameraPosition_x    = " << cameraPosition.x    << ";\n";
                std::cout << "cameraPosition_y    = " << cameraPosition.y    << ";\n";
                std::cout << "cameraPosition_z    = " << cameraPosition.z    << ";\n\n";

                std::cout << "shipPosition_x    = " << shipPosition.x    << ";\n";
                std::cout << "shipPosition_y    = " << shipPosition.y    << ";\n";
                std::cout << "shipPosition_z    = " << shipPosition.z    << ";\n\n";

                std::cout << "shipUpDirection_x    = " << shipUpDirection.x    << ";\n";
                std::cout << "shipUpDirection_y    = " << shipUpDirection.y    << ";\n";
                std::cout << "shipUpDirection_z    = " << shipUpDirection.z    << ";\n\n";

                std::cout << "cameraUpVector_x    = " << cameraUpVector.x    << ";\n";
                std::cout << "cameraUpVector_y    = " << cameraUpVector.y    << ";\n";
                std::cout << "cameraUpVector_z    = " << cameraUpVector.z    << ";\n\n";

            }
        } else {
            if((curDebounce == GLFW_KEY_L) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }


        if(glfwGetKey(window, GLFW_KEY_N)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_N;

                std::cout << "blinnUbo.mMat[2]_x    = " << glm::vec4(blinnUbo.mMat[2])[0]    << ";\n";
                std::cout << "blinnUbo.mMat[2]_y    = " << glm::vec4(blinnUbo.mMat[2])[1]    << ";\n";
                std::cout << "blinnUbo.mMat[2]_z    = " << glm::vec4(blinnUbo.mMat[2])[2]    << ";\n";
                std::cout << "blinnUbo.mMat[0]_x    = " << glm::vec4(blinnUbo.mMat[0])[0]    << ";\n";
                std::cout << "blinnUbo.mMat[0]_y    = " << glm::vec4(blinnUbo.mMat[0])[1]    << ";\n";
                std::cout << "blinnUbo.mMat[0]_z    = " << glm::vec4(blinnUbo.mMat[0])[2]    << ";\n\n";

            }
        } else {
            if((curDebounce == GLFW_KEY_N) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }


        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));

        DSship.map(currentImage, &blinnUbo, 0);

        blinnMatParUbo.Power = 200.0;
        DSship.map(currentImage, &blinnMatParUbo, 2);

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

                std::cout << "cameraUpVector_x      = " << cameraUpVector.x      << ";\n";
                std::cout << "cameraUpVector_y      = " << cameraUpVector.y      << ";\n";
                std::cout << "cameraUpVector_z      = " << cameraUpVector.z      << ";\n\n";

            }
        } else {
            if((curDebounce == GLFW_KEY_B) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }


        EmissionUniformBufferObject emissionUbo{};
        emissionUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), gubo.lightDir * 40.0f) * baseTr;
        DSsun.map(currentImage, &emissionUbo, 0);

        skyBoxUniformBufferObject sbubo{};
        sbubo.mvpMat = Mp * glm::mat4(glm::mat3(Mv));
        DSskyBox.map(currentImage, &sbubo, 0);

// **A10** Add to compute the uniforms and pass them to the shaders. You need two uniforms: one for the matrices, and the other for the material parameters.
        NormalUniformBufferObject nUbo{};

        // World and normal matrix should be the identity. The World-View-Projection should be variable ViewPrj
        nUbo.mMat = glm::mat4(1.0f) * glm::scale(glm::mat4(1), glm::vec3(4));
        nUbo.nMat = glm::mat4(1.0f);
        nUbo.mvpMat = ViewPrj * nUbo.mMat;

        // These informations should be used to fill the Uniform Buffer Object in Binding 0 of your DSL
        DS_earth.map(currentImage, &nUbo, 0);


        earthUniformBufferObject earthUbo{};

        // The specular power of the uniform buffer containing the material parameters of the new object should be set to:
        // XXX.Power = 200.0
        // Where you replace XXX.Power with the field of the local variable corresponding to the uniform buffer object
        earthUbo.pow = 200.0f;

        // The textre angle parameter of the uniform buffer containing the material parameters of the new object should be set to: tTime * TangTurnTimeFact
        // XXX.Ang = tTime * TangTurnTimeFact;
        // Where you replace XXX.Ang with the local field of the variable corresponding to the uniform buffer object
        earthUbo.ang = tTime * TangTurnTimeFact;

        // The selector for showing the clouds of the uniform buffer containing the material parameters of the new object should be set to:
        // XXX.ShowCloud = ShowCloud
        // Where you replace XXX.ShowCloud with the local field of the variable corresponding to the uniform buffer object
        earthUbo.showCloud = ShowCloud;

        // The selector for showing the clouds of the uniform buffer containing the material parameters of the new object should be set to:
        // XXX.ShowTexture = ShowTexture
        // Where you replace XXX.ShowTexture with the local field of the variable corresponding to the uniform buffer object
        earthUbo.showTexture = ShowTexture;

        // These informations should be used to fill the Uniform Buffer Object in Binding 6 of your DSL
        DS_earth.map(currentImage, &earthUbo, 6);
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
