//
// Created by ace on 2024.12.18..
//

#include "PolyParser.h"

#include <charconv>
#include <iostream>
#include <fstream>

#include "InMemoryTokenizer.h"
#include "ObjSaver.h"
#include "PolyUtils.h"
#include "PolySaver.h"


using namespace PolyhedronFolder;
using namespace std;


constexpr unsigned int str2int(const string& str, const int h = 0)
{
    return !(str.size() >= h) ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

void PolyParser::SetDataFromFile(const string &fileName) {
    error_code ec;
    size_t fileSize = filesystem::file_size(fileName, ec);

    if (ec) throw (EXC_FILENOTFOUND);

    polyRawData.reserve(fileSize);

    ifstream polyFileStrm(fileName,ios::binary);

    if(!polyFileStrm.is_open()) {
        throw EXC_FILENOTFOUND;
    }

    polyFileStrm.read(polyRawData.data(), fileSize);
    tokenizer.SetData(polyRawData.data(),fileSize);
}

void PolyParser::SetDataFromInput(const string &input) {
    tokenizer.SetData(input.data(),input.size());
}

void PolyParser::Parse(Polyhedron &polyhedron) {

    while (tokenizer) {
        string_view token = tokenizer.NextToken();

        if (token[0] == '#') {
            tokenizer.ToNextLine();
            continue;
        }

        auto command = string(token);
        try
        {
            switch(str2int(command)) {
            case str2int("START"): {
                uint n;

                string_view nT = tokenizer.NextToken();
                from_chars(nT.data(), nT.data() + nT.size(), n);

                polyhedron.Start(n);
                lastNAdd = n;
            }break;
            case str2int("ADD"): {
                uint edge, n;
                float pivot;

                string_view edgeT = tokenizer.NextToken(true);
                from_chars(edgeT.data(), edgeT.data() + edgeT.size(), edge);

                std::string_view nT = tokenizer.NextToken(true);
                if(nT.empty()) n = lastNAdd;
                else from_chars(nT.data(), nT.data() + nT.size(), n);

                std::string_view pivotT = tokenizer.NextToken(true);
                if(pivotT.empty()) pivot = PolyUtils::GetDefaultAngle(polyhedron.GetActiveFace()->GetEdgeCount(),n);
                else from_chars(pivotT.data(), pivotT.data() + pivotT.size(), pivot);

                polyhedron.Add(edge,n,pivot);
                lastNAdd = n;

            }break;
            case str2int("PUSH"): {
                uint edge, n;
                float pivot;

                string_view edgeT = tokenizer.NextToken(true);
                from_chars(edgeT.data(), edgeT.data() + edgeT.size(), edge);

                std::string_view nT = tokenizer.NextToken(true);
                if(nT.empty()) n = lastNAdd;
                else from_chars(nT.data(), nT.data() + nT.size(), n);

                std::cout << n << std::endl;
                std::string_view pivotT = tokenizer.NextToken(true);
                if(pivotT.empty()) pivot = PolyUtils::GetDefaultAngle(polyhedron.GetActiveFace()->GetEdgeCount(),n);
                else from_chars(pivotT.data(), pivotT.data() + pivotT.size(), pivot);

                polyhedron.Push(edge,n,pivot);
                lastNAdd = n;

            }break;
            case str2int("POP"): {
                polyhedron.Pop();
            }break;
            case str2int("PIVOT"): {
                uint n, m;
                float pivot;
                string_view nT = tokenizer.NextToken(true);
                from_chars(nT.data(), nT.data() + nT.size(), n);

                string_view mT = tokenizer.NextToken(true);
                from_chars(mT.data(), mT.data() + mT.size(), m);

                string_view pivotT = tokenizer.NextToken(true);
                from_chars(pivotT.data(), pivotT.data() + pivotT.size(), pivot);

                PolyUtils::SetDefaultAngle(n,m,pivot);

            } break;
            case str2int("PIVOT_POLY"): {
                uint n,m,o;
                string_view nT = tokenizer.NextToken(true);
                from_chars(nT.data(), nT.data() + nT.size(), n);

                string_view mT = tokenizer.NextToken(true);
                from_chars(mT.data(), mT.data() + mT.size(), m);

                string_view oT = tokenizer.NextToken(true);
                from_chars(oT.data(), oT.data() + oT.size(), o);

                PolyUtils::SetDefaultAngle(n,m,PolyUtils::CalcDefaultAngleBetween(n,m,o));

            } break;
            case str2int("PIVOT_VERTEX"): {
                uint n,m,o;
                string_view nT = tokenizer.NextToken(true);
                from_chars(nT.data(), nT.data() + nT.size(), n);

                string_view mT = tokenizer.NextToken(true);
                from_chars(mT.data(), mT.data() + mT.size(), m);

                string_view oT = tokenizer.NextToken(true);
                from_chars(oT.data(), oT.data() + oT.size(), o);

                PolyUtils::SetDefaultAngle(n,m,PolyUtils::CalcDefaultAngleBetween(n,m,o));
                PolyUtils::SetDefaultAngle(o,n,PolyUtils::CalcDefaultAngleBetween(o,n,m));
                PolyUtils::SetDefaultAngle(m,o,PolyUtils::CalcDefaultAngleBetween(m,o,n));
            } break;
            case str2int("RESET"): {
                polyhedron.Reset();
            } break;
            case str2int("SAVE_TO_POLY"): {
                const auto fileName = string(tokenizer.NextToken(true));
                PolySaver::SaveTo(fileName,polyhedron);
            } break;
            case str2int("SAVE_TO_OBJ"): {
                const auto fileName = string(tokenizer.NextToken(true));
                const auto objectName = string(tokenizer.NextToken(true));
                const auto mesh = polyhedron.GetIndexedMesh(glm::vec3(0.0));

                if (objectName.empty())
                    ObjSaver::SaveTo(fileName,mesh);
                else
                    ObjSaver::SaveTo(fileName,mesh,objectName);
            } break;
            case str2int("SET_N"):
            {
                uint n;
                string_view nT = tokenizer.NextToken(true);
                from_chars(nT.data(), nT.data() + nT.size(), n);
                polyhedron.GetActiveFace()->SetNumberOfEdges(n);
            } break;
            case str2int("REMOVE"): {
                uint edge;
                string_view edgeT = tokenizer.NextToken(true);
                from_chars(edgeT.data(), edgeT.data() + edgeT.size(), edge);
                polyhedron.Remove(edge);
            } break;
            default:
                std::cout << "Not a valid command: " << token << std::endl;
            break;

        }
        } catch(exception &e)
        {
            std::cerr << e.what()  << std::endl;
        }

    }
    polyRawData.clear();
}


