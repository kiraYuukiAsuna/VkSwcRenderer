cmake_minimum_required(VERSION 3.27)
project(VkSwcRenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
        ThirdParty/glfw/include
        ThirdParty/glm/include
        ThirdParty/VulkanMemoryAllocator/include
        ThirdParty/spdlog/include
        ThirdParty/imgui-1.90.4
        ~/SDK/vulkan/1.3.275.0/x86_64/include
        C:/VulkanSDK/1.3.275.0/Include
        src
)

if (WIN32)
    link_directories(
            ThirdParty/glfw/lib/windows
            C:/VulkanSDK/1.3.275.0/Lib
    )
elseif (UNIX AND NOT APPLE)
    link_directories(
            ThirdParty/glfw/lib/linux
            ~/SDK/vulkan/1.3.275.0/x86_64/lib
    )
elseif (APPLE)

endif ()


set(CPP_HEADER

)

set(CPP_SOURCE
        main.cpp

        src/Util/Logger/Log.cpp
        ThirdParty/imgui-1.90.4/imgui.cpp
        ThirdParty/imgui-1.90.4/imgui_draw.cpp
        ThirdParty/imgui-1.90.4/imgui_demo.cpp
        ThirdParty/imgui-1.90.4/imgui_tables.cpp
        ThirdParty/imgui-1.90.4/imgui_widgets.cpp

        src/UI/Application.cpp
        src/Graphics/GraphicsDevice.cpp
        src/Graphics/WindowSurface.cpp
        src/Graphics/SwapChain.cpp
        src/Graphics/GraphicsPipeline.cpp
        src/Graphics/ShaderCode.cpp
        src/Graphics/CommandBuffer.cpp
)

add_executable(${PROJECT_NAME} ${CPP_HEADER} ${CPP_SOURCE})

if (WIN32)
    target_link_libraries(${PROJECT_NAME}
            glfw3
            vulkan-1.lib
    )
elseif (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME}
            glfw3
            vulkan
    )
elseif (APPLE)

endif ()
