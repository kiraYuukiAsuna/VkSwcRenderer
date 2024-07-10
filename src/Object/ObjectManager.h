#pragma once

#include <numeric>
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

    void draw(uint32_t imageIndex) {
        static std::once_flag flag;
        std::call_once(flag,
                       [this]() {
                           pointVertices.clear();
                           lineVertices.clear();
                           pointIndices.clear();
                           lineIndices.clear();

                           for (auto&object: m_ObjectList) {
                               object->draw();

                               for (auto&node: object->nodeVertices) {
                                   pointVertices.push_back(node);
                               }

                               for (auto&line: object->lineVertices) {
                                   lineVertices.push_back(line);
                               }
                           }

                           if (pointVertices.empty()) return; // 确保向量不为空

                           // pointVertices.push_back(Vertex{{1, 1, 0}, {1.0, 1.0, 1.0}, 1.0});
                           // pointVertices.push_back(Vertex{{2, 2, 0}, {1.0, 1.0, 1.0}, 1.0});
                           if (pointVertices.size() > 0) {
                               createVertexBuffer(pointVertices, m_PointVertexBuffer, m_PointVertexBufferMemory);
                               // pointIndices.resize(pointVertices.size());
                               // std::iota(pointIndices.begin(), pointIndices.end(), 0);
                               // createIndexBuffer(pointIndices, m_PointIndexBuffer, m_PointIndexBufferMemory);
                           }
                           if (lineVertices.size() > 0) {
                               createVertexBuffer(lineVertices, m_LineVertexBuffer, m_LineVertexBufferMemory);
                               // lineIndices.resize(lineVertices.size());
                               // std::iota(lineIndices.begin(), lineIndices.end(), 0);
                               // createIndexBuffer(lineIndices, m_LineIndexBuffer, m_LineIndexBufferMemory);
                           }
                           else {
                               lineVertices.push_back(Vertex{{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 1.0});
                               lineVertices.push_back(Vertex{{10.0, 10.0, 10.0}, {1.0, 1.0, 1.0}, 1.0});
                               createVertexBuffer(lineVertices, m_LineVertexBuffer, m_LineVertexBufferMemory);
                           }
                       });


        updateUniformBuffer(imageIndex);
    }


    void normalizeVertices(std::vector<Vertex>&vertices) {
        float minValX = std::min_element(vertices.begin(), vertices.end(),
                                         [](const Vertex&a, const Vertex&b) { return a.pos.x < b.pos.x; })->pos.x;
        float maxValX = std::max_element(vertices.begin(), vertices.end(),
                                         [](const Vertex&a, const Vertex&b) { return a.pos.x < b.pos.x; })->pos.x;

        float minValY = std::min_element(vertices.begin(), vertices.end(),
                                         [](const Vertex&a, const Vertex&b) { return a.pos.y < b.pos.y; })->pos.y;
        float maxValY = std::max_element(vertices.begin(), vertices.end(),
                                         [](const Vertex&a, const Vertex&b) { return a.pos.y < b.pos.y; })->pos.y;

        for (auto&vertex: vertices) {
            vertex.pos.x = (vertex.pos.x - minValX) / (maxValX - minValX);
            vertex.pos.y = (vertex.pos.y - minValY) / (maxValY - minValY);
        }
    }

    void endDraw(uint32_t imageIndex) {
        // if (!m_UniformBuffers.empty() && m_UniformBuffers[imageIndex]) {
        //     Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_UniformBuffers[imageIndex]);
        // }
        // if (!m_UniformBuffersMemory.empty() && m_UniformBuffersMemory[imageIndex]) {
        //     Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_UniformBuffersMemory[imageIndex]);
        // }

        normalizeVertices(pointVertices);
        normalizeVertices(lineVertices);
        if (pointVertices.size() > 0) {
            if (m_PointVertexBuffer)
                Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_PointVertexBuffer);
            if (m_PointVertexBufferMemory)
                Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_PointVertexBufferMemory);
            if (m_PointIndexBuffer)
                Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_PointIndexBuffer);
            if (m_PointIndexBufferMemory)
                Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_PointIndexBufferMemory);
        }
        if (lineVertices.size() > 0) {
            if (m_LineVertexBuffer)
                Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_LineVertexBuffer);
            if (m_LineVertexBufferMemory)
                Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_LineVertexBufferMemory);
            if (m_LineIndexBuffer)
                Application::getInstance().m_GraphicsDevice.m_Device.destroyBuffer(m_LineIndexBuffer);
            if (m_LineIndexBufferMemory)
                Application::getInstance().m_GraphicsDevice.m_Device.freeMemory(m_LineIndexBufferMemory);
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
        {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}

    };

    std::vector<uint32_t> pointIndices = {0, 1, 2, 2, 3, 0};

    std::vector<Vertex> lineVertices = {
        {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}

    };

    std::vector<uint32_t> lineIndices = {0, 1, 2, 2, 3, 0};

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

    void createResource();

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
