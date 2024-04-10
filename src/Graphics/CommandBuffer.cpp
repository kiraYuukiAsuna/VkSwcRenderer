#include "CommandBuffer.h"
#include "UI/Application.h"
#include "Object/ObjectManager.h"

CommandBuffer::CommandBuffer(Application* application) : m_Application(application) {
}

CommandBuffer::~CommandBuffer() {
}

void CommandBuffer::CreateCommandPool() {
    auto queryFamilyIndices = m_Application->m_GraphicsDevice.findQueueFamilies(
        m_Application->m_GraphicsDevice.m_PhysicalDevice);

    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.queueFamilyIndex = queryFamilyIndices.graphicsFamily;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    ObjectManager::getInstance().m_CommandPool = m_Application->m_GraphicsDevice.m_Device.createCommandPool(poolInfo);
}

void CommandBuffer::cleanupCommandBuffer() {
    m_Application->m_GraphicsDevice.m_Device.freeCommandBuffers(ObjectManager::getInstance().m_CommandPool,
                                                                static_cast<uint32_t>(ObjectManager::getInstance().m_CommandBuffers.size()),
                                                                ObjectManager::getInstance().m_CommandBuffers.data());
}

void CommandBuffer::cleanup() {
    m_Application->m_GraphicsDevice.m_Device.destroyDescriptorPool(ObjectManager::getInstance().m_DescriptorPool);
    cleanupCommandBuffer();
    m_Application->m_GraphicsDevice.m_Device.destroyCommandPool(ObjectManager::getInstance().m_CommandPool);
}

void CommandBuffer::createCommandBuffers() {
    ObjectManager::getInstance().m_CommandBuffers.resize(m_Application->m_SwapChain.m_SwapChainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = ObjectManager::getInstance().m_CommandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = static_cast<uint32_t>(ObjectManager::getInstance().m_CommandBuffers.size());

    ObjectManager::getInstance().m_CommandBuffers = m_Application->m_GraphicsDevice.m_Device.allocateCommandBuffers(allocInfo);
}

void CommandBuffer::recordCommandBuffers(uint32_t imageIndex, std::vector<vk::DescriptorSet>& descriptorSets) {
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    beginInfo.pInheritanceInfo = nullptr;

    ObjectManager::getInstance().m_CommandBuffers[imageIndex].begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderPass = m_Application->m_GraphicsPipeline.m_RenderPass;
    renderPassInfo.framebuffer = m_Application->m_SwapChain.m_SwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = m_Application->m_SwapChain.m_SwapChainExtent;

    vk::ClearValue clearColor;
    clearColor.color = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    ObjectManager::getInstance().m_CommandBuffers[imageIndex].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    ObjectManager::getInstance().m_CommandBuffers[imageIndex].bindPipeline(vk::PipelineBindPoint::eGraphics,
                                              m_Application->m_GraphicsPipeline.m_SwcNodePipeline);

    ObjectManager::getInstance().draw();
    ObjectManager::getInstance().updateUniformBuffer(imageIndex);

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_Application->m_SwapChain.m_SwapChainExtent.width;
    viewport.height = (float)m_Application->m_SwapChain.m_SwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    ObjectManager::getInstance().m_CommandBuffers[imageIndex].setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset.setX(0);
    scissor.offset.setY(0);
    scissor.extent = m_Application->m_SwapChain.m_SwapChainExtent;
    ObjectManager::getInstance().m_CommandBuffers[imageIndex].setScissor(0, 1, &scissor);

    vk::Buffer pointVertexBuffers[] = {ObjectManager::getInstance().m_PointVertexBuffer};
    vk::DeviceSize pointOffsets[] = {0};
    ObjectManager::getInstance().m_CommandBuffers[imageIndex].bindVertexBuffers(0, 1, pointVertexBuffers, pointOffsets);

    //    ObjectManager::getInstance().m_CommandBuffers[imageIndex].bindIndexBuffer(m_IndexBuffer, 0, vk::IndexType::eUint32);

    ObjectManager::getInstance().m_CommandBuffers[imageIndex].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                    m_Application->m_GraphicsPipeline.m_PipelineLayout, 0, 1,
                                                    &descriptorSets[imageIndex], 0, nullptr);

    //    ObjectManager::getInstance().m_CommandBuffers[imageIndex].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    ObjectManager::getInstance().m_CommandBuffers[imageIndex].draw(static_cast<uint32_t>(ObjectManager::getInstance().pointVertices.size()), 1, 0,
                                      0);

    ObjectManager::getInstance().m_CommandBuffers[imageIndex].endRenderPass();



    ObjectManager::getInstance().m_CommandBuffers[imageIndex].end();


    // ObjectManager::getInstance().endDraw();

}
