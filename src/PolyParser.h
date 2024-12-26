//
// Created by ace on 2024.12.18..
//

#ifndef POLYPARSER_H
#define POLYPARSER_H

#include "Polyhedron.h"
#include "PolyUtils.h"

namespace PolyhedronFolder {
    class PolyParser {

    public:
        PolyParser();

        static void ParseFromFile(const std::string& filename,Polyhedron& polyhedron);
        static void Parse(std::string& command,Polyhedron& polyhedron);

    private:
        inline static uint lastNAdd = -1;
        inline static uint lastNPush = -1;
    };
}





#endif //POLYPARSER_H
