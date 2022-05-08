#pragma once
#include "../imgui/imgui.h"
#include <array>

namespace Engine
{

	struct RayHit
	{
		bool isHit = false;
		ImVec2 endPoint;
	};


	class CRayCaster
	{
	public:
		CRayCaster(const float fLimit, const std::array<std::array<int, 11>, 11>*);
		RayHit TraceRay(const ImVec2& start, const float angle);
		float		  m_fLimit;
		float         m_fStepSize = 0.5f;
	private:
		const std::array<std::array<int, 11>, 11>* m_pMap;
	};

}

