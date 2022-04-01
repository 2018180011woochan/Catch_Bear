#pragma once
class Player;
class IdleState;

class PlayerState
{
public:
	virtual ~PlayerState() { }

public:
	// ���� ���� Ű�� �޾ƾ� ��
	virtual void KeyCheck(Player& player);
	virtual void Update(shared_ptr<Player> player) {}

public:
	static IdleState idle;
};

