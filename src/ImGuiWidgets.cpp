//
// Created by ace on 2024.12.31..
//

#include "ImGuiWidgets.h"

#include <format>


#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

void ImGuiWidgets::RenderLightGUI(Light& light)
{

    static auto lightPosXZ = glm::vec2( 0.0f );
    lightPosXZ = glm::vec2( light.GetPosDir().x, light.GetPosDir().z );


    if ( ImGui::SliderFloat2( "Light Position XZ", glm::value_ptr( lightPosXZ ), -1.0f, 1.0f ) )
    {
        float lightPosL2 = lightPosXZ.x * lightPosXZ.x + lightPosXZ.y * lightPosXZ.y;
        if ( lightPosL2 > 1.0f ) // Ha kívülre esne a körön, akkor normalizáljuk
        {
            lightPosXZ /= sqrtf( lightPosL2 );
            lightPosL2 = 1.0f;
        }
        auto newLightPos = glm::vec4( lightPosXZ.x,sqrtf( 1.0f - lightPosL2 ), lightPosXZ.y,0.f );
        light.SetPosDir(newLightPos);
    }
    ImGui::LabelText( "Light Position Y", "%f", light.GetPosDir().y );

    ImGui::Bullet(); ColorPickerWithButton("Ambient","Ambient",light.GetAmbientComp());
    ImGui::Bullet(); ColorPickerWithButton("Diffuse","Diffuse",light.GetDiffuseComp());
    ImGui::Bullet(); ColorPickerWithButton("Specular","Specular",light.GetSpecularComp());

}

void ImGuiWidgets::RenderImage(uint image_id, ImVec2 size, ImVec2 uv1, ImVec2 uv2, ImVec4 tint_col , ImVec4 border_col)
{
    ImGui::Image(image_id,size,uv1,uv2,tint_col,border_col);
}

void ImGuiWidgets::PolygonTableItem(const int& idx,PolyhedronFolder::PolyhedronFace*& face, PolyhedronFolder::Polyhedron& poly)
{

    // ImGui::Bullet(); ImGui::SameLine();
    auto name = std::format("{}-NGON",face->GetEdgeCount());

    ImGui::Text("%d",idx);
    ImGui::TableNextColumn();

    ImGui::Text("%s",name.data());
    ImGui::TableNextColumn();

    ImGui::PushID(idx);
    int selected;

    if (face->IsActive())
    {
        selected = idx;
    }
    else
    {
        selected = -1;
    }

    if (ImGui::RadioButton(std::to_string(idx).data(),selected == idx))
    {
        poly.SetActiveFace(face);
    }


    ImGui::TableNextColumn();


    ImGui::PushID(idx);
    if(ImGui::ColorButton("",
        face->GetFreezeState() == PolyhedronFace::UNFREEZE ? ImVec4(1.0,0.6,0.2,1.0)
        : face->GetFreezeState() == PolyhedronFace::FREEZE_NODE ? ImVec4(0.3,0.44,1.0,1.0) : (ImVec4(0.0,0.0,1.0,1.0)) ))
    {
        face->ToggleFreeze();
    }

    ImGui::TableNextColumn();

    ImGui::BeginDisabled(face->GetFreezeState() == PolyhedronFace::UNFREEZE);
    auto anim = face->GetAnimationState();

    ImGui::PushID(idx);
    if(ImGui::SliderFloat("",&anim,0.0f,1.0f))
    {
        face->SetAnimationState(anim);
    }
    ImGui::PopID();
    ImGui::PopID();
    ImGui::PopID();

    ImGui::EndDisabled();
}

void ImGuiWidgets::RenderPolygonTable(Polyhedron& polyhedron)
{
    int idx = 1;

    auto list = polyhedron.GetFaceList();

    auto headers = {"ID","Type","Selected","Freeze","Animation State"};
    ImGui::BeginTable("Polygons",headers.size());
    for (auto &h : headers)
    {
        ImGui::TableSetupColumn(h);
    }
    ImGui::TableHeadersRow();

    for (auto &f : list)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        PolygonTableItem(idx,f,polyhedron);
        ++idx;
    }
    ImGui::EndTable();
}



void ImGuiWidgets::RenderPivotTable()
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
                    ImGui::Text("N/A"); //No Angle
                }
            }
            ImGui::TableNextRow();
            ImGui::TableNextColumn();


        }
        ImGui::EndTable();
    }


}

void ImGuiWidgets::ColorPickerWithButton(const std::string& label,const std::string& popup_id, glm::vec3& color_comp)
{
    if(ImGui::Button(label.data()))
        ImGui::OpenPopup(popup_id.data());
    if(ImGui::BeginPopup(popup_id.data()))
    {
        if(ImGui::ColorPicker3(label.data(), glm::value_ptr( color_comp ) ))
        {
        }
        ImGui::EndPopup();
    }
}