//
// Created by ace on 2024.12.18..
//

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "GLUtils.hpp"

namespace PolyhedronFolder {
    struct AbstractPolyhedronFace {
    public:
        AbstractPolyhedronFace(float angle,glm::mat4 tfMat);
        virtual ~AbstractPolyhedronFace() = 0;

        virtual glm::mat4 Transform(float t) = 0;

    protected:
        AbstractPolyhedronFace* parent = nullptr;

        float angle = 0.0f;
        glm::mat4 transformMtx = glm::mat4(1.0f);

    } typedef AbstractPolyhedronFace;

    template<int N>
    class PolyhedronFace : public AbstractPolyhedronFace {
    public:
        PolyhedronFace();
        ~PolyhedronFace() override {
            delete[] children;
        };


        void Add(int n);
        AbstractPolyhedronFace* Push(int n);
        void Pop();
        glm::mat4 Transform(float t) override;

        int static GetEdgeCount() {return N;}


    private:
        typedef struct MeshObject<Vertex> Mesh;
        Mesh mesh;

        AbstractPolyhedronFace* children[N];
        int activeNeighbourIdx = 0;
    };
}





#endif //POLYHEDRON_H
