//
// Created by ace on 2024.12.29..
//

#include "PolySaver.h"

#include <fstream>

using namespace PolyhedronFolder;
using namespace std;

void PolySaver::Save(const string& fileName,const Polyhedron& poly)
{
    ofstream fileStrm( PATH + fileName + EXT,ios::binary);

    SaveFace(&fileStrm,poly.GetRoot(),true);
}

void PolySaver::SaveFace(ofstream* fileStrm,const PolyhedronFace* polyF,const bool isRoot)
{
    if (isRoot)
    {
        auto command = WriteStart(polyF->GetEdgeCount());
        fileStrm->write(command.data(),command.size());
    }

    for (auto i = 0; i < polyF->GetEdgeCount(); i++)
    {
        auto child = polyF->GetNthChildren(i);
        if (child)
        {
            if(child->IsSingleParent())
            {
                auto command = WritePush(i,child->GetEdgeCount(),child->GetPivotVal());
                fileStrm->write(command.data(),command.size());
                SaveFace(fileStrm,child);
                command = WritePop();
                fileStrm->write(command.data(),command.size());
            }
            else
            {
                auto command = WriteAdd(i,child->GetEdgeCount(),child->GetPivotVal());
                fileStrm->write(command.data(),command.size());
            }
        }
    }

}
