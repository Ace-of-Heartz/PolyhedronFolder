//
// Created by ace on 2024.12.18..
//

#ifndef POLYUTILS_H
#define POLYUTILS_H
#include <unordered_map>

#include "GLUtils.hpp"

inline size_t key(int i,int j) {return (size_t) i << 32 | (unsigned int) j;}

namespace PolyhedronFolder {
    class PolyUtils {
    public:
        PolyUtils() = delete;
        static MeshObject<Vertex> ConstrPolyFace(int n);
        static std::initializer_list<VertexAttributeDescriptor> GetVertexAttributeDescriptors();

        static float GetDefaultAngle(const int& n,const int& m) {
            return defaultAngles.at(key(n,m));
        }

        static float GetDefaultAngle(const std::pair<int,int>& n) {
            return defaultAngles.at(key(n.first,n.second));
        }

        static void SetDefaultAngle(const int& n, const int& m, float angle) {
            defaultAngles[key(n,m)] = angle;
        }

        static void SetDefaultAngle(const std::pair<int,int>& n, float a) {
            defaultAngles[key(n.first,n.second)] = a;
        }

    private:
        static std::unordered_map<size_t, float> defaultAngles;
    };
}




#endif //POLYUTILS_H
