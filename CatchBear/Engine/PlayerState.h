#pragma once
class Player;
class GameObject;

class PlayerState
{
public:
	PlayerState() { }
	virtual ~PlayerState() { }

public:
	// ���� ���� Ű�� �޾ƾ� ��
	virtual PlayerState* KeyCheck(GameObject& player) = 0;
	virtual void Update(GameObject& player) = 0;
	virtual void Enter(GameObject& player) = 0;
	virtual void End(GameObject& player) = 0;

public:
	static class IdleState idle;
	static class MoveState move;

	STATE curState = STATE::END;
};

