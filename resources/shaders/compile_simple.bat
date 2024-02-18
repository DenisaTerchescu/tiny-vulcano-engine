
set BASE_PATH=%~dp0

D:\VulkanSDK\1.3.275.0\Bin\glslc.exe %BASE_PATH%shader.vert -o %BASE_PATH%shader.vert.spv
D:\VulkanSDK\1.3.275.0\Bin\glslc.exe %BASE_PATH%shader.frag -o %BASE_PATH%shader.frag.spv
pause