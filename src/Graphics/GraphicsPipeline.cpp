#include "GraphicsPipeline.h"
#include "Util/Logger/Log.h"
#include "UI/Application.h"

GraphicsPipeline::GraphicsPipeline(Application *application) : m_Application(application) {

}

GraphicsPipeline::~GraphicsPipeline() {

}

void GraphicsPipeline::create() {
    auto vertShaderCode = ShaderCode::readShaderFile("../src/Shader/swc_shader.vert.spv");
    auto fragShaderCode = ShaderCode::readShaderFile("../src/Shader/swc_shader.frag.spv");

    auto vertShaderModule = ShaderCode::createShaderModule(vertShaderCode, m_Application->m_GraphicsDevice.m_Device);
    auto fragShaderModule = ShaderCode::createShaderModule(fragShaderCode, m_Application->m_GraphicsDevice.m_Device);

    SEELE_INFO("Vertex Shader Code Size: {0}", vertShaderCode.size());
    SEELE_INFO("Fragment Shader Code Size: {0}", fragShaderCode.size());

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.pSpecializationInfo = nullptr;

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.pSpecializationInfo = nullptr;

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    vk::PipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo;
    assemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
    assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) m_Application->m_SwapChain.m_SwapChainExtent.width;
    viewport.height = (float) m_Application->m_SwapChain.m_SwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor;
    scissor.offset = 0;
    scissor.extent = m_Application->m_SwapChain.m_SwapChainExtent;

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.depthClampEnable = false;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = false;
    rasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationStateCreateInfo.lineWidth = 1.0;
    rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
    rasterizationStateCreateInfo.frontFace = vk::FrontFace::eClockwise;
    rasterizationStateCreateInfo.depthBiasEnable = false;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0;

    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.sampleShadingEnable = false;
    multisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampleStateCreateInfo.minSampleShading = 1.0;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = false;
    multisampleStateCreateInfo.alphaToOneEnable = false;

    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;


    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
    colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                                               | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachmentState.blendEnable = false;
    colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    colorBlendStateCreateInfo.logicOpEnable = false;
    colorBlendStateCreateInfo.logicOp = vk::LogicOp::eCopy;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendStateCreateInfo.blendConstants[0] = 0.0;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0;
    colorBlendStateCreateInfo.blendConstants[2] = 0.0;
    colorBlendStateCreateInfo.blendConstants[3] = 0.0;

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


    if (m_Application->m_GraphicsDevice.m_Device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr,
                                                                      &m_PipelineLayout) != vk::Result::eSuccess) {
        SEELE_ERROR_TAG(__func__, "Create Pipeline Layout Failed!");
    }

    m_Application->m_GraphicsDevice.m_Device.destroyShaderModule(vertShaderModule);
    m_Application->m_GraphicsDevice.m_Device.destroyShaderModule(fragShaderModule);
}

void GraphicsPipeline::cleanup() {
    m_Application->m_GraphicsDevice.m_Device.destroyPipelineLayout(m_PipelineLayout, nullptr);
}
