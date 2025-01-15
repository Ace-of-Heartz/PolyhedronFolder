//
// Created by ace on 2024.12.31..
//

#ifndef IMGUILIGHT_H
#define IMGUILIGHT_H

#include <string>

#include "imgui.h"
#include "Light.h"
#include "Polyhedron.h"
#include "PolyUtils.h"
using namespace PolyhedronFolder;

class ImGuiWidgets {
public:
    static void RenderLightGUI(Light&);
    static void RenderImage(uint image_id, ImVec2 size, ImVec2 uv1, ImVec2 uv2, ImVec4 tint_col = ImVec4(1,1,1,1), ImVec4 border_col =ImVec4(0,0,0,0));

    static void RenderPivotTable();
    static void RenderPolygonTable(Polyhedron& polyhedron);
    static void PolygonTableItem(const int& idx,PolyhedronFace*& face,Polyhedron& poly);
    static void ColorPickerWithButton(const std::string&  label,const std::string&  popup_id, glm::vec3& color_comp);
    //static void RenderMaterialGUI();
};



#endif //IMGUILIGHT_H
