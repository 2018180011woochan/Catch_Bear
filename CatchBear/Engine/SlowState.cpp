#include "pch.h"
#include "SlowState.h"
#include "Input.h"
#include "Player.h"
#include "AnimationController.h"
#include "DashRestState.h"
#include "Timer.h"
#include "IdleState.h"
#include "SlowRestState.h"
#include "GameObject.h"

PlayerState* SlowState::KeyCheck(GameObject& player, STATE ePlayer)
{
    if (INPUT->GetButton(KEY_TYPE::UP) || INPUT->GetButton(KEY_TYPE::DOWN))
    {
        return NULL;
    }
    else
    {
        ePlayer = STATE::SLOW_REST;
        return new SlowRestState(_fTime);
    }
    return NULL;
}

PlayerState* SlowState::Update(GameObject& player, STATE ePlayer)
{
    // 5�� ���ȸ� ������
    _fTime += DELTA_TIME;
    if (_fTime >= 5.f)
    {
        _fTime = 0.f;
        ePlayer = STATE::IDLE;
        return new IdleState;
    }

    return NULL;
}

void SlowState::Enter(GameObject& player)
{
    player.GetAnimationController()->SetTrackAnimationSet(0, 1);
    player.GetAnimationController()->SetTrackSpeed(0, 0.2f);
}

void SlowState::End(GameObject& player)
{
    player.GetAnimationController()->SetAnimationPosition(0, 0.f);
    player.GetAnimationController()->SetTrackSpeed(0, 1.f);
}