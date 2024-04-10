#pragma once

#include <glm/glm.hpp>
#include "Type.hpp"
#include <vector>
#include "ObjectBase.h"

class SwcObject : public ObjectBase {
public:
    SwcObject() = default;

    ~SwcObject() = default;

    void load(const std::string &filePath);

    void draw() override;

    void update() override;

    void destroy() override;

    struct SwcData{
        int n;
        int type;
        glm::vec3 position;
        float radius;
        int parent;
        glm::vec3 color{1.0f, 1.0f, 1.0f};
    };
private:
    std::vector<SwcData> m_SwcNodeData;
};
