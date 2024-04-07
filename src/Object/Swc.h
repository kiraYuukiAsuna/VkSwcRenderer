#pragma once

#include <glm/glm.hpp>
#include "Type.hpp"
#include <vector>

class Swc {
public:


private:
//    vec2 positions[3] = vec2[](
//            vec2(0.0, -0.5),
//            vec2(0.5, 0.5),
//            vec2(-0.5, 0.5)
//    );
//
//    vec3 colors[3] = vec3[](
//            vec3(1.0, 0.0, 0.0),
//            vec3(0.0, 1.0, 0.0),
//            vec3(0.0, 0.0, 1.0)
//    );

    std::vector<Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

};
