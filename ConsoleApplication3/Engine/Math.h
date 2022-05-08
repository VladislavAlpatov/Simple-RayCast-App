#pragma once
#include "../imgui/imgui.h"

namespace Engine
{
	float ImVec2DistTo(const ImVec2& a, const ImVec2& b);
	ImVec2 Transform(const ImVec2& src, const float angle, const float length);
}

