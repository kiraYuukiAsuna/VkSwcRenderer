#include "CommandBuffer.h"
#include "UI/Application.h"

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>

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
    m_Application->m_GraphicsDevice.m_Device.destroyDescriptorPool(m_DescriptorPool);
    for (size_t i = 0; i < m_Application->m_SwapChain.m_SwapChainImages.size(); i++) {
        m_Application->m_GraphicsDevice.m_Device.destroyBuffer(m_UniformBuffers[i]);
        m_Application->m_GraphicsDevice.m_Device.freeMemory(m_UniformBuffersMemory[i]);
    }
    m_Application->m_GraphicsDevice.m_Device.destroyBuffer(m_IndexBuffer);
    m_Application->m_GraphicsDevice.m_Device.freeMemory(m_IndexBufferMemory);
    m_Application->m_GraphicsDevice.m_Device.destroyBuffer(m_VertexBuffer);
    m_Application->m_GraphicsDevice.m_Device.freeMemory(m_VertexBufferMemory);
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

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) m_Application->m_SwapChain.m_SwapChainExtent.width;
    viewport.height = (float) m_Application->m_SwapChain.m_SwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    m_CommandBuffers[imageIndex].setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset.setX(0);
    scissor.offset.setY(0);
    scissor.extent = m_Application->m_SwapChain.m_SwapChainExtent;
    m_CommandBuffers[imageIndex].setScissor(0, 1, &scissor);

    vk::Buffer vertexBuffers[] = {m_VertexBuffer};
    vk::DeviceSize offsets[] = {0};
    m_CommandBuffers[imageIndex].bindVertexBuffers(0, 1, vertexBuffers, offsets);

    m_CommandBuffers[imageIndex].bindIndexBuffer(m_IndexBuffer, 0, vk::IndexType::eUint32);

    m_CommandBuffers[imageIndex].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                    m_Application->m_GraphicsPipeline.m_PipelineLayout, 0, 1,
                                                    &m_DescriptorSets[imageIndex], 0, nullptr);

    m_CommandBuffers[imageIndex].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

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

void CommandBuffer::createUniformBuffers() {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    m_UniformBuffers.resize(m_Application->m_SwapChain.m_SwapChainImages.size());
    m_UniformBuffersMemory.resize(m_Application->m_SwapChain.m_SwapChainImages.size());

    for (size_t i = 0; i < m_Application->m_SwapChain.m_SwapChainImages.size(); i++) {
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     m_UniformBuffers[i], m_UniformBuffersMemory[i]);
    }
}

void CommandBuffer::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f),
                                m_Application->m_SwapChain.m_SwapChainExtent.width /
                                (float) m_Application->m_SwapChain.m_SwapChainExtent.height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    void *data = m_Application->m_GraphicsDevice.m_Device.mapMemory(m_UniformBuffersMemory[currentImage], 0,
                                                                    sizeof(ubo));
    memcpy(data, &ubo, sizeof(ubo));
    m_Application->m_GraphicsDevice.m_Device.unmapMemory(m_UniformBuffersMemory[currentImage]);
}

void CommandBuffer::createDescriptorPool() {
    vk::DescriptorPoolSize poolSize{};
    poolSize.type = vk::DescriptorType::eUniformBuffer;
    poolSize.descriptorCount = static_cast<uint32_t>(m_Application->m_SwapChain.m_SwapChainImages.size());

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(m_Application->m_SwapChain.m_SwapChainImages.size());

    m_DescriptorPool = m_Application->m_GraphicsDevice.m_Device.createDescriptorPool(poolInfo);
}

void CommandBuffer::createDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(m_Application->m_SwapChain.m_SwapChainImages.size(),
                                                  m_Application->m_GraphicsPipeline.m_DescriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_Application->m_SwapChain.m_SwapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();

    m_DescriptorSets.resize(m_Application->m_SwapChain.m_SwapChainImages.size());
    if (m_Application->m_GraphicsDevice.m_Device.allocateDescriptorSets(&allocInfo, m_DescriptorSets.data()) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < m_Application->m_SwapChain.m_SwapChainImages.size(); i++) {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_UniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = m_DescriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        m_Application->m_GraphicsDevice.m_Device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
    }
}


