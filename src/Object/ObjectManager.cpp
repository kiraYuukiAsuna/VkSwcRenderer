#include "ObjectManager.h"
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include "UI/Application.h"

void ObjectManager::createVertexBuffer(std::vector<Vertex>&vertices, vk::Buffer&vertexBuffer,
                                       vk::DeviceMemory&vertexBufferMemory) {
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void* data = Application::getInstance().m_GraphicsDevice.m_Device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    Application::getInstance().m_GraphicsDevice.m_Device.unmapMemory(stagingBufferMemory);

    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(stagingBuffer);
    Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(stagingBufferMemory);
}

void ObjectManager::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
                                 vk::Buffer&buffer, vk::DeviceMemory&bufferMemory) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    if (Application::getInstance().m_GraphicsDevice.m_Device.createBuffer(&bufferInfo, nullptr, &buffer) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create buffer!");
    }

    vk::MemoryRequirements memRequirements;
    Application::getInstance().m_GraphicsDevice.m_Device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Application::getInstance().m_GraphicsDevice.findMemoryType(
        memRequirements.memoryTypeBits,
        properties);

    if (Application::getInstance().m_GraphicsDevice.m_Device.allocateMemory(&allocInfo, nullptr, &bufferMemory) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    Application::getInstance().m_GraphicsDevice.m_Device.bindBufferMemory(buffer, bufferMemory, 0);
}

void ObjectManager::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer CommandBuffer;
    if (Application::getInstance().m_GraphicsDevice.m_Device.allocateCommandBuffers(&allocInfo, &CommandBuffer) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    if (CommandBuffer.begin(&beginInfo) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    vk::BufferCopy copyRegion{};
    copyRegion.size = size;
    CommandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    CommandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffer;

    if (Application::getInstance().m_GraphicsDevice.m_GraphicsQueue.submit(1, &submitInfo, nullptr) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to submit copy buffer command buffer!");
    }

    Application::getInstance().m_GraphicsDevice.m_GraphicsQueue.waitIdle();

    Application::getInstance().m_GraphicsDevice.m_Device.freeCommandBuffers(m_CommandPool, 1, &CommandBuffer);
}

void ObjectManager::createIndexBuffer(std::vector<uint32_t>&indices, vk::Buffer&indexBuffer,
                                      vk::DeviceMemory&indexBufferMemory) {
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void* data = Application::getInstance().m_GraphicsDevice.m_Device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), (size_t)bufferSize);
    Application::getInstance().m_GraphicsDevice.m_Device.unmapMemory(stagingBufferMemory);

    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(stagingBuffer);
    Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(stagingBufferMemory);
}

void ObjectManager::createUniformBuffers(std::vector<vk::Buffer>&uniformBuffer,
                                         std::vector<vk::DeviceMemory>&uniformBufferMemory) {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffer.resize(Application::getInstance().m_SwapChain.m_SwapChainImages.size());
    uniformBufferMemory.resize(Application::getInstance().m_SwapChain.m_SwapChainImages.size());

    for (size_t i = 0; i < Application::getInstance().m_SwapChain.m_SwapChainImages.size(); i++) {
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     uniformBuffer[i], uniformBufferMemory[i]);
    }
}

void ObjectManager::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    // ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
    //                         glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::mat4(1.0f);
    // View matrix
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 targetPos = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    ubo.view = glm::lookAt(cameraPos, targetPos, upVector);

    // Projection matrix
    float fov = glm::radians(60.0f);
    float aspect = static_cast<float>(Application::getInstance().m_SwapChain.m_SwapChainExtent.width) / (float)Application::getInstance().m_SwapChain.m_SwapChainExtent.height; // replace windowWidth and windowHeight with your actual window size
    float nearPlane = 0.0f;
    float farPlane = 20000.0f;
    ubo.proj = glm::perspective(fov, aspect, nearPlane, farPlane);

    ubo.proj[1][1] *= -1;

    void* data = Application::getInstance().m_GraphicsDevice.m_Device.mapMemory(m_UniformBuffersMemory[currentImage], 0,
        sizeof(ubo));
    memcpy(data, &ubo, sizeof(ubo));
    Application::getInstance().m_GraphicsDevice.m_Device.unmapMemory(m_UniformBuffersMemory[currentImage]);
}

ObjectManager::~ObjectManager() {

}

ObjectManager::ObjectManager() {
}

void ObjectManager::createDescriptorPool() {
    vk::DescriptorPoolSize poolSize{};
    poolSize.type = vk::DescriptorType::eUniformBuffer;
    poolSize.descriptorCount = static_cast<uint32_t>(Application::getInstance().m_SwapChain.m_SwapChainImages.size());

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(Application::getInstance().m_SwapChain.m_SwapChainImages.size());

    m_DescriptorPool = Application::getInstance().m_GraphicsDevice.m_Device.createDescriptorPool(poolInfo);
}

void ObjectManager::createDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(Application::getInstance().m_SwapChain.m_SwapChainImages.size(),
                                                 Application::getInstance().m_GraphicsPipeline.m_DescriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(Application::getInstance().m_SwapChain.m_SwapChainImages.
        size());
    allocInfo.pSetLayouts = layouts.data();

    m_DescriptorSets.resize(Application::getInstance().m_SwapChain.m_SwapChainImages.size());
    if (Application::getInstance().m_GraphicsDevice.m_Device.allocateDescriptorSets(&allocInfo, m_DescriptorSets.data())
        !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < Application::getInstance().m_SwapChain.m_SwapChainImages.size(); i++) {
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

        Application::getInstance().m_GraphicsDevice.m_Device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
    }
}
