//
// Created by ace on 2024.12.18..
//

#ifndef POLYUTILS_H
#define POLYUTILS_H
#include <unordered_map>

#include "GLUtils.hpp"


class PolyUtils {
public:
    PolyUtils();
    static MeshObject<Vertex> ConstrPolyFace(int n);

    float GetDefaultAngle(const int& n,const int& m) const {
        return defaultAngles.at(std::make_pair(n,m));
    }

    float GetDefaultAngle(const std::pair<int,int>& n) const {
        return defaultAngles.at(n);
    }

    void SetDefaultAngle(const int& n, const int& m, float angle) {
        defaultAngles[std::make_pair(n,m)] = angle;
    }

    void SetDefaultAngle(const std::pair<int,int>& n, float a) {
        defaultAngles.at(n) = a;
    }

private:
    std::unordered_map<std::pair<int,int>, float> defaultAngles;
};



#endif //POLYUTILS_H
