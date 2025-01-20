#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ObjParser.h"
#include "ParametricSurfaceMesh.hpp"
#include "ProgramBuilder.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <iostream>
#include <string>
#include <array>
#include <algorithm>

#include "PolyParser.h"
#include "PolyUtils.h"

#include "ImGuiWidgets.h"
#include "ObjSaver.h"
#include "PolySaver.h"


using namespace PolyhedronFolder;

CMyApp::CMyApp()
{
}

CMyApp::~CMyApp()
{
}

void CMyApp::SetupDebugCallback()
{
    // engedélyezzük és állítsuk be a debug callback függvényt ha debug context-ben vagyunk
    GLint context_flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
    if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);
        glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
    }
}

void CMyApp::InitShaders()
{
    m_programID = glCreateProgram();
    ProgramBuilder{m_programID}
        .ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_PosNormTex.vert")
        .ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_Lighting.frag")
        .Link();
    m_programAxis = glCreateProgram();
    ProgramBuilder{m_programAxis}
        .ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_axes.vert")
        .ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_PosCol.frag")
        .Link();
}


void CMyApp::CleanShaders()
{
    glDeleteProgram(m_programID);
    glDeleteProgram(m_programAxis);
}


void CMyApp::InitGeometry()
{
    const std::initializer_list<VertexAttributeDescriptor> vertexAttribList = {
        {0, offsetof(Vertex, position), 3, GL_FLOAT},
        {1, offsetof(Vertex, normal), 3, GL_FLOAT},
        {2, offsetof(Vertex, texcoord), 2, GL_FLOAT},
    };
    auto polyMeshCPU = m_Polyhedron.GetTransformedMesh(m_camera.GetEye());
    m_PolyhedronPoly = CreateGLObjectFromMesh(polyMeshCPU, vertexAttribList);
}

void CMyApp::CleanGeometry()
{
    CleanOGLObject(m_PolyhedronPoly);
}

void CMyApp::InitTextures()
{
    // sampler

    glCreateSamplers(1, &m_SamplerID);
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ImageRGBA PolyhedronImage = ImageFromFile("Assets/wood.jpg");

    glCreateTextures(GL_TEXTURE_2D, 1, &m_PolyhedronTextureID);
    glTextureStorage2D(m_PolyhedronTextureID, NumberOfMIPLevels(PolyhedronImage), GL_RGBA8, PolyhedronImage.width,
                       PolyhedronImage.height);
    glTextureSubImage2D(m_PolyhedronTextureID, 0, 0, 0, PolyhedronImage.width, PolyhedronImage.height, GL_RGBA,
                        GL_UNSIGNED_BYTE, PolyhedronImage.data());

    glGenerateTextureMipmap(m_PolyhedronTextureID);
}

void CMyApp::LoadTexture(const std::string& filename)
{
    ImageRGBA PolyhedronImage = ImageFromFile(filename);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_PolyhedronTextureID);
    glTextureStorage2D(m_PolyhedronTextureID, NumberOfMIPLevels(PolyhedronImage), GL_RGBA8, PolyhedronImage.width,
                       PolyhedronImage.height);
    glTextureSubImage2D(m_PolyhedronTextureID, 0, 0, 0, PolyhedronImage.width, PolyhedronImage.height, GL_RGBA,
                        GL_UNSIGNED_BYTE, PolyhedronImage.data());

    glGenerateTextureMipmap(m_PolyhedronTextureID);
}


void CMyApp::CleanTextures()
{
    glDeleteTextures(1, &m_PolyhedronTextureID);
}


bool CMyApp::Init()
{
    SetupDebugCallback();

    // törlési szín legyen kékes
    glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

    InitShaders();
    InitGeometry();
    InitTextures();

    glPointSize(16.0f);
    glLineWidth(4.0f);

    glEnable(GL_CULL_FACE); // kapcsoljuk be a hátrafelé néző lapok eldobását
    glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok

    glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

    // kamera
    m_camera.SetView(
        glm::vec3(0.0, 7.0, 7.0), // honnan nézzük a színteret	   - eye
        glm::vec3(0.0, 0.0, 0.0), // a színtér melyik pontját nézzük - at
        glm::vec3(0.0, 1.0, 0.0)); // felfelé mutató irány a világban - up

    m_cameraManipulator.SetCamera(&m_camera);

    return true;
}

