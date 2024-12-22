//
// Created by ace on 2024.12.18..
//

#ifndef POLYUTILS_H
#define POLYUTILS_H
#include <unordered_map>

#include "GLUtils.hpp"

inline size_t key(int i,int j) {return (size_t) i << 32 | (unsigned int) j;}

class PolyUtils {
public:
    PolyUtils();
    static MeshObject<Vertex> ConstrPolyFace(int n);

    float GetDefaultAngle(const int& n,const int& m) const {
        return defaultAngles.at(key(n,m));
    }

    float GetDefaultAngle(const std::pair<int,int>& n) const {
        return defaultAngles.at(key(n.first,n.second));
    }

    void SetDefaultAngle(const int& n, const int& m, float angle) {
        defaultAngles[key(n,m)] = angle;
    }

    void SetDefaultAngle(const std::pair<int,int>& n, float a) {
        defaultAngles[key(n.first,n.second)] = a;
    }

private:
    std::unordered_map<size_t, float> defaultAngles;
};



#endif //POLYUTILS_H
