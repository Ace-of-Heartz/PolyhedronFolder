#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ObjParser.h"
#include "ParametricSurfaceMesh.hpp"
#include "ProgramBuilder.h"

#include <imgui.h>

#include <iostream>
#include <string>
#include <array>
#include <algorithm>

#include "PolyParser.h"
#include "PolyUtils.h"

#include "ImGuiWidgets.h"


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
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
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
	ProgramBuilder{ m_programID }
		.ShaderStage( GL_VERTEX_SHADER, "Shaders/Vert_PosNormTex.vert" )
		.ShaderStage( GL_FRAGMENT_SHADER, "Shaders/Frag_Lighting.frag" )
		.Link();
	m_programAxis = glCreateProgram();
    ProgramBuilder{ m_programAxis}
        .ShaderStage( GL_VERTEX_SHADER, "Shaders/Vert_axes.vert")
        .ShaderStage( GL_FRAGMENT_SHADER, "Shaders/Frag_PosCol.frag")
    	.Link();


}


void CMyApp::CleanShaders()
{
	glDeleteProgram( m_programID );
	glDeleteProgram( m_programAxis);
}


void CMyApp::InitGeometry()
{

	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList = {
		{ 0, offsetof( Vertex, position ), 3, GL_FLOAT },
		{ 1, offsetof( Vertex, normal   ), 3, GL_FLOAT },
		{ 2, offsetof( Vertex, texcoord ), 2, GL_FLOAT },
	};
	auto polyMeshCPU = m_Polyhedron.GetTransformedMesh(m_baseTransformation,m_camera.GetEye());
	m_PolyhedronPoly = CreateGLObjectFromMesh(polyMeshCPU,vertexAttribList);

}

void CMyApp::CleanGeometry()
{
	CleanOGLObject( m_PolyhedronPoly );
}

