cd shaders
glslc BlinnShader.frag -o     BlinnFrag.spv
glslc BlinnShader.vert -o     BlinnVert.spv
glslc EmissionShader.frag -o  EmissionFrag.spv
glslc EmissionShader.vert -o  EmissionVert.spv
glslc NormalMapShader.frag -o NormalMapFrag.spv
glslc NormalMapShader.vert -o NormalMapVert.spv
glslc SkyBoxShader.frag -o    SkyBoxFrag.spv
glslc SkyBoxShader.vert -o    SkyBoxVert.spv
glslc TextShader.frag -o      TextFrag.spv
glslc TextShader.vert -o      TextVert.spv