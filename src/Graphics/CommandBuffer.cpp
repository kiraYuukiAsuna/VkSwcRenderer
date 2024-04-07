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

void CommandBuffer::cleanupCommandBuffer() {
    m_Application->m_GraphicsDevice.m_Device.freeCommandBuffers(m_CommandPool,
                                                                static_cast<uint32_t>(m_CommandBuffers.size()),
                                                                m_CommandBuffers.data());
}

void CommandBuffer::cleanup() {
    m_Application->m_GraphicsDevice.m_Device.freeMemory(m_VertexBufferMemory);
    m_Application->m_GraphicsDevice.m_Device.destroyBuffer(m_VertexBuffer);
    cleanupCommandBuffer();
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

//    vk::Viewport viewport{};
//    viewport.x = 0.0f;
//    viewport.y = 0.0f;
//    viewport.width = (float) m_Application->m_SwapChain.m_SwapChainExtent.width;
//    viewport.height = (float) m_Application->m_SwapChain.m_SwapChainExtent.height;
//    viewport.minDepth = 0.0f;
//    viewport.maxDepth = 1.0f;
//
//    m_CommandBuffers[imageIndex].setViewport(0, 1, &viewport);
//
//    vk::Rect2D scissor{};
//    scissor.offset.setX(0);
//    scissor.offset.setY(0);
//    scissor.extent = m_Application->m_SwapChain.m_SwapChainExtent;
//    m_CommandBuffers[imageIndex].setScissor(0, 1, &scissor);

    vk::Buffer vertexBuffers[] = {m_VertexBuffer};
    vk::DeviceSize offsets[] = {0};
    m_CommandBuffers[imageIndex].bindVertexBuffers(0, 1, vertexBuffers, offsets);

    m_CommandBuffers[imageIndex].draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);

    m_CommandBuffers[imageIndex].endRenderPass();
    m_CommandBuffers[imageIndex].end();

}

void CommandBuffer::createVertexBuffer() {
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    m_VertexBuffer = m_Application->m_GraphicsDevice.m_Device.createBuffer(bufferInfo);

    vk::MemoryRequirements memRequirements = m_Application->m_GraphicsDevice.m_Device.getBufferMemoryRequirements(
            m_VertexBuffer);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Application->m_GraphicsDevice.findMemoryType(memRequirements.memoryTypeBits,
                                                                               vk::MemoryPropertyFlagBits::eHostVisible |
                                                                               vk::MemoryPropertyFlagBits::eHostCoherent);

    m_VertexBufferMemory = m_Application->m_GraphicsDevice.m_Device.allocateMemory(allocInfo);

    m_Application->m_GraphicsDevice.m_Device.bindBufferMemory(m_VertexBuffer, m_VertexBufferMemory, 0);

    void *data = m_Application->m_GraphicsDevice.m_Device.mapMemory(m_VertexBufferMemory, 0, bufferInfo.size);
    memcpy(data, vertices.data(), (size_t) bufferInfo.size);
    m_Application->m_GraphicsDevice.m_Device.unmapMemory(m_VertexBufferMemory);
}
