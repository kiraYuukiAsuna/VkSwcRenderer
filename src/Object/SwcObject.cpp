#include "SwcObject.h"
#include "Util/FileIo/SwcIo.hpp"
#include "Type.hpp"
#include <vector>

void SwcObject::load(const std::string&filePath) {
    m_SwcNodeData.clear();

    ESwc swc(filePath);
    swc.ReadFromFile();

    auto units = swc.getValue();
    for (auto&unit: units) {
        SwcData data{
            unit.n,
            unit.type,
            glm::vec3{unit.x, unit.y , unit.z},
            unit.radius,
            unit.parent,
            glm::vec3{0.0f, 1.0f, 1.0f}
        };
        m_SwcNodeData.push_back(data);
    }
}

void SwcObject::draw() {
    for (auto&node: m_SwcNodeData) {
        nodeVertices.push_back({node.position, node.color});
    }

    for (auto&node: m_SwcNodeData) {
        if (node.parent != -1) {
            lineVertices.push_back({node.position, node.color});
            lineVertices.push_back({m_SwcNodeData[node.parent].position, m_SwcNodeData[node.parent].color});
        }
    }
}

void SwcObject::update() {
}

void SwcObject::destroy() {
}
