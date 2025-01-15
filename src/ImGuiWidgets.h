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

    static void RenderPivotTable()
    {

        auto nValues = PolyUtils::GetNValues();
        if(ImGui::BeginTable("Pivot values",nValues.size() + 1,ImGuiTableFlags_Borders))
        {

            ImGui::TableSetupColumn("N Values");
            for (auto n : nValues)
            {
                ImGui::TableSetupColumn(std::to_string(n).data());
            }

            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            // for (auto m : nValues)
            // {
            //     ImGui::TableNextColumn();
            //     ImGui::Text("%i", m);
            // }
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            for (auto n : nValues)
            {
                ImGui::Text("%d",n);
                for (auto m : nValues)
                {
                    ImGui::TableNextColumn();
                    try
                    {
                        auto res = PolyUtils::GetDefaultAngle(n,m);
                        ImGui::Text("%f",res);
                    } catch (...)
                    {
                        ImGui::Text("NO ANGLE");
                    }
                }
                ImGui::TableNextRow();
                ImGui::TableNextColumn();


            }
            ImGui::EndTable();
        }


    }
    static void PolyFaceButton(int& id,PolyhedronFolder::PolyhedronFace*& face,PolyhedronFolder::Polyhedron& poly);
    static void ColorPickerWithButton(const std::string&  label,const std::string&  popup_id, glm::vec3& color_comp);
    //static void RenderMaterialGUI();
};



#endif //IMGUILIGHT_H
