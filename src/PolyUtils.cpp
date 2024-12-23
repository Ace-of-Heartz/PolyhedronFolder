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

Mesh PolyUtils::ConstrPolyFace(int n) {

    float n_f = n;
    float interiorAngle = glm::pi<float>() - (1.f - 2.f / n_f) * glm::pi<float>();

    auto V = Vertex{glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0f, 1.0f, 0.0f),glm::vec2(0.5f,1.0f)}; //TODO: Normal + UV

    std::vector<Vertex> vertexArray;
    std::vector<uint> indexArray;

    vertexArray.resize(n);
    indexArray.resize((n - 2) * 3);
    vertexArray[0] = V;

    for (int i = 1; i < n; ++i) {
        float i_f = i ;

        auto pos = V.position - glm::vec3( std::sin(interiorAngle * i_f ),0,std::cos(interiorAngle * i_f));
        auto uv = glm::vec2( (std::sin(2.f * glm::pi<float>() * i_f / n_f) + 1.0f) / 2.0f ,(std::cos(2.f * glm::pi<float>() * i_f / n_f) + 1.0f) / 2.0f);

        V = Vertex{pos, glm::vec3(0.0f, 1.0f, 0.0f),uv};
        vertexArray[i] = V;
    }

    for (int i = 0; i < (n - 2); i++) {
        indexArray[i*3] = 0;
        indexArray[i*3+1] = i+1;
        indexArray[i*3+2] = i+2;
    }

    return Mesh{vertexArray, indexArray};

}

std::initializer_list<VertexAttributeDescriptor>  PolyUtils::GetVertexAttributeDescriptors() {
    return{
		{ 0, offsetof( Vertex, position ), 3, GL_FLOAT },
		{ 1, offsetof( Vertex, normal   ), 3, GL_FLOAT },
		{ 2, offsetof( Vertex, texcoord ), 2, GL_FLOAT },
	};
}

