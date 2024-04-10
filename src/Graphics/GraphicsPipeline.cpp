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

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo;
    assemblyStateCreateInfo.topology = vk::PrimitiveTopology::ePointList;
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
    rasterizationStateCreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
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

    std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();


    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


    if (m_Application->m_GraphicsDevice.m_Device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr,
                                                                      &m_PipelineLayout) != vk::Result::eSuccess) {
        SEELE_ERROR_TAG(__func__, "Create Pipeline Layout Failed!");
    }

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &assemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = m_PipelineLayout;
    pipelineCreateInfo.renderPass = m_RenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = -1;

    if (m_Application->m_GraphicsDevice.m_Device.createGraphicsPipelines(nullptr, 1, &pipelineCreateInfo, nullptr,
                                                                         &m_SwcNodePipeline) != vk::Result::eSuccess) {
        SEELE_ERROR_TAG(__func__, "Create SwcNode Graphics Pipeline Failed!");
    }

    assemblyStateCreateInfo.topology = vk::PrimitiveTopology::eLineList;
    if (m_Application->m_GraphicsDevice.m_Device.createGraphicsPipelines(nullptr, 1, &pipelineCreateInfo, nullptr,
                                                                         &m_SwcConnectLinePipeline) != vk::Result::eSuccess) {
        SEELE_ERROR_TAG(__func__, "Create SwcConnectLine Graphics Pipeline Failed!");
    }

    m_Application->m_GraphicsDevice.m_Device.destroyShaderModule(vertShaderModule);
    m_Application->m_GraphicsDevice.m_Device.destroyShaderModule(fragShaderModule);
}

void GraphicsPipeline::cleanup() {
    m_Application->m_GraphicsDevice.m_Device.destroyDescriptorSetLayout(m_DescriptorSetLayout, nullptr);
    m_Application->m_GraphicsDevice.m_Device.destroyPipeline(m_SwcConnectLinePipeline, nullptr);
    m_Application->m_GraphicsDevice.m_Device.destroyPipeline(m_SwcNodePipeline, nullptr);
    m_Application->m_GraphicsDevice.m_Device.destroyPipelineLayout(m_PipelineLayout, nullptr);
    m_Application->m_GraphicsDevice.m_Device.destroyRenderPass(m_RenderPass, nullptr);
}

void GraphicsPipeline::createRenderPass() {
    vk::AttachmentDescription attachmentDescription;
    attachmentDescription.format = m_Application->m_SwapChain.m_SwapChainImageFormat;
    attachmentDescription.samples = vk::SampleCountFlagBits::e1;
    attachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    attachmentDescription.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpassDescription;
    subpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentRef;

    vk::RenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &attachmentDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;

    vk::SubpassDependency subpassDependency;
    subpassDependency.srcSubpass = vk::SubpassExternal;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.dstAccessMask =
            vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    if (m_Application->m_GraphicsDevice.m_Device.createRenderPass(&renderPassCreateInfo, nullptr, &m_RenderPass) !=
        vk::Result::eSuccess) {
        SEELE_ERROR_TAG(__func__, "Create Render Pass Failed!");
    }

}

void GraphicsPipeline::createDescriptorSetLayout() {
    vk::DescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (m_Application->m_GraphicsDevice.m_Device.createDescriptorSetLayout(&layoutInfo, nullptr,
                                                                           &m_DescriptorSetLayout) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}
