cmake_minimum_required(VERSION 3.27)
project(SwcRenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
        ThirdParty/glfw/include
        ThirdParty/glm/include
        ThirdParty/VulkanMemoryAllocator/include
        ~/SDK/vulkan/1.3.275.0/x86_64/include
        src
)

link_directories(
        ThirdParty/glfw/lib/linux
        ~/SDK/vulkan/1.3.275.0/x86_64/lib
)

set(CPP_HEADER

)

set(CPP_SOURCE
        main.cpp
)

if (WIN32)

elseif (UNIX AND NOT APPLE)

elseif (APPLE)

endif ()

add_executable(SwcRenderer ${CPP_HEADER} ${CPP_SOURCE})

target_link_libraries(SwcRenderer
        glfw3
        vulkan
)
