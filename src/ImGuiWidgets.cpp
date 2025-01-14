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

void ImGuiWidgets::PolyFaceButton(int& id,PolyhedronFolder::PolyhedronFace*& face, PolyhedronFolder::Polyhedron& poly)
{

    ImGui::Bullet(); ImGui::SameLine();
    auto name = std::format("{}. {}-NGON",id,face->GetEdgeCount());

    if (ImGui::Button(name.data()))
    {
        poly.SetActiveFace(face);
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