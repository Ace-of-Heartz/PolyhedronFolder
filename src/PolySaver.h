//
// Created by ace on 2024.12.29..
//

#ifndef POLYSAVER_H
#define POLYSAVER_H


#include <format>
#include <functional>
#include <unordered_map>

#include "Polyhedron.h"


namespace PolyhedronFolder
{
    const std::string EXT = ".poly";
    const std::string PATH = "../Assets/Saves/";

    class PolySaver {
    public:
        static void Save(const std::string& fileName,const Polyhedron& poly);
    private:
        static void SaveFace(std::ofstream* fileStrm,const PolyhedronFace* poly,bool isRoot = false);

        static std::string WriteStart(uint n) {return std::format("START {}\n", n); };
        static std::string WriteAdd(uint edge,uint n, float pivotVal) {return std::format("ADD {} {} {}\n",edge,n,pivotVal);};
        static std::string WritePush(uint edge,uint n, float pivotVal) {return std::format("PUSH {} {} {}\n",edge,n,pivotVal);};
        static std::string WritePop() {return "POP\n";};

    };
}




#endif //POLYSAVER_H