void CMyApp::InitTextures()
{
	// sampler

	glCreateSamplers( 1, &m_SamplerID );
	glSamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glSamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glSamplerParameteri( m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glSamplerParameteri( m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	ImageRGBA PolyhedronImage = ImageFromFile( "Assets/wood.jpg");

	glCreateTextures( GL_TEXTURE_2D, 1, &m_PolyhedronTextureID );
	glTextureStorage2D( m_PolyhedronTextureID, NumberOfMIPLevels( PolyhedronImage ), GL_RGBA8, PolyhedronImage.width, PolyhedronImage.height );
	glTextureSubImage2D( m_PolyhedronTextureID, 0, 0, 0, PolyhedronImage.width, PolyhedronImage.height, GL_RGBA, GL_UNSIGNED_BYTE, PolyhedronImage.data() );

	glGenerateTextureMipmap( m_PolyhedronTextureID );
}

void CMyApp::LoadTexture(const std::string& filename) {
	ImageRGBA PolyhedronImage = ImageFromFile( filename );

	glCreateTextures( GL_TEXTURE_2D, 1, &m_PolyhedronTextureID );
	glTextureStorage2D( m_PolyhedronTextureID, NumberOfMIPLevels( PolyhedronImage ), GL_RGBA8, PolyhedronImage.width, PolyhedronImage.height );
	glTextureSubImage2D( m_PolyhedronTextureID, 0, 0, 0, PolyhedronImage.width, PolyhedronImage.height, GL_RGBA, GL_UNSIGNED_BYTE, PolyhedronImage.data() );

	glGenerateTextureMipmap( m_PolyhedronTextureID );
}


void CMyApp::CleanTextures()
{
	glDeleteTextures( 1, &m_PolyhedronTextureID );
}


bool CMyApp::Init()
{
	SetupDebugCallback();

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitGeometry();
	InitTextures();

	glPointSize( 16.0f );
	glLineWidth( 4.0f );

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hátrafelé néző lapok eldobását
	glCullFace(GL_BACK);    // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok

	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	// kamera
	m_camera.SetView(
		glm::vec3(0.0, 7.0, 7.0),	// honnan nézzük a színteret	   - eye
		glm::vec3(0.0, 0.0, 0.0),   // a színtér melyik pontját nézzük - at
		glm::vec3(0.0, 1.0, 0.0));  // felfelé mutató irány a világban - up

	m_cameraManipulator.SetCamera( &m_camera );

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
}

void CMyApp::Update( const SUpdateInfo& updateInfo )
{
	if (m_animate) {
		m_currentFoldValue += updateInfo.DeltaTimeInSec * m_animationSpeed;
		m_animationState += updateInfo.DeltaTimeInSec * m_animationSpeed;
		if ( m_animationState >= 2.0f )
		{
			m_animationState = m_animationState - 2.0f;
		}
		else if (m_animationState >= 1.0f)
		{
			m_currentFoldValue = 2.0f - m_animationState;
			m_Polyhedron.SetFoldVal(m_currentFoldValue);
		} else
		{
			m_currentFoldValue = m_animationState;
			m_Polyhedron.SetFoldVal(m_currentFoldValue);
		}
	}

	InitGeometry();
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

	m_cameraManipulator.Update( updateInfo.DeltaTimeInSec );
}

void CMyApp::SetLightingUniforms( GLuint program, float Shininess, glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks )
{
	// - Fényforrások beállítása
	glProgramUniform3fv( program, ul( program, "cameraPos" ), 1, glm::value_ptr( m_camera.GetEye() ) );

	glProgramUniform4fv( program, ul( program, "lightPosDir1" ),  1, glm::value_ptr( m_light1.GetPosDir()) );
	glProgramUniform3fv( program, ul( program, "La1" ),		 1, glm::value_ptr( m_light1.GetAmbientComp() ) );
	glProgramUniform3fv( program, ul( program, "Ld1" ),		 1, glm::value_ptr( m_light1.GetDiffuseComp()) );
	glProgramUniform3fv( program, ul( program, "Ls1" ),		 1, glm::value_ptr( m_light1.GetSpecularComp()) );

	glProgramUniform4fv( program, ul( program, "lightPosDir2" ),  1, glm::value_ptr( m_light2.GetPosDir()) );
	glProgramUniform3fv( program, ul( program, "La2" ),		 1, glm::value_ptr( m_light2.GetAmbientComp() ) );
	glProgramUniform3fv( program, ul( program, "Ld2" ),		 1, glm::value_ptr( m_light2.GetDiffuseComp()) );
	glProgramUniform3fv( program, ul( program, "Ls2" ),		 1, glm::value_ptr( m_light2.GetSpecularComp()) );

	glProgramUniform1f( program, ul( program, "lightConstantAttenuation"	 ), m_lightConstantAttenuation );
	glProgramUniform1f( program, ul( program, "lightLinearAttenuation"	 ), m_lightLinearAttenuation   );
	glProgramUniform1f( program, ul( program, "lightQuadraticAttenuation" ), m_lightQuadraticAttenuation);

	// - Anyagjellemzők beállítása
	glProgramUniform3fv( program, ul( program, "Ka" ),		 1, glm::value_ptr( Ka ) );
	glProgramUniform3fv( program, ul( program, "Kd" ),		 1, glm::value_ptr( Kd ) );
	glProgramUniform3fv( program, ul( program, "Ks" ),		 1, glm::value_ptr( Ks ) );

	glProgramUniform1f( program, ul( program, "Shininess" ),	Shininess );
}

void CMyApp::RenderPolyhedron() {
	glProgramUniformMatrix4fv( m_programID, ul( m_programID,"viewProj"), 1, GL_FALSE, glm::value_ptr( m_camera.GetViewProj() ) );

	auto matWorld = glm::mat4( 1.0f );

	glProgramUniformMatrix4fv( m_programID, ul( m_programID,"world" ),    1, GL_FALSE, glm::value_ptr( matWorld ) );
	glProgramUniformMatrix4fv( m_programID, ul( m_programID,"worldIT" ),  1, GL_FALSE, glm::value_ptr( glm::transpose( glm::inverse( matWorld ) ) ) );

	SetLightingUniforms( m_programID, m_Shininess, m_Ka, m_Kd, m_Ks );

	glProgramUniform1i( m_programID, ul( m_programID,"texImage" ), 0 );

	glBindTextureUnit( 0, m_PolyhedronTextureID );
	glBindSampler( 0, m_SamplerID );

	glBindVertexArray( m_PolyhedronPoly.vaoID );

	glUseProgram( m_programID );

	glDrawElements( GL_TRIANGLES,
					m_PolyhedronPoly.count,
					GL_UNSIGNED_INT,
					nullptr );
}

void CMyApp::RenderObject() {

}

void CMyApp::RenderAxis() {
	glm::mat4 matWorld = glm::translate(glm::vec3(0,0,0));

	glDisable(GL_DEPTH_TEST);

	glProgramUniform1f(m_programAxis, ul(m_programAxis, "mult"), 0.5f);

	glProgramUniformMatrix4fv(m_programAxis, ul(m_programAxis, "viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
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

	glUseProgram( 0 );

	glBindTextureUnit( 0, 0 );
	glBindSampler( 0, 0 );

	glBindVertexArray( 0 );
}

void CMyApp::RenderGUI()
{
	if ( ImGui::Begin( "Lighting settings" ) )
	{
		if (ImGui::BeginTabBar( "Lighting & Material" )) {
			if (ImGui::BeginTabItem( "Material" )) {
				ImGui::InputFloat("Shininess", &m_Shininess, 0.1f, 1.0f, "%.1f" );
				static float Kaf = 1.0f;
				static float Kdf = 1.0f;
				static float Ksf = 1.0f;
				if ( ImGui::SliderFloat( "Ka", &Kaf, 0.0f, 1.0f ) )
				{
					m_Ka = glm::vec3( Kaf );
				}
				if ( ImGui::SliderFloat( "Kd", &Kdf, 0.0f, 1.0f ) )
				{
					m_Kd = glm::vec3( Kdf );
				}
				if ( ImGui::SliderFloat( "Ks", &Ksf, 0.0f, 1.0f ) )
				{
					m_Ks = glm::vec3( Ksf );
				}
			ImGui::EndTabItem();
			}

			if(ImGui::BeginTabItem("1. Lighting"))
			{
				ImGuiWidgets::RenderLightGUI(m_light1);
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("2. Lighting"))
			{
				ImGuiWidgets::RenderLightGUI(m_light2);
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();

	}
	ImGui::End();

	if ( ImGui::Begin("Polygon Settings")) {

		if(ImGui::BeginChild("Controls")) {



			if(ImGui::Checkbox("Animate?", &m_animate)) {
			}

			if (ImGui::SliderFloat("Animation Speed", &m_animationSpeed,0.001,10.0f)) {

			}

			if(ImGui::SliderFloat("Animation state",&m_currentFoldValue,0.0f,1.0f)) {
				m_animate = false;

				m_animationState = m_currentFoldValue;
				m_Polyhedron.SetFoldVal(m_currentFoldValue);
			}

			static bool focusOnInput = false;
			static ImVector<char> buffer = ImVector<char>();
			ImGui::InputTextWithHint("Input Command","ADD 2 4...",reinterpret_cast<char*>(&buffer),1024,ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsUppercase,
				[](ImGuiInputTextCallbackData* data) {
					auto buffer = static_cast<ImVector<char> *>(data->UserData);
					//IM_ASSERT(buffer->begin() == data->Buf);
					buffer->resize(data->BufSize);
					data->Buf = buffer->begin();
					return 0;
				}, (void*)&buffer
 			);

			if(focusOnInput)
			{
				ImGui::SetKeyboardFocusHere(-1);
				focusOnInput = false;
			}

			if(ImGui::Button("Run Command") || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
				if (!buffer.empty()) {
					std::string temp = buffer.Data;
					PolyParser::SetDataFromInput(temp);
					PolyParser::Parse(m_Polyhedron);
				}
				focusOnInput = true;
			}
		}
		ImGui::EndChild();

		if (ImGui::BeginChild("View")) {


		}
		ImGui::EndChild();



	}
	ImGui::End();

}

// https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL2/SDL_Keysym
// https://wiki.libsdl.org/SDL2/SDL_Keycode
// https://wiki.libsdl.org/SDL2/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if ( key.repeat == 0 ) // Először lett megnyomva
	{
		if ( key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL )
		{
			CleanShaders();
			InitShaders();
		}
		if ( key.keysym.sym == SDLK_F1 )
		{
			GLint polygonModeFrontAndBack[ 2 ] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv( GL_POLYGON_MODE, polygonModeFrontAndBack ); // Kérdezzük le a jelenlegi polygon módot! Külön adja a front és back módokat.
			GLenum polygonMode = ( polygonModeFrontAndBack[ 0 ] != GL_FILL ? GL_FILL : GL_LINE ); // Váltogassuk FILL és LINE között!
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode( GL_FRONT_AND_BACK, polygonMode ); // Állítsuk be az újat!
		}
	}
	m_cameraManipulator.KeyboardDown( key );
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_cameraManipulator.KeyboardUp( key );
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove( mouse );
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
	m_cameraManipulator.MouseWheel( wheel );
}


// a két paraméterben az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_camera.SetAspect( static_cast<float>(_w) / _h );
}

// Le nem kezelt, egzotikus esemény kezelése
// https://wiki.libsdl.org/SDL2/SDL_Event

#include <iostream>
void CMyApp::OtherEvent( const SDL_Event& ev )
{
	if ( ev.type == SDL_DROPFILE || ev.type == SDL_DROPTEXT) {

		std::string filename = std::string(ev.drop.file);

		if (filename.rfind(".poly") != std::string::npos) {
			m_Polyhedron.Reset();
			PolyParser::SetDataFromFile(filename);
			PolyParser::Parse(m_Polyhedron);

		}
		else if (filename.rfind(".ojb") != std::string::npos) {
			auto mesh = ObjParser::parse(filename);
			const std::initializer_list<VertexAttributeDescriptor> vertexAttribList = {
				{ 0, offsetof( Vertex, position ), 3, GL_FLOAT },
				{ 1, offsetof( Vertex, normal   ), 3, GL_FLOAT },
				{ 2, offsetof( Vertex, texcoord ), 2, GL_FLOAT },
			};

			m_PolyhedronObject = CreateGLObjectFromMesh(mesh,vertexAttribList);
		}
		else if (filename.rfind(".png") != std::string::npos) {
			LoadTexture(filename);
		}
		SDL_free(ev.drop.file);
	}
}


