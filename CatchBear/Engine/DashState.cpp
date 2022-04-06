#include "pch.h"
#include "DashState.h"
#include "Timer.h"
#include "Player.h"
#include "AnimationController.h"
#include "Input.h"
#include "IdleState.h"
#include "DashRestState.h"
#include "GameObject.h"

PlayerState* DashState::KeyCheck(GameObject& player, STATE ePlayer)
{
    if (INPUT->GetButton(KEY_TYPE::UP) || INPUT->GetButton(KEY_TYPE::DOWN))
    {
        return NULL;
    }
    else
    {
        ePlayer = STATE::DASH_REST;
        return new DashRestState(_fDashTime);
    }
    return NULL;
}

PlayerState* DashState::Update(GameObject& player, STATE ePlayer)
{
    // 5�� ���ȸ� ������
    _fDashTime += DELTA_TIME;
    if (_fDashTime >= 5.f)
    {
        int k = 0;
        _fDashTime = 0.f;
        ePlayer = STATE::IDLE;
        return new IdleState;
    }

    return NULL;
}

void DashState::Enter(GameObject& player)
{
    player.GetAnimationController()->SetTrackAnimationSet(0, 3);
}

void DashState::End(GameObject& player)
{
}