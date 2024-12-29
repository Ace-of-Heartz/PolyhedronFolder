//
// Created by ace on 2024.12.18..
//

#include "PolyUtils.h"
#include <iostream>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <cmath>
#include <glm/gtx/transform.hpp>
using namespace PolyhedronFolder;

typedef struct MeshObject<Vertex> Mesh;


Mesh PolyUtils::ConstrPolyFace(int n,float s) {

    std::vector<Vertex> vertexArray;
    std::vector<uint> indexArray;

    vertexArray.resize(n);
    indexArray.resize((n - 2) * 3);

    const float N = n;
    const float angleOfRotation = glm::two_pi<float>() / n;
    const float radius = s / (2.0f * glm::sin(glm::pi<float>()/N));
    float nth = 0;
    Vertex V;

    for (int i = 0; i < n; ++i) {

        auto pos = glm::vec3(glm::sin(angleOfRotation * nth) * radius,0,glm::cos(angleOfRotation * nth) * radius);
        auto uv = glm::vec2 ((glm::sin(glm::two_pi<float>() * nth/N) + 1.0f) / 2.0f, (glm::cos(glm::two_pi<float>() * nth / N) + 1.0f ) / 2.0f );

        V = Vertex{pos, glm::vec3(0.0f,1.0f,0.0f),uv};
        vertexArray[i] = V;
        nth += 1.0f;
    }


    for (int i = 0; i < (n - 2); i++) {
        indexArray[i*3] = 0;
        indexArray[i*3+1] = i+1;
        indexArray[i*3+2] = i+2;
    }

    return Mesh{vertexArray, indexArray};
}

glm::mat4 PolyUtils::CalcTransformMtx(uint toEdge, float n,float parentN) {
    glm::mat4 mtx = glm::mat4(1.0f);


    float aParent = 1.0f / (2.0f * glm::tan(glm::pi<float>()/parentN));
    float a = 1.0f / (2.0f * glm::tan(glm::pi<float>()/n));


    float angleOfRotation = glm::two_pi<float>() / parentN;
    float offset = angleOfRotation * 0.5f;

    float diff = ((1.0f - 2.0f / parentN) - (1.0f - 2.f / n)) * glm::half_pi<float>();

    mtx = glm::rotate(mtx,-diff,glm::vec3(0.0f,1.0f,0.0f));

    mtx = glm::rotate(glm::mat4(1), angleOfRotation * toEdge, glm::vec3(0.0f, 1.0f, 0.0f)) * mtx;
    mtx = glm::scale(glm::mat4(1),glm::vec3(-1.0f,1.0f,-1.0f)) * mtx;
    mtx = glm::translate(glm::mat4(1), glm::vec3( ( a + aParent) * glm::sin(offset + angleOfRotation * toEdge), 0.0f, ( a + aParent) * glm::cos(offset + angleOfRotation * toEdge) ))  * mtx;

    return mtx;
}

float PolyUtils::CalcDefaultAngleBetween(float n, float m, float o) {

    const float nAngle = (1.f - 2.f / n) * glm::pi<float>();
    const float mAngle = (1.f - 2.f / m) * glm::pi<float>();
    const float oAngle = (1.f - 2.f / o) * glm::pi<float>();

    return glm::pi<float>() - glm::acos(
        (glm::cos(oAngle) - glm::cos(nAngle) * glm::cos(mAngle)) / (glm::sin(nAngle) * glm::sin(mAngle))
    );
}

