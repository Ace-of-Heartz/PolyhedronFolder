//
// Created by ace on 2024.12.18..
//

#ifndef POLYPARSER_H
#define POLYPARSER_H

#include "Polyhedron.h"

namespace PolyhedronFolder {
    class PolyParser {

    public:
        PolyParser();

        static PolyhedronFace* parse(const std::string& filename);

    private:
    };
}





#endif //POLYPARSER_H
