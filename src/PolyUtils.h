//
// Created by ace on 2024.12.18..
//

#ifndef POLYUTILS_H
#define POLYUTILS_H
#include <map>
#include <unordered_map>

#include "GLUtils.hpp"

inline size_t key(int i,int j) {return (size_t) i << 32 | (unsigned int) j;}

namespace PolyhedronFolder {

    class PolyUtils {
    public:
        PolyUtils() = delete;
        static MeshObject<Vertex> ConstrPolyFace(int n, float s);

        static float GetDefaultAngle(const uint& n,const uint& m) {
            if (defaultAngles.contains(key(n,m))) {
                return defaultAngles[key(n,m)];
            }
            else if (defaultAngles.contains(key(m,n))) {
                return defaultAngles.contains(key(m,n));
            } else {
                std::cerr << "No default angle specified for " << n << " and " << m << " polygons!" << std::endl;
            }
        }

        static float GetDefaultAngle(const std::pair<uint,uint>& n) {
            if (defaultAngles.contains(key(n.first,n.second))) {
                return defaultAngles[key(n.first,n.second)];
            }
            else if (defaultAngles.contains(key(n.second,n.first))) {
                return defaultAngles.contains(key(n.second,n.first));
            } else {
                std::cerr << "No default angle specified for " << n.first << " and " << n.second << " polygons!" << std::endl;
            }
        }

        static void SetDefaultAngle(const uint& n, const uint& m, float angle) {
            defaultAngles.insert(std::make_pair(key(n,m),angle));
        }

        static void SetDefaultAngle(const std::pair<uint,uint>& n, float a) {
            defaultAngles.insert(std::make_pair(key(n.first,n.second),a));
        }

        static glm::mat4 CalcTransformMtx(uint toEdge,float n,float parentN);

        static float CalcDefaultAngleBetween(float n, float m, float o);

    private:
        inline static std::unordered_map<size_t, float> defaultAngles;
    };


}




#endif //POLYUTILS_H
