//
// Created by ace on 2024.12.18.
//

#include "Polyhedron.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

glm::mat4 PolyhedronFolder::PolyhedronFace::Transform(float t) {
    auto result = glm::mat4(1.0f);

    if (t < 0.0f || t > 1.0f) {
        throw std::invalid_argument("Invalid t value for interpolation.");
    }

    if(parent != nullptr) {
        const float radian = std::lerp(0.f,angle,t);
        const auto rotationMtx = glm::rotate(glm::mat4(1.0f), radian, glm::vec3(0.0f, 0.0f, 1.0f));

        result = parent->Transform(t) * transformMtx * rotationMtx;
    }

    return result;
}
