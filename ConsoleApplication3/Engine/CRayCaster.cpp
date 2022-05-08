#pragma once
#include "CRayCaster.h"
#include "Math.h"

using namespace Engine;

RayHit Engine::CRayCaster::TraceRay(const ImVec2& start, const float angle)
{
    RayHit hit;

    float step = 0.f;

    while (!hit.isHit and step <= m_fLimit)
    {
        hit.endPoint = Transform(start, angle, step);

        const int x = hit.endPoint.x / 30;
        const int y = hit.endPoint.y / 30;

        if (x >= m_pMap->size() or y >= m_pMap->at(0).size())
            break;
        
        if (m_pMap->at(y).at(x) == 1)
        {
            hit.isHit = true;

            return hit;
        }
        step += m_fStepSize;
    }

    return hit;
}

Engine::CRayCaster::CRayCaster(const float fLimit, const std::array<std::array<int, 11>, 11>* pMap)
{
    m_fLimit = fLimit;
    m_pMap  = pMap;
}
