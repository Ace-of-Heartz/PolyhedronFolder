//
// Created by ace on 2025.01.03..
//

#include "Transform.h"

#include <glm/gtx/transform.hpp>

glm::mat4 Transform::GetTransformMatrix() const
{
    return translate(translation) * rotate(glm::radians(rotation.z),glm::vec3(0.0f,0.0f,1.0f)) * rotate(glm::radians(rotation.y),glm::vec3(0.0f,1.0f,0.0f)) * rotate(glm::radians(rotation.x),glm::vec3(1.0f,0.0f,0.0f)) * glm::scale(scale) * glm::mat4(1.0f);
}
glm::mat4 Transform::GetInverseTransformMatrix() const
{
    return inverse(GetInverseTransformMatrix());
}



glm::vec3 Transform::GetTranslationComponent() const
{
    return translation;
}

glm::vec3 Transform::GetRotationComponent() const
{
    return rotation;
}

glm::vec3 Transform::GetScalingComponent() const
{
    return scale;
}

void Transform::SetTranslationComponent(const glm::vec3& translation)
{
    this->translation = translation;
}

void Transform::SetScalingComponent(const glm::vec3& scale)
{
    this->scale = scale;
}

void Transform::SetRotationComponent(const glm::vec3& rotation)
{
    this->rotation = rotation;
}

