VK_SDK="include/VulkanSDK"
export VULKAN_SDK=$VK_SDK/Windows
export VULKAN_LIBRARY="$VK_SDK/Windows/Lib"
export VULKAN_INCLUDE_DIR="$VK_SDK/Windows/Include" 
dir=$(pwd)
ln -s "$dir/$VK_SDK/Windows/Lib" "$dir/$VK_SDK/Windows/lib"
ln -s "$dir/$VK_SDK/Include" "$dir/$VK_SDK/Windows/Include"
ln -s "$dir/$VK_SDK/Windows/Include" "$dir/$VK_SDK/Windows/include"

if [[ -d build ]];
then 
    rm -rf build
fi

mkdir build && cd build
cmake -D CMAKE_TOOLCHAIN_FILE=../windows.cmake .. && make
