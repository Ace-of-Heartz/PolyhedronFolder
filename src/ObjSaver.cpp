//
// Created by ace on 2024.12.31..
//

#include "ObjSaver.h"
#include <fstream>

void ObjSaver::SaveTo(const std::string& filename,const IndexedMeshObject& mesh,const std::string& objectName )
{
    outputStrm = std::ofstream( PATH + filename + EXT,std::ios::binary);

    if(!outputStrm.is_open())
    {
        throw std::runtime_error("Error: File couldn't be opened.");
    }

    WriteObject(objectName);
    for (auto &pos : mesh.positions)
    {
        WriteVertexPos(pos);
    }

    for (auto &uv : mesh.texCoords)
    {
        WriteTexCoord(uv);
    }


    for(int i = 0; i < mesh.positionIndices.size(); i += 3)
    {
        WriteFace(&mesh.positionIndices[i],&mesh.texCoordIndices[i]);
    }

    outputStrm.close();
}
