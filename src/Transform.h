//
// Created by ace on 2025.01.03..
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>

class Transform {
public:
    glm::mat4 GetTransformMatrix() const;

    glm::mat4 GetInverseTransformMatrix() const;

    glm::vec3 GetTranslationComponent() const;
    glm::vec3 GetRotationComponent() const;
    glm::vec3 GetScalingComponent() const;

    void SetTranslationComponent(const glm::vec3& translation);
    void SetRotationComponent(const glm::vec3& rotation);
    void SetScalingComponent(const glm::vec3& scale);
private:
    glm::vec3 translation = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};



#endif //TRANSFORM_H
