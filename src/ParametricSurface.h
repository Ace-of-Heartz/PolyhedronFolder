//
// Created by ace on 2024.10.19..
//
#ifndef PARAMETRICSURFACES_H
#define PARAMETRICSURFACES_H

#include <iostream>
#include <glm/glm.hpp>




class ParamSurf {
public:
    [[nodiscard]] virtual glm::vec3 GetPos(float u,float v) const noexcept = 0;
    [[nodiscard]] virtual glm::vec3 GetNorm(float u, float v) const noexcept {
        float const h = ParamSurf::GetOffset();

        glm::vec3 const du = (GetPos(u + h, v) - GetPos(u, v)) / h;
        glm::vec3 const dv = (GetPos(u, v + h) - GetPos(u, v)) / h;
        return normalize(cross(du,dv));
    }
    [[nodiscard]] virtual glm::vec2 GetTex(float u, float v) const noexcept {
        return {u,v};
    }
private:
    [[nodiscard]] virtual float GetOffset() const noexcept {
        return 0.01f;
    }

};

class Quad : public ParamSurf{
public:
    [[nodiscard]] glm::vec3 GetPos(float u,float v) const noexcept override {
        return {u,v,0.0f};
    }
    [[nodiscard]] glm::vec3 GetNorm(float u, float v) const noexcept override {
        return {0.0f,0.0f,1.0f};
    }
};

class Sphere : public ParamSurf {
public:
    [[nodiscard]] glm::vec3 GetPos( float u, float v ) const noexcept override
    {
        u = u * glm::pi<float>();

        v = glm::two_pi<float>() * v;
        return {sin(u) * cos(v),sin(u) * sin(v), cos(u)};
    }

    // TODO: Debug sphere norm
    // [[nodiscard]] glm::vec3 GetNorm( float u, float v ) const noexcept override
    // {
    //     glm::vec3 const du = {cos(u) * cos(v), cos(u) * sin(v), -sin(u)};
    //     glm::vec3 const dv = {sin(u) * -sin(v), sin(u) * cos(v),0.f};
    //
    //     return normalize(glm::vec3(sin(u) * cos(v),sin(u) * cos(v),cos(u) ));
    // }
};

class Bezier3x3 : public ParamSurf {
public:
    explicit Bezier3x3(std::array<std::array<glm::vec3,3>,3> ps) : m_ps(ps) {}
    ~Bezier3x3() {}

    [[nodiscard]] glm::vec3 GetPos(float u, float v) const noexcept override {
        float Bu[3] = {(1-u) * (1-u), (1-u) * u * 2, u * u };
        float Bv[3] = {(1-v) * (1-v), (1-v) * v * 2, v * v };

        glm::vec3 sum = glm::vec3(0,0,0);
        for(int i = 0; i < 3; ++i) {
            for(int j = 0; j < 3; ++i) {
                sum += Bu[i] * Bv[j] * m_ps[i][j];
            }
        }
        return sum;
    }

private:
    std::array<std::array<glm::vec3,3>,3> m_ps;

};


template <int N,int M>
class BezierNxM : public ParamSurf {
public:
    explicit BezierNxM(std::array<std::array<glm::vec3,M>,N> ps) : m_ps(ps){}
    ~BezierNxM() {}

    [[nodiscard]] glm::vec3 GetPos( float u, float v ) const noexcept override {
        return DeCasteljau2D(v,u);
    }

private:
    glm::vec3 DeCasteljau2D(float t,float s) const {
        std::array<std::array<glm::vec3,M>,N> bs = m_ps;

        for (int k = 0; k < N; ++k) {
            std::array<glm::vec3,M> b_temp = std::array<glm::vec3,M>();
            for (int j = 1; j < M; ++j ) {
                for (int i = 0; i < M-j; ++i) {
                    {
                        b_temp[i] = bs[k][i] * (1-t) + bs[k][i+1] * t;
                    }
                }
                bs[k] = b_temp;
            }
        }

        std::array<glm::vec3, N> temp_bs = std::array<glm::vec3, N>();
        for(int k = 0; k < N; ++k) {
            temp_bs[k] = bs[k][0];
        }

        std::array<glm::vec3 ,N> b_temp = std::array<glm::vec3,N>();
        for (int j = 1; j < N; ++j) {
            for (int i = 0; i < N-j; ++i) {
                b_temp[i] = temp_bs[i] * (1-s) + temp_bs[i+1] * s;
            }
            temp_bs = b_temp;
        }
        return  temp_bs[0];
    }

    std::array<std::array<glm::vec3,M>,N> m_ps;

};





#endif //PARAMETRICSURFACES_H
