//
// Created by ace on 2024.12.18.
//

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "Polyhedron.h"

#include <iostream>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "PolyUtils.h"

using namespace glm;
using namespace PolyhedronFolder;

PolyhedronFace::PolyhedronFace(
    const uint n,
    const float pivotVal,
    const glm::mat4 &tfMat,
    PolyhedronFace* parent
    )
: numberOfEdges(n), pivotVal(pivotVal), localTransformMtx(tfMat), parent(parent), mesh(PolyUtils::ConstrPolyFace(n)) {
    children.resize(n);

}

PolyhedronFace::~PolyhedronFace() {
}

void PolyhedronFace::Add(uint edge, uint n, float pivotVal) {

    auto tfMat = PolyUtils::CalcTransformMtx(edge,n);

    children[edge] = new PolyhedronFace(n,pivotVal,tfMat,this); //TODO: Calc transform matrix ....
}

PolyhedronFace * PolyhedronFace::Push(uint edge, const uint n, float pivotVal) {
    auto tfMat = PolyUtils::CalcTransformMtx(edge,n);

    children[edge] = new PolyhedronFace(n,pivotVal,tfMat,this); // TODO: Calc transform matrix ....
    return children[edge];
}

Mesh PolyhedronFace::GetTransformedMesh(const float t, const glm::mat4& parentTransformMtx) {
    glm::mat4 tfMat = glm::mat4(1);
    if (parent)
        tfMat = GetFoldTransformationMatrix(t);

    std::vector<Vertex> transformedVertices;
    transformedVertices.reserve(mesh.vertexArray.size());

    tfMat = parentTransformMtx * localTransformMtx * tfMat;

    for(auto &v : mesh.vertexArray) {
        auto tfPos = tfMat * glm::vec4(v.position,1.0);
        transformedVertices.push_back({ glm::vec3(tfPos.x,tfPos.y,tfPos.z),v.normal,v.texcoord});
    }

    auto mMesh = Mesh{transformedVertices,mesh.indexArray};

    for(auto &c : children) {
        if (c) {
            auto cMesh = c->GetTransformedMesh(t,tfMat);
            mMesh += cMesh;
        }
    }

    if (!parent) {
        for(auto &i : mMesh.vertexArray) {
            std::cout << i.position.x << " " << i.position.y << " " << i.position.z << std::endl;
        }
        std::cout << "---" << std::endl;
        for (auto &i : mMesh.indexArray) {
            std:: cout << i << std::endl;
        }
    }


    return mMesh;
}


glm::mat4 PolyhedronFace::GetFoldTransformationMatrix(const float t) const {
    auto result = glm::mat4(1.0f);

    if (t < 0.0f || t > 1.0f) {
        throw std::invalid_argument("Invalid t value for interpolation.");
    }

    if(parent) {
        const float radian = std::lerp(0.f,pivotVal,t);
        const auto rotationMtx = glm::rotate(glm::mat4(1.0f), radian, glm::vec3(0.0f, 0.0f, 1.0f));
        result = rotationMtx * result;
    }

    return result;
}

Polyhedron::Polyhedron() : isDirty(true) {};

Mesh Polyhedron::GetTransformedMesh()  {
    auto res = Mesh();

    if (root) {
        res = root->GetTransformedMesh(foldVal,glm::mat4(1));
    }
    isDirty = false;

    return res;
}

