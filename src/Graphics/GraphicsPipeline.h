#pragma once

#include <vulkan/vulkan.hpp>
#include "ShaderCode.h"

class Application;

class GraphicsPipeline {
public:
    GraphicsPipeline(Application *application);

    ~GraphicsPipeline();

    void create();

    void createRenderPass();

    void createDescriptorSetLayout();

    void cleanup();

    vk::RenderPass m_RenderPass;
    vk::Pipeline m_SwcNodePipeline;
    vk::Pipeline m_SwcConnectLinePipeline;

    vk::DescriptorSetLayout m_DescriptorSetLayout;

    vk::PipelineLayout m_PipelineLayout;
private:
    Application *m_Application;

};
