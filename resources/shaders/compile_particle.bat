
set BASE_PATH=%~dp0

C:\VulkanSDK\1.3.290.0\Bin\glslc.exe %BASE_PATH%particle.vert -o %BASE_PATH%particle_vert.spv
C:\VulkanSDK\1.3.290.0\Bin\glslc.exe %BASE_PATH%particle.frag -o %BASE_PATH%particle_frag.spv
pause