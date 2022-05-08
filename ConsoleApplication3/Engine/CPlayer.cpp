#include "CPlayer.h"
#include "Math.h"

Engine::CPlayer::CPlayer(const ImVec2& origin, float stepSize, float fov)
{
	m_fFieldOfView = fov;
	m_vecOrigin    = origin;
	m_fMoveSpeed   = stepSize;
}

void Engine::CPlayer::MoveForward()
{
	m_vecOrigin = Transform(m_vecOrigin, m_fViewAngle, m_fMoveSpeed);

}

void Engine::CPlayer::MoveBackWard()
{
	m_vecOrigin = Transform(m_vecOrigin, m_fViewAngle + 180.f, m_fMoveSpeed);
}
