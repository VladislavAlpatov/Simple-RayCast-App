#pragma once
#include "../imgui/imgui.h"

namespace Engine
{
	class CPlayer
	{
	public:
		CPlayer(const ImVec2& origin, float stepSize, float fov);
		void MoveForward();
		void MoveBackWard();
		ImVec2 m_vecOrigin;
		float  m_fViewAngle = 0.f;
		float  m_fMoveSpeed;
		float  m_fFieldOfView;

	};
}


