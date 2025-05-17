
del frag.spv
del vert.spv

set BASE_PATH=%~dp0

C:\VulkanSDK\1.3.290.0\Bin\glslc.exe %BASE_PATH%shader.vert -o %BASE_PATH%vert.spv
C:\VulkanSDK\1.3.290.0\Bin\glslc.exe %BASE_PATH%shader.frag -o %BASE_PATH%frag.spv
exit