//
// Created by ace on 2024.12.31..
//

#ifndef OBJSAVER_H
#define OBJSAVER_H
#include <format>
#include <string>

#include "GLUtils.hpp"

#include <fstream>

class ObjSaver {
public:
    static void SaveTo(const std::string& filename,const IndexedMeshObject& mesh,const std::string& objectName = "Poly");
private:
    static void WriteObject(const std::string& name)
    {
        auto temp = "o " + name + "\n";
        outputStrm.write(temp.data(),temp.size());
    }
    static void WriteVertexPos(const glm::vec3& vertexPos)
    {
        auto temp = std::format("v {} {} {}\n",vertexPos.x,vertexPos.y,vertexPos.z);
        outputStrm.write(temp.data(),temp.size());
    }
    static void WriteNormal(const glm::vec3& normal)
    {
        auto temp = std::format("vn {} {} {}\n",normal.x,normal.y,normal.z);
        outputStrm.write(temp.data(),temp.size());
    }
    static void WriteTexCoord(const glm::vec2& texCoord)
    {
        auto temp = std::format("vt {} {}\n",texCoord.x,texCoord.y);
        outputStrm.write(temp.data(),temp.size());
    }
    static void WriteFace(const uint* indices)
    {
        auto temp = std::format("f {} {} {}\n",indices[0],indices[1],indices[2]);
        outputStrm.write(temp.data(),temp.size());
    }

    static void WriteFace(const uint* posIndices,const uint* uvIndices)
    {
        auto temp = std::format("f {}/{} {}/{} {}/{}\n",posIndices[0],uvIndices[0],posIndices[1],uvIndices[1],posIndices[2],uvIndices[2]);
        outputStrm.write(temp.data(),temp.size());
    }

    inline static std::ofstream outputStrm;

    inline static const std::string EXT = ".obj";
    inline static const std::string PATH = "../Assets/Saves/";

};



#endif //OBJSAVER_H
