//
// Created by ace on 2024.12.31..
//

#include "ImGuiWidgets.h"

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
    ImGui::Bullet();
    if(ImGui::Button("Ambient"))
        ImGui::OpenPopup("Ambient");
    if(ImGui::BeginPopup("Ambient"))
    {
        auto ambient = light.GetAmbientComp();
        if(ImGui::ColorPicker3("Ambient", glm::value_ptr( ambient ) ))
        {
            light.SetAmbientComp( ambient );
        }
        ImGui::EndPopup();
    }
    ImGui::Bullet();
    if(ImGui::Button("Diffuse"))
        ImGui::OpenPopup("Diffuse");
    if(ImGui::BeginPopup("Diffuse"))
    {
        auto diffuse = light.GetDiffuseComp();
        if(ImGui::ColorPicker3("Diffuse", glm::value_ptr( diffuse ) ))
        {
            light.SetDiffuseComp( diffuse );
        }
        ImGui::EndPopup();
    }
    ImGui::Bullet();
    if(ImGui::Button("Specular"))
        ImGui::OpenPopup("Specular");
    if(ImGui::BeginPopup("Specular"))
    {
        auto specular = light.GetSpecularComp();
        if(ImGui::ColorPicker3("Specular", glm::value_ptr( specular ) ))
        {
            light.SetSpecularComp( specular );
        }
        ImGui::EndPopup();
    }


}
