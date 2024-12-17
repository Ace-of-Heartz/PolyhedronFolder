//
// Created by ace on 2024.12.18..
//

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "GLUtils.hpp"


struct AbstractPolyhedronFace {
    virtual ~AbstractPolyhedronFace() = 0;
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

    glm::mat4 Rotate(float angle);

private:
    int activeNeighbourIdx = 0;

    typedef struct MeshObject<Vertex> Mesh;
    Mesh mesh;

    template<int M>
    AbstractPolyhedronFace* children[N];
    template<int M>
    AbstractPolyhedronFace* parent;
};



#endif //POLYHEDRON_H
