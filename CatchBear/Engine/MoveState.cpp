#include "pch.h"
#include "MoveState.h"
#include "Input.h"
#include "IdleState.h"
#include "Player.h"
#include "AnimationController.h"
#include "JumpState.h"
#include "AttackState.h"
#include "GameObject.h"
#include "Timer.h"

PlayerState* MoveState::KeyCheck(GameObject& player)
{
	if (INPUT->GetButton(KEY_TYPE::UP) || INPUT->GetButton(KEY_TYPE::DOWN))
	{
		// ���� ���� ����
		return NULL;
	}
	else if (INPUT->GetButtonDown(KEY_TYPE::SPACE)) return new JumpState;
	else if (INPUT->GetButtonDown(KEY_TYPE::CTRL))	return new AttackState;
	else return new IdleState;
}

PlayerState* MoveState::Update(GameObject& player)
{
	// �÷��̾ �ӵ� ���� �����϶�
	if (player.GetCurItem(Player::ITEM::SPEED_DOWN))
	{
		_fSpeedDownTime += DELTA_TIME;

		if (_fSpeedDownTime <= 5.f)
		{
			player.SetPlayerSpeed(_fSlowSpeed);
		}

		else
		{
			player.SetPlayerSpeed(player.GetPlayerOriginalSpeed());
			_fSpeedDownTime = 0.f;
			player.SetCurItem(Player::ITEM::SPEED_DOWN, false);
		}

	}

	return NULL;
}

void MoveState::Enter(GameObject& player)
{
	player.GetAnimationController()->SetTrackAnimationSet(0, 1);
}

void MoveState::End(GameObject& player)
{
	player.GetAnimationController()->SetAnimationPosition(0, 0.f);
}
