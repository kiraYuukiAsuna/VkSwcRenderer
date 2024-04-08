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
    m_Application->m_GraphicsDevice.m_Device.freeMemory(m_IndexBufferMemory);
    m_Application->m_GraphicsDevice.m_Device.destroyBuffer(m_IndexBuffer);
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
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void *data = m_Application->m_GraphicsDevice.m_Device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), (size_t) bufferSize);
    m_Application->m_GraphicsDevice.m_Device.unmapMemory(stagingBufferMemory);


    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, m_VertexBuffer, m_VertexBufferMemory);

    copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

    m_Application->m_GraphicsDevice.m_Device.destroyBuffer(stagingBuffer);
    m_Application->m_GraphicsDevice.m_Device.freeMemory(stagingBufferMemory);
}

void CommandBuffer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
                                 vk::Buffer &buffer, vk::DeviceMemory &bufferMemory) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    if (m_Application->m_GraphicsDevice.m_Device.createBuffer(&bufferInfo, nullptr, &buffer) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create buffer!");
    }

    vk::MemoryRequirements memRequirements;
    m_Application->m_GraphicsDevice.m_Device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Application->m_GraphicsDevice.findMemoryType(memRequirements.memoryTypeBits,
                                                                               properties);

    if (m_Application->m_GraphicsDevice.m_Device.allocateMemory(&allocInfo, nullptr, &bufferMemory) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    m_Application->m_GraphicsDevice.m_Device.bindBufferMemory(buffer, bufferMemory, 0);
}

void CommandBuffer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer;
    if (m_Application->m_GraphicsDevice.m_Device.allocateCommandBuffers(&allocInfo, &commandBuffer) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    if (commandBuffer.begin(&beginInfo) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    vk::BufferCopy copyRegion{};
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (m_Application->m_GraphicsDevice.m_GraphicsQueue.submit(1, &submitInfo, nullptr) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to submit copy buffer command buffer!");
    }

    m_Application->m_GraphicsDevice.m_GraphicsQueue.waitIdle();

    m_Application->m_GraphicsDevice.m_Device.freeCommandBuffers(m_CommandPool, 1, &commandBuffer);
}

void CommandBuffer::createIndexBuffer() {
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void *data = m_Application->m_GraphicsDevice.m_Device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), (size_t) bufferSize);
    m_Application->m_GraphicsDevice.m_Device.unmapMemory(stagingBufferMemory);

    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, m_IndexBuffer, m_IndexBufferMemory);

    copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

    m_Application->m_GraphicsDevice.m_Device.destroyBuffer(stagingBuffer);
    m_Application->m_GraphicsDevice.m_Device.freeMemory(stagingBufferMemory);
}
