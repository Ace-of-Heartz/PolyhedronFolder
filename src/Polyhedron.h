//
// Created by ace on 2024.12.18..
//

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include <optional>
#include <glm/gtc/constants.hpp>

#include "GLUtils.hpp"

namespace PolyhedronFolder {
    typedef struct MeshObject<Vertex> Mesh;

    class PolyhedronFace {
    public:
        PolyhedronFace(uint n,float pivotVal = 0.0f, const glm::mat4 &tfMat = glm::mat4(1.0f),PolyhedronFace* parent = nullptr,bool isRoot = true);
        ~PolyhedronFace();

        void SetNumberOfEdges(uint numberOfEdges);
        void SetPivotVal(float pivotVal);
        void SetTransformMatrix(glm::mat4 transformMatrix);

        [[nodiscard]] Mesh GetTransformedMesh(float t, const glm::mat4&, const glm::vec3 cameraPos);
        [[nodiscard]] glm::mat4 GetFoldTransformationMatrix(float t) const;
        [[nodiscard]] unsigned int GetEdgeCount() const {return numberOfEdges;}

    private:
        friend class Polyhedron;

        void Add(uint edge,uint n, float pivotVal = glm::half_pi<float>());
        PolyhedronFace* Push(uint edge,uint n, float pivotVal = glm::half_pi<float>());
        PolyhedronFace* Pop() {return parent;};

        Mesh mesh;

        std::vector<PolyhedronFace*> children;
        PolyhedronFace* parent;
        int activeNeighbourIdx = 0;

        unsigned int numberOfEdges = 0;
        float pivotVal = 0.0f;
        glm::mat4 localTransformMtx = glm::mat4(1.0f);
    };

    class Polyhedron {
    public:
        Polyhedron();

        Mesh GetTransformedMesh(const glm::mat4& baseTransform,const glm::vec3& cameraPos);

        void Start(uint n) {
            root = new PolyhedronFace(n);
            active = root;
            isDirty = true;
        }

        void Add(uint edge,uint n, float pivot_val = glm::half_pi<float>()) {
            std::cout << pivot_val << std::endl;
            active->Add(edge,n,pivot_val);
            isDirty = true;
        }

        void Push(uint edge,uint n, float pivot_val = glm::half_pi<float>()) {
            active = active->Push(edge,n,pivot_val);
            isDirty = true;
        }

        void Pop() {active = active->Pop();}

        void Reset() {
            if (root) {
                delete root; root = nullptr;
                isDirty = true;
            }
            active = nullptr;

        };

        [[nodiscard]] bool IsDirty() const {return isDirty;}
        void SetFoldVal(float t) {foldVal = t; isDirty = true;}

        [[nodiscard]] PolyhedronFace *GetActiveFace() const {return active;};
        [[nodiscard]] PolyhedronFace *GetRoot() const {return root;}
    private:
        PolyhedronFace* root = nullptr;
        PolyhedronFace* active = nullptr;

        bool isDirty = true;
        float foldVal = 0.0f;
    };
}





#endif //POLYHEDRON_H
