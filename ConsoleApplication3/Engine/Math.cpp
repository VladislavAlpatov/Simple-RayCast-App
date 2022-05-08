#include "Math.h"


#define _USE_MATH_DEFINES
#include <cmath>

float Engine::ImVec2DistTo(const ImVec2& a, const ImVec2& b)
{
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

ImVec2 Engine::Transform(const ImVec2& src, const float angle, const float length)
{
    ImVec2 trasnformedVec;

    trasnformedVec.x = src.x + (cosf(-angle * (M_PI / 180.f)) * length);
    trasnformedVec.y = src.y + (sinf(-angle * (M_PI / 180.f)) * length);

    return trasnformedVec;

}