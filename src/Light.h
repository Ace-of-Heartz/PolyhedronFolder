//
// Created by ace on 2024.12.26..
//

#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <X11/Xlib.h>


class Light {
public:
    Light(glm::vec4 posDir,glm::vec3 lAmbient, glm::vec3 lDiffuse, glm::vec3 lSpecular)
    : m_posDir(posDir),m_La(lAmbient), m_Ld(lDiffuse), m_Ls(lSpecular) {}

    [[nodiscard]] glm::vec4 GetPosDir() const {return m_posDir;}
    [[nodiscard]] glm::vec3 GetAmbientComp() const {return m_La;};
    [[nodiscard]] glm::vec3 GetDiffuseComp() const {return m_Ld;};
    [[nodiscard]] glm::vec3 GetSpecularComp() const {return m_Ls;};

    void SetPosDir(glm::vec4 posDir) {m_posDir = posDir;}

    void SetAmbientComp(const glm::vec3 &la) {
        m_La = la;
    }
    void SetDiffuseComp(const glm::vec3 &ld) {
        m_Ld = ld;
    }
    void SetSpecularComp(const glm::vec3 &ls) {
        m_Ls = ls;
    }



protected:
    glm::vec4 m_posDir;
    glm::vec3 m_La;
    glm::vec3 m_Ld;
    glm::vec3 m_Ls;

};






#endif //LIGHT_H
