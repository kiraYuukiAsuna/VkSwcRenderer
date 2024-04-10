#include "SwcObject.h"

#include <unordered_map>

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
    std::unordered_map<int, int> n_to_index_map;
    for (int i = 0; i < m_SwcNodeData.size(); ++i) {
        n_to_index_map[m_SwcNodeData[i].n] = i;
    }

    for (auto&node: m_SwcNodeData) {
        nodeVertices.push_back({node.position, node.color});
    }

    for (auto&node: m_SwcNodeData) {
        if (node.parent != -1) {
            auto iter = n_to_index_map.find(node.parent);
            if(iter==n_to_index_map.end()) {
                continue;
            }
            lineVertices.push_back({node.position, node.color});
            lineVertices.push_back({m_SwcNodeData[iter->second].position, m_SwcNodeData[iter->second].color});
        }
    }
}

void SwcObject::update() {
}

void SwcObject::destroy() {
}
