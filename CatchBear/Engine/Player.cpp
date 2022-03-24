#include "pch.h"
#include "Player.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "AnimationController.h"
#include "AnimationTrack.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::LateUpdate()
{
	//GetTransform()->PreRender();
	KeyCheck();
	GetAnimationController()->AdvanceTime(DELTA_TIME);
	GetTransform()->UpdateTransform(NULL);
	GetAnimationController()->SetWorldMatrix();
}

void Player::KeyCheck()
{
	Vec3 pos = GetTransform()->GetLocalPosition();


	if (INPUT->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::A))
		pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::D))
		pos += GetTransform()->GetRight() * _speed * DELTA_TIME;


	GetTransform()->SetLocalPosition(pos);

}
