//
// Created by ace on 2024.12.18..
//

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "GLUtils.hpp"

namespace PolyhedronFolder {

    class PolyhedronFace {
    public:
        PolyhedronFace(uint n,float angle,glm::mat4 tfMat);
        ~PolyhedronFace();


        void Add(int n);
        PolyhedronFace* Push(int n);
        void Pop();
        glm::mat4 Transform(float t);




        [[nodiscard]] unsigned int GetEdgeCount() const {return numberOfEdges;}


    private:
        typedef struct MeshObject<Vertex> Mesh;
        Mesh mesh;

        std::vector<PolyhedronFace*> children;
        PolyhedronFace* parent;
        int activeNeighbourIdx = 0;

        unsigned int numberOfEdges = 0;
        float angle = 0.0f;
        glm::mat4 transformMtx = glm::mat4(1.0f);
    };
}





#endif //POLYHEDRON_H
