//
// Created by ace on 2024.12.18..
//

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include <glm/gtc/constants.hpp>

#include "GLUtils.hpp"
#include "Transform.h"
#include <list>

namespace PolyhedronFolder {
    typedef MeshObject<Vertex> Mesh;

    class PolyhedronFace {
    public:

        [[nodiscard]] Mesh GetTransformedMesh(float t, const glm::mat4&, glm::vec3 cameraPos);
        [[nodiscard]] glm::mat4 GetFoldTransformationMatrix(float t) const;
        [[nodiscard]] unsigned int GetEdgeCount() const {return numberOfEdges;}
        [[nodiscard]] bool IsSingleParent() const
        {
            bool res = false;
            for (int i = 0; i < numberOfEdges  && !res; i++)
            {
                res = false || children[i] != nullptr;
            }
            return res;
        }
        [[nodiscard]] PolyhedronFace* GetNthChildren(const uint n) const { return children[n];}
        [[nodiscard]] float GetPivotVal() const { return pivotVal;}
        [[nodiscard]] void SetAnimationState(const float animationState) { this->animationState = animationState;}
        [[nodiscard]] float GetAnimationState() const { return animationState;}


        enum FreezeState
        {
            FREEZE_BRANCH,
            FREEZE_NODE,
            UNFREEZE,

        };

        [[nodiscard]] FreezeState GetFreezeState() const { return freezeState;}
        void ToggleFreeze()
        {
            switch (freezeState)
            {
                case UNFREEZE:
                    freezeState = FREEZE_NODE;
                    break;
                case FREEZE_NODE:
                    freezeState = FREEZE_BRANCH;
                    break;
                case FREEZE_BRANCH:
                    freezeState = UNFREEZE;
                    break;
            }
        }
       void SetFreezeState(const FreezeState state) { freezeState = state;}


        [[nodiscard]] std::list<PolyhedronFace*> GetChildrenList()
        {
            std::list<PolyhedronFace*> res;

            for (auto child : children)
            {
                if (child)
                {
                    res.push_back(child);
                    if (child->IsSingleParent())
                    {
                        auto child_list = child->GetChildrenList();
                        res.insert(res.end(),child_list.begin() , child_list.end());
                    }
                }
            }
            return res;
        }

        void SetActiveState(bool state) {activeState = state;}
        [[nodiscard]] bool IsActive() const { return activeState;}

    private:
        PolyhedronFace(uint n,float pivotVal = 0.0f, const glm::mat4 &tfMat = glm::mat4(1.0f),uint edge = -1,PolyhedronFace* parent = nullptr,bool isRoot = true);
        ~PolyhedronFace();
        friend class Polyhedron;

        void SetNumberOfEdges(uint newNumberOfEdges);
        void SetPivotVal(float pivotVal);
        void SetTransformMatrix(const glm::mat4& transformMatrix);

        void Add(uint edge,uint n, float pivotVal = glm::half_pi<float>());
        [[nodiscard]] PolyhedronFace* Push(uint edge,uint n, float pivotVal = glm::half_pi<float>());
        [[nodiscard]] PolyhedronFace* Pop() const {return parent;};
        void Remove(uint edge);


        Mesh mesh;

        std::vector<PolyhedronFace*> children;
        PolyhedronFace* parent;
        int parentEdgeIndex;
        bool activeState = false;

        glm::mat4 localTransformMtx = glm::mat4(1.0f);

        unsigned int numberOfEdges = 0;
        float pivotVal = 0.0f;

        float animationState = 0.0f;
        FreezeState freezeState = UNFREEZE;

    };

    class Polyhedron
    {
    public:
        Polyhedron();

        Mesh GetTransformedMesh(const glm::vec3& cameraPos, bool setToOrigin = false);
        IndexedMeshObject GetIndexedMesh(const glm::vec3& cameraPos);

        void Start(const uint n) {
            if (n <= 2)
                throw std::invalid_argument("Edge count must be greater than 2!");


            root = new PolyhedronFace(n);
            active = root;
            SetActive();
            isDirty = true;
        }

        void UnsetActive() const {active ->SetActiveState(false);}
        void SetActive() const {active->SetActiveState(true);}

        void Add(uint edge, uint n, float pivot_val = glm::half_pi<float>()) {
            if (!active)
                throw std::logic_error("No active polyhedron to add to!");

            if (active->GetEdgeCount() <= edge)
                throw std::invalid_argument("Edge out of range!");

            if (n <= 2)
                throw  std::invalid_argument("Edge count must be greater than 2!");

            active->Add(edge,n,pivot_val);
            isDirty = true;

        }

        void Push(uint edge, uint n, float pivot_val = glm::half_pi<float>()) {
            if (!active)
                throw std::logic_error("No active polyhedron to push to!");

            if (active->GetEdgeCount() <= edge)
                throw std::invalid_argument("Edge out of range!");

            if (n <= 2)
                throw std::invalid_argument("Edge count must be greater than 2!");

            UnsetActive();
            active = active->Push(edge,n,pivot_val);
            SetActive();

            isDirty = true;
        }

        void Pop()
        {
            if(!active)
                throw std::logic_error("No active polyhedron to pop out of!");

            UnsetActive();
            auto p = active->Pop();
            if (p)
            {
                active = p;
                SetActive();
            }

        }

        void Reset() {
            if (root) {
                delete root; root = nullptr;
                isDirty = true;
            }
            active = nullptr;
        }

        void Remove(const uint edge) const
        {
            if (!active)
                throw std::logic_error("No active polyhedron to remove!");

            if (active->GetEdgeCount() <= edge)
                throw std::invalid_argument("Edge out of range!\nTried removing " + std::to_string(edge+1)  + "nth edge of " + std::to_string(active->GetEdgeCount()) + "-gon");

            active->Remove(edge);
        }


        void SetPivotOfActive(float pivot_val) const
        {
            if (!active)
                throw std::logic_error("No active polyhedron to change pivot of!");

            active->SetPivotVal(pivot_val);
        }

        void SetEdgeCountOfActive(uint n) const
        {
            if (!active)
            {
                throw std::logic_error("No active polyhedron to change edge count!");
            }

            if(n <= 2)
            {
                throw std::invalid_argument("Edge count must be greater than 2!");
            }
            active->SetNumberOfEdges(n);
        }

        [[nodiscard]] std::list<PolyhedronFace*> GetFaceList() const
        {
            if (root)
            {
                std::list<PolyhedronFace*> res;
                res.push_back(root);

                auto root_list = root->GetChildrenList();


                res.insert(res.end(),root_list.begin(),root_list.end());

                return res;
            }
            else
                return {};
        }

        [[nodiscard]] Transform& GetLocalTransform() { return localTransform;}


        [[nodiscard]] bool IsDirty() const {return isDirty;}
        void SetAnimationState(const float t) {animationState = t; isDirty = true;}

        [[nodiscard]] PolyhedronFace *GetActiveFace() const
        {
            if (!active)
                throw std::logic_error("No active polyhedron to query!");

            return active;
        };
        [[nodiscard]] PolyhedronFace *GetRoot() const
        {
            if (!root)
                throw std::logic_error("No active polyhedron to query!");
            return root;
        }


        [[nodiscard]] bool IsInstantiated() const { return root != nullptr;}

        void SetActiveFace(PolyhedronFace* face)
        {
            if (face)
            {
                UnsetActive();
                active = face;
                SetActive();
            }
        }
    private:



        PolyhedronFace* root = nullptr;
        PolyhedronFace* active = nullptr;

        Transform localTransform;

        bool isDirty = true;
        float animationState = 0.0f;


    };
}





#endif //POLYHEDRON_H
