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

            switch(str2int(command)) {
            case str2int("START"): {
                uint n;

                string_view nT = tokenizer.NextToken();

                if (nT.empty()) throw invalid_argument("Invalid input: Expected polygon edge count");
                from_chars(nT.data(), nT.data() + nT.size(), n);

                polyhedron.Start(n);
                lastNAdd = n;
            }break;
            case str2int("ADD"): {
                uint edge, n;
                float pivot;

                string_view edgeT = tokenizer.NextToken(true);
                std::string_view nT = tokenizer.NextToken(true);
                std::string_view pivotT = tokenizer.NextToken(true);

                if (edgeT.empty()) throw invalid_argument("Invalid input: Expected edge ID");
                from_chars(edgeT.data(), edgeT.data() + edgeT.size(), edge);

                if(nT.empty()) n = lastNAdd;
                else from_chars(nT.data(), nT.data() + nT.size(), n);

                if(pivotT.empty()) pivot = PolyUtils::GetDefaultAngle(polyhedron.GetActiveFace()->GetEdgeCount(),n);
                else from_chars(pivotT.data(), pivotT.data() + pivotT.size(), pivot);

                polyhedron.Add(edge,n,pivot);
                lastNAdd = n;

            }break;
            case str2int("PUSH"): {
                uint edge, n;
                float pivot;

                string_view edgeT = tokenizer.NextToken(true);
                std::string_view nT = tokenizer.NextToken(true);
                std::string_view pivotT = tokenizer.NextToken(true);

                if (edgeT.empty()) throw invalid_argument("Invalid input: Expected edge ID");
                from_chars(edgeT.data(), edgeT.data() + edgeT.size(), edge);


                if(nT.empty()) n = lastNAdd;
                else from_chars(nT.data(), nT.data() + nT.size(), n);

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
                string_view mT = tokenizer.NextToken(true);
                string_view pivotT = tokenizer.NextToken(true);

                if (nT.empty() || mT.empty() || pivotT.empty())
                {
                    throw invalid_argument("Invalid input: Expected two integers and a real number");
                }

                from_chars(nT.data(), nT.data() + nT.size(), n);
                from_chars(mT.data(), mT.data() + mT.size(), m);
                from_chars(pivotT.data(), pivotT.data() + pivotT.size(), pivot);
                PolyUtils::SetDefaultAngle(n,m,pivot);

            } break;
            case str2int("PIVOT_POLY"): {
                uint n,m,o;
                string_view nT = tokenizer.NextToken(true);
                string_view mT = tokenizer.NextToken(true);
                string_view oT = tokenizer.NextToken(true);

                if (nT.empty() || mT.empty() || oT.empty())
                {
                    throw invalid_argument("Invalid input: Expected three integers");
                }

                from_chars(nT.data(), nT.data() + nT.size(), n);
                from_chars(mT.data(), mT.data() + mT.size(), m);
                from_chars(oT.data(), oT.data() + oT.size(), o);

                PolyUtils::SetDefaultAngle(n,m,PolyUtils::CalcDefaultAngleBetween(n,m,o));

            } break;
            case str2int("PIVOT_VERTEX"): {
                uint n,m,o;
                string_view nT = tokenizer.NextToken(true);
                string_view mT = tokenizer.NextToken(true);
                string_view oT = tokenizer.NextToken(true);

                if (nT.empty() || mT.empty() || oT.empty())
                {
                    throw invalid_argument("Invalid input: Expected three integers");
                }

                from_chars(nT.data(), nT.data() + nT.size(), n);
                from_chars(mT.data(), mT.data() + mT.size(), m);
                from_chars(oT.data(), oT.data() + oT.size(), o);

                PolyUtils::SetDefaultAngle(n,m,PolyUtils::CalcDefaultAngleBetween(n,m,o));
                PolyUtils::SetDefaultAngle(o,n,PolyUtils::CalcDefaultAngleBetween(o,n,m));
                PolyUtils::SetDefaultAngle(m,o,PolyUtils::CalcDefaultAngleBetween(m,o,n));
            } break;
            case str2int("CLEAR"): {
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

                if (nT.empty()) throw invalid_argument("Invalid input: Expected polygon edge count");

                from_chars(nT.data(), nT.data() + nT.size(), n);
                polyhedron.SetEdgeCountOfActive(n);

            } break;
            case str2int("SET_PIVOT"):
                {
                    float p;
                    string_view pT = tokenizer.NextToken(true);

                    if (pT.empty()) throw invalid_argument("Invalid input: Expected pivot value");
                    from_chars(pT.data(), pT.data() + pT.size(), p);
                    polyhedron.SetPivotOfActive(p);
                } break;
            case str2int("REMOVE"): {
                uint edge;
                string_view edgeT = tokenizer.NextToken(true);
                if (edgeT.empty()) throw invalid_argument("Invalid input: Expected edge ID");
                from_chars(edgeT.data(), edgeT.data() + edgeT.size(), edge);
                polyhedron.Remove(edge);
            } break;
            default:
                // std::cout << "Not a valid command: " << token << std::endl;
            break;

        }

    }
    polyRawData.clear();
}


