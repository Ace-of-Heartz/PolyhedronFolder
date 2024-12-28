//
// Created by ace on 2024.12.18..
//

#include "PolyParser.h"

#include <charconv>
#include <iostream>
#include <fstream>

#include "InMemoryTokenizer.h"
#include "PolyUtils.h"


using namespace std;
using namespace PolyhedronFolder;


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

    // while(tokenizer) {
    //     cout << tokenizer.NextToken() << endl;
    // }
}

void PolyParser::SetDataFromInput(const string &input) {
    tokenizer.SetData(input.data(),input.size());
}

void PolyParser::Parse(Polyhedron &polyhedron) {

    while (tokenizer) {
        string_view token = tokenizer.NextToken();
        cout << token << std::endl;

        if (token[0] == '#') {
            tokenizer.ToNextLine();
            continue;
        }



        auto command = string(token);
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
                //TODO
            } break;
            case str2int("SAVE_TO_OBJ"): {
                //TODO
            } break;
            default:
                std::cout << "Not a valid command: " << token << std::endl;
            break;

        }
    }
    polyRawData.clear();
}


