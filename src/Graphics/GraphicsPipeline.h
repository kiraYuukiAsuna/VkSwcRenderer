#pragma once

#include <vulkan/vulkan.hpp>
#include "ShaderCode.h"

class Application;

class GraphicsPipeline {
public:
    GraphicsPipeline(Application *application);

    ~GraphicsPipeline();

    void create();

    void cleanup();

private:
    Application *m_Application;

    vk::PipelineLayout m_PipelineLayout;
};
