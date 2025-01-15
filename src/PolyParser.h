//
// Created by ace on 2024.12.18..
//

#ifndef POLYPARSER_H
#define POLYPARSER_H

#include "InMemoryTokenizer.h"
#include "Polyhedron.h"
#include "PolyUtils.h"

namespace PolyhedronFolder {


    class PolyParser {

    public:
        PolyParser();

        static void ParseFromFile(const std::string& filename,Polyhedron& polyhedron);
        static void Parse(Polyhedron& polyhedron);
        static void SetDataFromFile(const std::string& filename);
        static void SetDataFromInput(const std::string& data);

        enum Exception { EXC_FILENOTFOUND };

    private:
        inline static uint lastNAdd = 3;
        inline static uint lastNPush = 3;

        inline static std::vector<char> polyRawData;
        inline static InMemoryTokenizer tokenizer;
    };
}





#endif //POLYPARSER_H