void CMyApp::Clean()
{
    CleanShaders();
    CleanGeometry();
    CleanTextures();
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
    if (m_animate)
    {
        m_currentFoldValue += updateInfo.DeltaTimeInSec * m_animationSpeed;
        m_animationState += updateInfo.DeltaTimeInSec * m_animationSpeed;
        if (m_animationState >= 2.0f)
        {
            m_animationState = m_animationState - 2.0f;
        }
        else if (m_animationState >= 1.0f)
        {
            m_currentFoldValue = 2.0f - m_animationState;
            m_Polyhedron.SetAnimationState(m_currentFoldValue);
        }
        else
        {
            m_currentFoldValue = m_animationState;
            m_Polyhedron.SetAnimationState(m_currentFoldValue);
        }
    }

    InitGeometry();
    m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

    m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);
}

void CMyApp::SetLightingUniforms(GLuint program, float Shininess, glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks)
{
    // - Fényforrások beállítása
    glProgramUniform3fv(program, ul(program, "cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));

    glProgramUniform4fv(program, ul(program, "lightPosDir1"), 1, glm::value_ptr(m_light1.GetPosDir()));
    glProgramUniform3fv(program, ul(program, "La1"), 1, glm::value_ptr(m_light1.GetAmbientComp()));
    glProgramUniform3fv(program, ul(program, "Ld1"), 1, glm::value_ptr(m_light1.GetDiffuseComp()));
    glProgramUniform3fv(program, ul(program, "Ls1"), 1, glm::value_ptr(m_light1.GetSpecularComp()));

    glProgramUniform4fv(program, ul(program, "lightPosDir2"), 1, glm::value_ptr(m_light2.GetPosDir()));
    glProgramUniform3fv(program, ul(program, "La2"), 1, glm::value_ptr(m_light2.GetAmbientComp()));
    glProgramUniform3fv(program, ul(program, "Ld2"), 1, glm::value_ptr(m_light2.GetDiffuseComp()));
    glProgramUniform3fv(program, ul(program, "Ls2"), 1, glm::value_ptr(m_light2.GetSpecularComp()));

    glProgramUniform1f(program, ul(program, "lightConstantAttenuation"), m_lightConstantAttenuation);
    glProgramUniform1f(program, ul(program, "lightLinearAttenuation"), m_lightLinearAttenuation);
    glProgramUniform1f(program, ul(program, "lightQuadraticAttenuation"), m_lightQuadraticAttenuation);

    // - Anyagjellemzők beállítása
    glProgramUniform3fv(program, ul(program, "Ka"), 1, glm::value_ptr(Ka));
    glProgramUniform3fv(program, ul(program, "Kd"), 1, glm::value_ptr(Kd));
    glProgramUniform3fv(program, ul(program, "Ks"), 1, glm::value_ptr(Ks));

    glProgramUniform1f(program, ul(program, "Shininess"), Shininess);
}

void CMyApp::RenderPolyhedron()
{
    glProgramUniformMatrix4fv(m_programID, ul(m_programID, "viewProj"), 1, GL_FALSE,
                              glm::value_ptr(m_camera.GetViewProj()));

    auto matWorld = glm::mat4(1);

    glProgramUniformMatrix4fv(m_programID, ul(m_programID, "world"), 1, GL_FALSE, glm::value_ptr(matWorld));
    glProgramUniformMatrix4fv(m_programID, ul(m_programID, "worldIT"), 1, GL_FALSE,
                              glm::value_ptr(glm::transpose(glm::inverse(matWorld))));

    SetLightingUniforms(m_programID, m_Shininess, m_Ka, m_Kd, m_Ks);

    glProgramUniform1i(m_programID, ul(m_programID, "texImage"), 0);

    glBindTextureUnit(0, m_PolyhedronTextureID);
    glBindSampler(0, m_SamplerID);

    glBindVertexArray(m_PolyhedronPoly.vaoID);

    glUseProgram(m_programID);

    glDrawElements(GL_TRIANGLES,
                   m_PolyhedronPoly.count,
                   GL_UNSIGNED_INT,
                   nullptr);
}

void CMyApp::RenderObject()
{
    glDisable(GL_CULL_FACE);
    glProgramUniformMatrix4fv(m_programID, ul(m_programID, "viewProj"), 1, GL_FALSE,
                              glm::value_ptr(m_camera.GetViewProj()));

    auto matWorld = m_PolyhedronObject.GetLocalTransform().GetTransformMatrix();

    glProgramUniformMatrix4fv(m_programID, ul(m_programID, "world"), 1, GL_FALSE, glm::value_ptr(matWorld));
    glProgramUniformMatrix4fv(m_programID, ul(m_programID, "worldIT"), 1, GL_FALSE,
                              glm::value_ptr(glm::transpose(glm::inverse(matWorld))));

    SetLightingUniforms(m_programID, m_Shininess, m_Ka, m_Kd, m_Ks);

    glProgramUniform1i(m_programID, ul(m_programID, "texImage"), 0);

    glBindTexture(0, m_PolyhedronTextureID);
    glBindSampler(0, m_SamplerID);

    glBindVertexArray(m_PolyhedronObject.GetObject().vaoID);

    glUseProgram(m_programID);

    glDrawElements(GL_TRIANGLES, m_PolyhedronObject.GetObject().count,GL_UNSIGNED_INT, nullptr);
    glEnable(GL_CULL_FACE);
}

void CMyApp::RenderAxis()
{
    glm::mat4 matWorld = glm::translate(glm::vec3(0, 0, 0));

    glDisable(GL_DEPTH_TEST);

    glProgramUniform1f(m_programAxis, ul(m_programAxis, "mult"), 0.5f);

    glProgramUniformMatrix4fv(m_programAxis, ul(m_programAxis, "viewProj"), 1, GL_FALSE,
                              glm::value_ptr(m_camera.GetViewProj()));
    glProgramUniformMatrix4fv(m_programAxis, ul(m_programAxis, "world"), 1, GL_FALSE, glm::value_ptr(matWorld));

    glUseProgram(m_programAxis);

    glDrawArrays(GL_LINES, 0, 6);

    glEnable(GL_DEPTH_TEST);
}


void CMyApp::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderPolyhedron();
    RenderObject();

    RenderAxis();

    glUseProgram(0);

    glBindTextureUnit(0, 0);
    glBindSampler(0, 0);

    glBindVertexArray(0);
}

void CMyApp::RenderGUI()
{
    // ImGui::ShowDemoWindow();
    ImGuiIO& io = ImGui::GetIO();

    static bool show_lighting_settings = false;
    static bool show_poly_settings = true;
    static bool show_object_settings = false;
    static bool show_misc_settings = false;
    static bool show_data_overview = true;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::Button("Save"))
            {
                ImGui::OpenPopup("Save", ImGuiPopupFlags_NoReopen | ImGuiPopupFlags_NoOpenOverExistingPopup);
            }

            if (ImGui::Button("Export to .obj"))
            {
                ImGui::OpenPopup("Export to .obj", ImGuiPopupFlags_NoReopen | ImGuiPopupFlags_NoOpenOverExistingPopup);
            }


            static std::string path_buffer;
            if (ImGui::BeginPopup("Save"))
            {
                ImGui::InputText("File name", &path_buffer);

                if (ImGui::Button("Save"))
                {
                    try
                    {
                        PolySaver::SaveTo(path_buffer, m_Polyhedron);
                    } catch (std::logic_error e)
                    {
                        m_errorHappened = true;
                        m_errorMessage = e.what();
                    }
                }

                ImGui::EndPopup();
            }

            static std::string obj_name;
            if (ImGui::BeginPopup("Export to .obj"))
            {
                ImGui::InputText("File name", &path_buffer);
                ImGui::InputText("Object name:", &obj_name);
                if (ImGui::Button("Export"))
                {
                    ObjSaver::SaveTo(path_buffer, m_Polyhedron.GetIndexedMesh(m_camera.GetAt()), obj_name);
                }

                ImGui::EndPopup();
            }

            ImGui::Separator();

            if (ImGui::Button("Exit"))
                SDL_Quit();


            if (ImGui::Button("About"))
            {
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Visibility"))
        {
            ImGui::Text("Window Visibility");
            ImGui::Checkbox("Lighting", &show_lighting_settings);
            ImGui::Checkbox("Polyhedron", &show_poly_settings);
            ImGui::Checkbox("Data Overview",&show_data_overview);
            ImGui::Checkbox("Object", &show_object_settings);
            ImGui::Checkbox("Misc.", &show_misc_settings);
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (show_lighting_settings && ImGui::Begin("Lighting settings"))
    {
        if (ImGui::BeginTabBar("Lighting & Material"))
        {
            if (ImGui::BeginTabItem("Material"))
            {
                ImGui::InputFloat("Shininess", &m_Shininess, 0.1f, 1.0f, "%.1f");
                static float Kaf = 1.0f;
                static float Kdf = 1.0f;
                static float Ksf = 1.0f;
                if (ImGui::SliderFloat("Ka", &Kaf, 0.0f, 1.0f))
                {
                    m_Ka = glm::vec3(Kaf);
                }
                if (ImGui::SliderFloat("Kd", &Kdf, 0.0f, 1.0f))
                {
                    m_Kd = glm::vec3(Kdf);
                }
                if (ImGui::SliderFloat("Ks", &Ksf, 0.0f, 1.0f))
                {
                    m_Ks = glm::vec3(Ksf);
                }
                ImGui::EndTabItem();
            }

            static int l_id = 1;
            if (ImGui::BeginTabItem("Lightings"))
            {
                ImGui::BeginListBox("Lightings");
                {
                    if (ImGui::Button("Lighting 1")) l_id = 1;
                    if (ImGui::Button("Lighting 2")) l_id = 2;

                    ImGui::EndListBox();
                }

                ImGui::Separator();

                if (l_id == 1)
                {
                    ImGuiWidgets::RenderLightGUI(m_light1);
                }
                else if (l_id == 2)
                {
                    ImGuiWidgets::RenderLightGUI(m_light2);
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
        ImGui::End();
    }

    if (show_poly_settings && ImGui::Begin("Polyhedron Settings"))
    {
        if (ImGui::BeginTabBar("Polyhedron Settings"))
        {
            if (ImGui::BeginTabItem("Attributes"))
            {
                auto translate = m_Polyhedron.GetLocalTransform().GetTranslationComponent();
                if (ImGui::InputFloat3("Position", value_ptr(translate)))
                {
                    m_Polyhedron.GetLocalTransform().SetTranslationComponent(translate);
                }

                auto rotation = m_Polyhedron.GetLocalTransform().GetRotationComponent();
                if (ImGui::InputFloat3("Rotation", value_ptr(rotation)))
                {
                    m_Polyhedron.GetLocalTransform().SetRotationComponent(rotation);
                }

                auto scale = m_Polyhedron.GetLocalTransform().GetScalingComponent();
                if (ImGui::InputFloat3("Scale", value_ptr(scale)))
                {
                    m_Polyhedron.GetLocalTransform().SetScalingComponent(scale);
                }

                ImGui::Separator();

                if (m_Polyhedron.IsInstantiated())
                {
                    int number_of_edges = m_Polyhedron.GetActiveFace()->GetEdgeCount();
                    if(ImGui::InputInt("Number of edges:", &number_of_edges,1,5))
                    {
                        if (number_of_edges < 3)
                        {
                            number_of_edges = 3;
                        }
                    }
                    if(ImGui::IsItemDeactivatedAfterEdit())
                    {
                        m_Polyhedron.SetEdgeCountOfActive(number_of_edges);
                    }

                    float max_pivot_val = m_Polyhedron.GetActiveFace()->GetPivotVal();
                    if (ImGui::SliderFloat("Pivot value",&max_pivot_val,0,glm::two_pi<float>()))
                    {
                        m_Polyhedron.SetPivotOfActive(max_pivot_val);
                    }

                }




                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Controls"))
            {
                if (ImGui::Checkbox("Animate?", &m_animate))
                {
                }

                if (ImGui::SliderFloat("Animation Speed", &m_animationSpeed, 0.001, 10.0f))
                {
                }

                if (ImGui::SliderFloat("Animation state", &m_currentFoldValue, 0.0f, 1.0f))
                {
                    m_animate = false;

                    m_animationState = m_currentFoldValue;
                    m_Polyhedron.SetAnimationState(m_currentFoldValue);
                }

                static bool focusOnInput = false;
                static std::string command_buffer;
                ImGui::InputTextWithHint("Input Command", "START 4", &command_buffer,
                                         ImGuiInputTextFlags_CharsUppercase);

                if (focusOnInput)
                {
                    ImGui::SetKeyboardFocusHere(-1);
                    focusOnInput = false;
                }

                if (ImGui::Button("Run Command") || ImGui::IsKeyPressed(ImGuiKey_Enter))
                {
                    if (!command_buffer.empty())
                    {
                        std::string temp = command_buffer.data();
                        PolyParser::SetDataFromInput(temp);

                        try
                        {
                            PolyParser::Parse(m_Polyhedron);
                        }
                        catch (std::invalid_argument& e)
                        {
                            m_errorHappened = true;
                            m_errorMessage = e.what();
                        }
                        catch (std::logic_error& e)
                        {
                            m_errorHappened = true;
                            m_errorMessage = e.what();
                        }
                        catch (...)
                        {
                            m_errorHappened = true;
                            m_errorMessage = "Unknown error";
                        }
                    }
                    focusOnInput = true;
                }

                ImGui::EndTabItem();
            }

            if(ImGui::BeginTabItem("Information"))
            {
                auto addBullet = [](const char* commandName, const char* commandDesc)
                {
                    ImGui::BulletText("%s", commandName);
                    ImGui::SameLine();
                    ImGui::Text("%s", commandDesc);
                };
                ImGui::Text("Commands:");
                addBullet("START n:", "Initialize an N-gon with 'n' sides");
                addBullet("ADD e n pivot:",
                          "Attach an 'n' side N-gon to the 'e'th side of the active polygon.\n\t'n': Optional argument. If not specified, use the last input 'n' value.\n\t'pivot': Optional argument. If not specified, lookup the pivot value between\n the current active polygon, and the input polygon.");
                addBullet("PUSH e n pivot:",
                          "Attach an 'n' side N-gon to the 'e'th side of the active polygon. Changes the active side to the input polygon.\n\t'n': Optional argument. If not specified, use the last input 'n' value.\n\t'pivot': Optional argument. If not specified, lookup the pivot value between the current active polygon,\n and the input polygon.");
                addBullet("POP", "Makes the current active polygon's parent into the active polygon");
                addBullet("PIVOT n m p", "Specifies the default value between 'n' and 'm' side polygons to 'p'");
                addBullet("PIVOT_POLY n m o",
                          "Calculates the default value between 'n' and 'm' assuming they meet at 'o' regular polygon");
                addBullet("PIVOT_VERTEX n m o",
                          "PIVOT_POLY, but for all possible combinations between 'n', 'm' and 'o'");
                ImGui::Separator();
                addBullet("CLEAR", "Clears the current polygon");
                addBullet("SAVE_TO_POLY path", "Save the current poly to the specified path");
                addBullet("SAVE_TO_OBJ path", "Save the current poly into an .obj mesh to the specified path");
                addBullet("SET_N n", "Sets the number of edges of the active polygon. Destructive operation");
                addBullet("SET_PIVOT p", "Sets the pivot value of the active polygon");
                addBullet("REMOVE e", "Removes the polygon attached to the 'e'nth edge of the active polygon");

                ImGui::EndTabItem();

            }


            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    if (show_data_overview && ImGui::Begin("Data Overview"))
    {
        if(ImGui::BeginTabBar("Data"))
        {
            if (ImGui::BeginTabItem("Data"))
            {

                ImGuiWidgets::RenderPolygonTable(m_Polyhedron);

                ImGuiWidgets::RenderPivotTable();

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    if (show_object_settings && ImGui::Begin("Object Settings"))
    {
        if (ImGui::BeginChild("Transform", ImVec2(0, 80)))
        {
            auto translate = m_PolyhedronObject.GetLocalTransform().GetTranslationComponent();
            if (ImGui::InputFloat3("Position", value_ptr(translate)))
            {
                m_PolyhedronObject.GetLocalTransform().SetTranslationComponent(translate);
            }

            auto rotation = m_PolyhedronObject.GetLocalTransform().GetRotationComponent();
            if (ImGui::InputFloat3("Rotation", value_ptr(rotation)))
            {
                m_PolyhedronObject.GetLocalTransform().SetRotationComponent(rotation);
            }

            auto scale = m_PolyhedronObject.GetLocalTransform().GetScalingComponent();
            if (ImGui::InputFloat3("Scale", value_ptr(scale)))
            {
                m_PolyhedronObject.GetLocalTransform().SetScalingComponent(scale);
            }
        }
        ImGui::EndChild();

        if (ImGui::Button("Delete"))
        {
            m_PolyhedronObject.ClearObject();
        }

        ImGui::End();
    }

    if (show_misc_settings && ImGui::Begin("Misc Settings"))
    {
        ImGui::Text("Current Texture:");
        ImGuiWidgets::RenderImage(m_PolyhedronTextureID, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1));
        ImGui::End();
    }

    if (m_errorHappened)
    {
        ImGui::OpenPopup("Error");
        m_errorHappened = false;
    }

    if (ImGui::BeginPopup("Error"))
    {
        ImGui::Text("Error occurred: %s", m_errorMessage.data());
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

// https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL2/SDL_Keysym
// https://wiki.libsdl.org/SDL2/SDL_Keycode
// https://wiki.libsdl.org/SDL2/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
    if (key.repeat == 0) // Először lett megnyomva
    {
        if (key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL)
        {
            CleanShaders();
            InitShaders();
        }
        if (key.keysym.sym == SDLK_F1)
        {
            GLint polygonModeFrontAndBack[2] = {};
            // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
            glGetIntegerv(GL_POLYGON_MODE, polygonModeFrontAndBack);
            // Kérdezzük le a jelenlegi polygon módot! Külön adja a front és back módokat.
            GLenum polygonMode = (polygonModeFrontAndBack[0] != GL_FILL ? GL_FILL : GL_LINE);
            // Váltogassuk FILL és LINE között!
            // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
            glPolygonMode(GL_FRONT_AND_BACK, polygonMode); // Állítsuk be az újat!
        }
    }
    m_cameraManipulator.KeyboardDown(key);
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
    m_cameraManipulator.KeyboardUp(key);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
    m_cameraManipulator.MouseMove(mouse);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
    m_cameraManipulator.MouseWheel(wheel);
}


// a két paraméterben az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
    glViewport(0, 0, _w, _h);
    m_camera.SetAspect(static_cast<float>(_w) / _h);
}

// Le nem kezelt, egzotikus esemény kezelése
// https://wiki.libsdl.org/SDL2/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{
    if (ev.type == SDL_DROPFILE || ev.type == SDL_DROPTEXT)
    {
        std::string filename = std::string(ev.drop.file);

        if (filename.rfind(".poly") != std::string::npos)
        {
            m_Polyhedron.Reset();
            PolyParser::SetDataFromFile(filename);

            try
            {
                PolyParser::Parse(m_Polyhedron);
            }
            catch (std::invalid_argument& e)
            {
                m_errorHappened = true;
                m_errorMessage = e.what();
            }
            catch (std::logic_error& e)
            {
                m_errorHappened = true;
                m_errorMessage = e.what();
            }
            catch (...)
            {
                m_errorHappened = true;
                m_errorMessage = "Unknown error";
            }
        }
        else if (filename.rfind(".obj") != std::string::npos)
        {
            auto mesh = ObjParser::parse(filename);
            std::initializer_list<VertexAttributeDescriptor> vertexAttribList = {
                {0, offsetof(Vertex, position), 3, GL_FLOAT},
                {1, offsetof(Vertex, normal), 3, GL_FLOAT},
                {2, offsetof(Vertex, texcoord), 2, GL_FLOAT},
            };

            m_PolyhedronObject = ObjectWrapper(CreateGLObjectFromMesh(mesh, vertexAttribList));
        }
        else if (filename.rfind(".png") != std::string::npos || filename.rfind(".jpg") != std::string::npos)
        {
            LoadTexture(filename);
        }
        SDL_free(ev.drop.file);
    }
}
