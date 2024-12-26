//
// Created by ace on 2024.12.18..
//

#include "PolyParser.h"
#include "PolyUtils.h"
#include <fstream>
#include <iostream>

using namespace PolyhedronFolder;

using namespace std;

void PolyParser::ParseFromFile(const string &fileName, Polyhedron &polyhedron) {

    ifstream polyFileStrm(fileName,ios::binary);

    if(!polyFileStrm.is_open()) {
        throw std::runtime_error("File could not be opened");
    }

    string lineBuffer;
    lineBuffer.resize(1024);
    while(polyFileStrm.good() && !polyFileStrm.eof()) {
        polyFileStrm.getline(lineBuffer.data(), lineBuffer.size());

        Parse(lineBuffer, polyhedron);
    }
}

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

void PolyParser::Parse(string &str, Polyhedron &polyhedron) {

    string temp = str;

    vector<string> tokens;
    std::cout << str << std::endl;

    bool finished = false;
    while (!finished) {
        finished = temp.find(' ') == string::npos;
        tokens.push_back(temp.substr(0, temp.find(' ')));
        temp.erase(0, temp.find(' ') + 1);

    }

    uint n,m,o;
    if (tokens[0] == "START") {
        polyhedron.Start(stoi(tokens[1]));
    } else if (tokens[0] == "ADD") {
        if (tokens.size() <= 3)
            polyhedron.Add(stoi(tokens[1]), stoi(tokens[2]));
        else
            polyhedron.Add(stoi(tokens[1]), stoi(tokens[2]), stof(tokens[3]));
    } else if (tokens[0] == "PUSH") {
        if (tokens.size() <= 3)
            polyhedron.Push(stoi(tokens[1]), stoi(tokens[2]));
        else
            polyhedron.Push(stoi(tokens[1]), stoi(tokens[2]), stof(tokens[3]));
    } else if (tokens[0] == "POP") {
        polyhedron.Pop();
    } else if (tokens[0] == "PIVOT") {
        // PolyUtils::SetDefaultAngle(stoi(tokens[1]),stoi(tokens[2]),stof(tokens[3]));
    } else if (tokens[0] == "PIVOT_POLY") {
        n = stoi(tokens[1]);
        m = stoi(tokens[2]);
        o = stoi(tokens[3]);
        // PolyUtils::SetDefaultAngle(n,m,PolyUtils::CalcDefaultAngleBetween(n,m,o));
    } else if (tokens[0] == "PIVOTE_VERTEX") {
        n = stoi(tokens[1]);
        m = stoi(tokens[2]);
        o = stoi(tokens[3]);
        // PolyUtils::SetDefaultAngle(n,m,PolyUtils::CalcDefaultAngleBetween(n,m,o));
        // PolyUtils::SetDefaultAngle(n,o,PolyUtils::CalcDefaultAngleBetween(n,o,m));
        // PolyUtils::SetDefaultAngle(m,o,PolyUtils::CalcDefaultAngleBetween(m,o,n));
    } else if (tokens[0] == "RESET") {
        polyhedron.Reset();
    } else if (tokens[0] == "SAVE") {

    } else {
        std::cerr << "Unrecognised command: "<< tokens[0] << std::endl;
    }

}
