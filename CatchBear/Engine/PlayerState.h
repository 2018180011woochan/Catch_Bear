#pragma once
class Player;

class PlayerState
{
public:
	PlayerState() { }
	virtual ~PlayerState() { }

public:
	// ���� ���� Ű�� �޾ƾ� ��
	virtual PlayerState* KeyCheck(Player& player) = 0;
	virtual void Update(Player& player) = 0;
	virtual void Enter(Player& player) = 0;

public:
	static class IdleState idle;
	static class MoveState move;
};

