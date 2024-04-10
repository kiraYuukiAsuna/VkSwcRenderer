#pragma once
#include <vector>

#include "Type.hpp"

class ObjectBase {
public:
    ObjectBase() = default;

    virtual ~ObjectBase() = default;

    virtual void draw() = 0;

    virtual void update() = 0;

    virtual void destroy() = 0;

    std::vector<Vertex> nodeVertices;
    std::vector<Vertex> lineVertices;
};
