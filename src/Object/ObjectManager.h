#pragma once

#include <vector>
#include "ObjectBase.h"
#include "Type.hpp"
#include "UI/Application.h"

class ObjectManager {
public:
    ~ObjectManager();

    static ObjectManager& getInstance() {
        static ObjectManager instance;
        return instance;
    }

    void addObject(ObjectBase* object) {
        m_ObjectList.push_back(object);
    }

    void draw() {
        pointVertices.clear();
        lineVertices.clear();
        for (auto&object: m_ObjectList) {
            object->draw();

            for (auto&node: object->nodeVertices) {
                pointVertices.push_back(node);
            }

            for(auto& line: object->lineVertices) {
                lineVertices.push_back(line);
            }
        }
        pointVertices.push_back(Vertex{{1, 1, 0}, {1.0, 1.0, 1.0}, 1.0});
        pointVertices.push_back(Vertex{{2, 2, 0}, {1.0, 1.0, 1.0}, 1.0});
        if (pointVertices.size() > 0) {
            createVertexBuffer(pointVertices, m_PointVertexBuffer, m_PointVertexBufferMemory);
            // createIndexBuffer(pointIndices, m_PointIndexBuffer, m_PointIndexBufferMemory);
        }
        if (lineVertices.size() > 0) {
            createVertexBuffer(lineVertices, m_LineVertexBuffer, m_LineVertexBufferMemory);
            // createIndexBuffer(lineIndices, m_LineIndexBuffer, m_LineIndexBufferMemory);
        }
        else {
            lineVertices.push_back(Vertex{{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 1.0});
            lineVertices.push_back(Vertex{{10.0, 10.0, 10.0}, {1.0, 1.0, 1.0}, 1.0});
            createVertexBuffer(lineVertices, m_LineVertexBuffer, m_LineVertexBufferMemory);
        }
        createUniformBuffers(m_UniformBuffers, m_UniformBuffersMemory);
        createDescriptorPool();
        createDescriptorSets();
    }

    void endDraw() {
        for (size_t i = 0; i < Application::getInstance().m_SwapChain.m_SwapChainImages.size(); i++) {
            Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_UniformBuffers[i]);
            Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_UniformBuffersMemory[i]);
        }

        if (pointVertices.size() > 0) {
            Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_PointVertexBuffer);
            Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_PointVertexBufferMemory);
            // Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_PointIndexBuffer);
            // Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_PointIndexBufferMemory);
        }
        if (lineVertices.size() > 0) {
            Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_LineVertexBuffer);
            Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_LineVertexBufferMemory);
            // Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_LineIndexBuffer);
            // Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_LineIndexBufferMemory);
        }
    }

    void update() {
        for (auto&object: m_ObjectList) {
            object->update();
        }
    }

    void removeObject(ObjectBase&object) {
    }

    std::vector<Vertex> pointVertices = {
        // {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        // {{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        // {{0.5f,  0.5f, 1.0f},  {0.0f, 1.0f, 0.0f}},
        // {{-0.5f, 0.5f, 0.0f},  {1.0f, 1.0f, 1.0f}}

    };

    std::vector<uint32_t> pointIndices = {}; //{0, 1, 2, 2, 3, 0};

    std::vector<Vertex> lineVertices = {
        // {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        // {{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        // {{0.5f,  0.5f, 1.0f},  {0.0f, 1.0f, 0.0f}},
        // {{-0.5f, 0.5f, 0.0f},  {1.0f, 1.0f, 1.0f}}

    };

    std::vector<uint32_t> lineIndices = {}; //{0, 1, 2, 2, 3, 0};

    vk::Buffer m_PointVertexBuffer;
    vk::DeviceMemory m_PointVertexBufferMemory;

    vk::Buffer m_PointIndexBuffer;
    vk::DeviceMemory m_PointIndexBufferMemory;

    vk::Buffer m_LineVertexBuffer;
    vk::DeviceMemory m_LineVertexBufferMemory;

    vk::Buffer m_LineIndexBuffer;
    vk::DeviceMemory m_LineIndexBufferMemory;

    std::vector<vk::Buffer> m_UniformBuffers;
    std::vector<vk::DeviceMemory> m_UniformBuffersMemory;

    vk::DescriptorPool m_DescriptorPool;

    std::vector<vk::DescriptorSet> m_DescriptorSets;

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    vk::CommandPool m_CommandPool;
    std::vector<vk::CommandBuffer> m_CommandBuffers;

    void updateUniformBuffer(uint32_t currentImage);

private:
    ObjectManager();

    void createDescriptorPool();

    void createDescriptorSets();

    void createVertexBuffer(std::vector<Vertex>&vertices, vk::Buffer&vertexBuffer, vk::DeviceMemory&vertexBufferMemory);

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
                      vk::Buffer&buffer, vk::DeviceMemory&bufferMemory);

    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

    void createIndexBuffer(std::vector<uint32_t>&indices, vk::Buffer&indexBuffer, vk::DeviceMemory&indexBufferMemory);

    void createUniformBuffers(std::vector<vk::Buffer>&uniformBuffer, std::vector<vk::DeviceMemory>&uniformBufferMemory);


    std::vector<ObjectBase *> m_ObjectList;
};
