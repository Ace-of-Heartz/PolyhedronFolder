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
    const uint edge,
    PolyhedronFace* parent,
    bool isRoot
    )
:  parentEdgeIndex(edge),numberOfEdges(n), pivotVal(pivotVal), localTransformMtx(tfMat), parent(parent), mesh(PolyUtils::ConstrPolyFace(n,1)) {
    children.resize(n);
}


PolyhedronFace::~PolyhedronFace() {
}

void PolyhedronFace::SetNumberOfEdges(uint newNumberOfEdges)
{
    this->mesh = PolyUtils::ConstrPolyFace(newNumberOfEdges, 1);

    if (parent)
    {
        this->localTransformMtx = PolyUtils::CalcTransformMtx(parentEdgeIndex,newNumberOfEdges,parent->GetEdgeCount());
    }

    std::vector<PolyhedronFace*> newChildren;
    newChildren.resize(newNumberOfEdges);

    uint i = 0;
    for (; i < newNumberOfEdges && i < this->numberOfEdges; i++)
    {
        if(children[i])
        {
            this->children[i]->SetTransformMatrix(PolyUtils::CalcTransformMtx(i,this->children[i]->GetEdgeCount(),newNumberOfEdges));
            newChildren[i] = this->children[i];

        }
    }

    for (; i < this->numberOfEdges; i++)
    {
        if(children[i])
        {
            this->Remove(i);
        }
    }

    this->numberOfEdges = newNumberOfEdges;
    this->children = newChildren;

}

void PolyhedronFace::SetPivotVal(const float pivotVal)
{
    this->pivotVal = pivotVal;
}

void PolyhedronFace::SetTransformMatrix(const mat4& transformMatrix)
{
    this->localTransformMtx = transformMatrix;
}

void PolyhedronFace::Add(uint edge, uint n, float pivotVal) {
    //TODO: Make this nicer
    if (edge == 0 && parent != nullptr || children[edge] != nullptr) {
        throw std::invalid_argument("PolyhedronFace::Add : Bad edge index");
    }

    auto tfMat = PolyUtils::CalcTransformMtx(edge,n,numberOfEdges);

    children[edge] = new PolyhedronFace(n,pivotVal,tfMat,edge,this,false); //TODO: Calc transform matrix ....
}

PolyhedronFace* PolyhedronFace::Push(uint edge, const uint n, float pivotVal) {

    if (edge == 0 && parent != nullptr || children[edge] != nullptr) {
        throw std::invalid_argument("PolyhedronFace::Push : Bad edge index ");
    }

    auto tfMat = PolyUtils::CalcTransformMtx(edge,n,numberOfEdges);

    children[edge] = new PolyhedronFace(n,pivotVal,tfMat,edge,this,false); // TODO: Calc transform matrix ....
    return children[edge];
}

void PolyhedronFace::Remove(const uint edge)
{
    if (children[edge])
    {
        auto temp = children[edge];
        children[edge] = nullptr;
        delete temp;
    }
}


Mesh PolyhedronFace::GetTransformedMesh(
    const float t,
    const glm::mat4& parentTransformMtx,
    const glm::vec3 cameraPos) {

    auto tfMat = glm::mat4(1);
    if (parent)
    {
        if (freezeState == UNFREEZE)
            tfMat = GetFoldTransformationMatrix(t);
        else
            tfMat = GetFoldTransformationMatrix(animationState);
    }

    std::vector<Vertex> transformedVertices;
    transformedVertices.reserve(mesh.vertexArray.size());

    tfMat = parentTransformMtx * localTransformMtx  * tfMat;
    auto tfMatTransposeI = glm::transpose(glm::inverse(tfMat));

    bool isFacingAway;
    for(auto &v : mesh.vertexArray) {

        //Transform vertices
        auto tfPos = tfMat * glm::vec4(v.position,1.0);

        //Transform normals
        auto tfNorm = normalize(tfMatTransposeI * glm::vec4(v.normal,1.0));

        //Check if face is facing away
        auto posToCamera = cameraPos - glm::vec3(tfPos.x,tfPos.y,tfPos.z);
        isFacingAway = glm::dot(posToCamera,glm::vec3(tfNorm.x,tfNorm.y,tfNorm.z)) < 0.0f;



        if(isFacingAway) {
            tfNorm *= -1.0f;
        }

        transformedVertices.push_back({ glm::vec3(tfPos.x,tfPos.y,tfPos.z) / tfPos.w,glm::vec3(tfNorm.x,tfNorm.y,tfNorm.z) / abs(tfNorm.w),v.texcoord});

    }
    Mesh mMesh = Mesh{transformedVertices,mesh.indexArray,isFacingAway};

    for(auto &c : children) {
        if (c) {

            Mesh cMesh;
            if (freezeState == FREEZE_BRANCH)
            {
                cMesh = c->GetTransformedMesh(animationState,tfMat,cameraPos);

            } else
            {
                cMesh = c->GetTransformedMesh(t,tfMat,cameraPos);
            }


            mMesh += cMesh;
        }
    }

#ifdef DEBUG
    if (!parent) {
        for(auto &i : mMesh.vertexArray) {
            std::cout << i.position.x << " " << i.position.y << " " << i.position.z << std::endl;
        }
        std::cout << "---" << std::endl;
        for (auto &i : mMesh.indexArray) {
            std:: cout << i << std::endl;
        }
    }
#endif

    return mMesh;
}


glm::mat4 PolyhedronFace::GetFoldTransformationMatrix(const float t) const {
    auto result = glm::mat4(1.0f);

    if (t < 0.0f || t > 1.0f) {
        throw std::invalid_argument("Invalid t value for interpolation.");
    }

    if(parent) {
        const float radian = std::lerp(0.f,pivotVal,t);

        const float N = numberOfEdges;
        const float R = 1.0f / (2.0f * glm::sin(glm::pi<float>()/N));


        result = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,-R)) * result;
        result = glm::rotate(glm::mat4(1.0f),((1.0f - 2.0f / N) * half_pi<float>() ),glm::vec3(0.0f,1.0f,0.0)) * result;

        result = glm::rotate(glm::mat4(1.0f), -radian, glm::vec3(0.0f, 0.0f, 1.0f)) * result;
        result = glm::rotate(glm::mat4(1.0f),(-(1.0f - 2.0f / N) * half_pi<float>() ),glm::vec3(0.0f,1.0f,0.0)) * result;

        result = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,R)) * result;

    }

    return result;
}

Polyhedron::Polyhedron() : isDirty(true) {};

Mesh Polyhedron::GetTransformedMesh(const glm::vec3& cameraPos,const bool setToOrigin)  {
    auto res = Mesh();

    if (root) {
        res = root->GetTransformedMesh(animationState,setToOrigin ? mat4(1) : localTransform.GetTransformMatrix(),cameraPos);
    }
    isDirty = false;

    return res;
}

IndexedMeshObject Polyhedron::GetIndexedMesh(const glm::vec3& cameraPos)
{
    return IndexedMeshObject(GetTransformedMesh(cameraPos, true));
}


