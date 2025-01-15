//
// Created by ace on 2024.12.18..
//

#ifndef POLYUTILS_H
#define POLYUTILS_H
#include <map>
#include <optional>
#include <unordered_map>

#include "GLUtils.hpp"

inline size_t key(uint i,uint j) {return (size_t) i << 32 | (unsigned int) j;}

namespace PolyhedronFolder {

    class PolyUtils {
    public:
        PolyUtils() = delete;
        static MeshObject<Vertex> ConstrPolyFace(uint n, float s);

        static float GetDefaultAngle(const uint& n,const uint& m) {

            auto keyVal = n <= m ? key(n,m) : key(m,n);

            if (!defaultAngles.contains(keyVal))
                throw std::logic_error("No default angle!");

            return defaultAngles[keyVal];

        }

        static float GetDefaultAngle(const std::pair<uint,uint>& n) {

            auto keyVal = n.first <= n.second ? key(n.first,n.second) : key(n.second,n.first);

            if (!defaultAngles.contains(keyVal)) {
                throw std::logic_error("No default angle!");
            }
            return defaultAngles[keyVal];
        }

        static void SetDefaultAngle(const uint& n, const uint& m, float angle) {

            auto keyVal = n <= m ? key(n,m) : key(m,n);

            if (defaultAngles.contains(keyVal))
            {
                std::cout << "Change from " << defaultAngles[keyVal] << " to " << angle  << std::endl;
                defaultAngles.erase(keyVal);
            }

            defaultAngles.insert(std::make_pair(keyVal,angle));
            nValues.insert(n); nValues.insert(m);
        }

        static void SetDefaultAngle(const std::pair<uint,uint>& n, float a) {

            auto keyVal = n.first <= n.second ? key(n.first,n.second) : key(n.second,n.first);

            if (defaultAngles.contains(keyVal))
                defaultAngles.erase(keyVal);

            defaultAngles.insert(std::make_pair(keyVal,a));
            nValues.insert(n.first); nValues.insert(n.second);
        }

        static glm::mat4 CalcTransformMtx(uint toEdge,float n,float parentN);

        static float CalcDefaultAngleBetween(float n, float m, float o);

        static std::vector<std::pair<std::pair<uint,uint>,std::optional<float>>> GetDefaultAngleList()
        {
            std::vector<std::pair<std::pair<uint,uint>,std::optional<float>>> res;
            res.reserve(nValues.size() * nValues.size());
            for(auto &n : nValues)
            {
                for (auto &m : nValues)
                {
                    try
                    {
                        res.push_back(std::make_pair(std::make_pair(n,m),GetDefaultAngle(n,m)));
                    } catch (...)
                    {
                        res.push_back(std::make_pair(std::make_pair(n,m),std::optional<float>()));
                    }
                }
            }
            return res;
        }

        static std::set<uint> GetNValues()
        {
            return nValues;
        }
    private:
        inline static std::unordered_map<size_t, float> defaultAngles;
        inline static std::set<uint> nValues;
    };


}




#endif //POLYUTILS_H
