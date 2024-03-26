#include "CommandBuffer.h"
#include "UI/Application.h"

CommandBuffer::CommandBuffer(Application *application) : m_Application(application) {

}

CommandBuffer::~CommandBuffer() {

}

void CommandBuffer::CreateCommandPool() {
    auto queryFamilyIndices = m_Application->m_GraphicsDevice.findQueueFamilies(
            m_Application->m_GraphicsDevice.m_PhysicalDevice);

    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.queueFamilyIndex = queryFamilyIndices.graphicsFamily;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    m_CommandPool = m_Application->m_GraphicsDevice.m_Device.createCommandPool(poolInfo);


}

void CommandBuffer::cleanup() {
    m_Application->m_GraphicsDevice.m_Device.destroyCommandPool(m_CommandPool);
}

void CommandBuffer::createCommandBuffers() {
    m_CommandBuffers.resize(m_Application->m_SwapChain.m_SwapChainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    m_CommandBuffers = m_Application->m_GraphicsDevice.m_Device.allocateCommandBuffers(allocInfo);


}

void CommandBuffer::recordCommandBuffers(uint32_t imageIndex) {
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    beginInfo.pInheritanceInfo = nullptr;

    m_CommandBuffers[imageIndex].begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderPass = m_Application->m_GraphicsPipeline.m_RenderPass;
    renderPassInfo.framebuffer = m_Application->m_SwapChain.m_SwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = m_Application->m_SwapChain.m_SwapChainExtent;

    vk::ClearValue clearColor;
    clearColor.color = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    m_CommandBuffers[imageIndex].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    m_CommandBuffers[imageIndex].bindPipeline(vk::PipelineBindPoint::eGraphics,
                                              m_Application->m_GraphicsPipeline.m_GraphicsPipeline);
    m_CommandBuffers[imageIndex].draw(3, 1, 0, 0);

    m_CommandBuffers[imageIndex].endRenderPass();
    m_CommandBuffers[imageIndex].end();

}
