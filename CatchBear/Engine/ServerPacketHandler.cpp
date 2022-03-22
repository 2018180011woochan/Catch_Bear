#include "pch.h"
#include "ServerPacketHandler.h"
#include "SceneManager.h"
#include "Input.h"
#include "Scene.h"
#include "GameObject.h"
#include "Player.h"
#include "Transform.h"
#include "Timer.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// ���� ������ �۾���

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	//if (pkt.success() == false)
	//	return true;

	//GPlayer.playerId = pkt.playerid();
	//cout << "�� �÷��̾��� �г����� " << GPlayer.name << "�̸� ";
	//cout << "�� Ŭ���̾�Ʈ�� ID�� " << GPlayer.playerId << "�Դϴ�!" << endl;
	//
	//if (pkt.players().size() == 0)
	//{
	//	// ĳ���� ����â
	//}
	//
	//// ���� UI ��ư ������ �κ�â ����
	//Protocol::C_ENTER_LOBBY enterLobbyPkt;
	//enterLobbyPkt.set_playerid(GPlayer.playerId);
	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	//session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_LOBBY(PacketSessionRef& session, Protocol::S_ENTER_LOBBY& pkt)
{
#pragma region test
	//// ���� ��� �÷��̾ �غ�ƴٸ� C_ENTER_GAME ��Ŷ ����
	//if (pkt.isallplayersready())
	//{
	//	Protocol::C_ENTER_GAME enterGamePkt;
	//	enterGamePkt.set_playerid(GPlayer.playerId);
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//	session->Send(sendBuffer);
	//}
	//// �غ� �ȵƴٸ� �����
	//else
	//{
	//	string sChat;
	//	// ä���ϰ������ ��Ʈ�� Ű ������ (�ϴ� �ӽ�)
	//	cout << "ä���ϰ������ LControlŰ �������� / �����ϰ� ������ �����̽��� �������� " << endl;
	//	if (CKeyManager::Get_Instance()->Key_Down(VK_LCONTROL))
	//	{
	//		cin >> sChat;
	//		Protocol::C_CHAT chatPkt;
	//		chatPkt.set_msg(sChat);
	//		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	//		session->Send(sendBuffer);
	//	}
	//	if (CKeyManager::Get_Instance()->Key_Down(VK_SPACE))
	//	{
	//		Protocol::C_ENTER_LOBBY enterLobbyPkt;
	//		enterLobbyPkt.set_playerid(GPlayer.playerId);
	//		enterLobbyPkt.set_isplayerready(true);

	//		cout << GPlayer.playerId << "�� �÷��̾� �غ� �Ϸ�!" << endl;
	//		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	//		session->Send(sendBuffer);
	//	}
	//}
#pragma endregion test
	//// ���� ��� �÷��̾ �غ�ƴٸ� C_ENTER_GAME ��Ŷ ����
	//if (pkt.isallplayersready())
	//{
	//	Protocol::C_ENTER_GAME enterGamePkt;
	//	enterGamePkt.set_playerid(GPlayer.playerId);
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//	session->Send(sendBuffer);
	//}
	//// �غ� �ȵƴٸ� �����
	//else
	//{
	//	Protocol::C_ENTER_LOBBY LobbyPkt;
	//	
	//	if (!GPlayer.bisPlayerReady)
	//	{
	//		// �ϴ��� ready�� �Է��ϸ� �غ�ƴٱ�..���� ���ӿ��� ui�� Ŭ���ؼ� ready�ǵ��� �����Ұ���.
	//		string chat;
	//		cin >> chat;
	//		if (chat == "ready")
	//		{
	//			LobbyPkt.set_playerid(GPlayer.playerId);
	//			LobbyPkt.set_isplayerready(true);
	//			GPlayer.bisPlayerReady = true;
	//			cout << "ID " << GPlayer.playerId << "�غ�Ϸ�" << endl;
	//		}

	//		// ready �Ѱ� �ƴ϶�� ä������ ������ ���� ���ӿ��� ä��â UI�� Ŭ���ϸ� �׶� cin�� �޾� ä�� ������ �Է¹���
	//		else
	//		{
	//			Protocol::C_CHAT chatPkt;
	//			chatPkt.set_playerid(GPlayer.playerId);
	//			chatPkt.set_msg(chat);
	//			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	//			session->Send(sendBuffer);
	//		}
	//	}
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(LobbyPkt);
	//	session->Send(sendBuffer);
	//}
	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (pkt.success() != true)
		return false;

	// TODO
	Protocol::C_ENTER_GAME enterGamePkt;
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	session->Send(sendBuffer);

	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	// ���� ������ �÷��̾ ã�´�
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Player")
		{
			_player = gameObject;
			break;
		}
	}

	if (INPUT->GetButton(KEY_TYPE::W))
	{
		Protocol::C_MOVE MovePkt;
		MovePkt.set_xpos(_player->GetX());
		MovePkt.set_ypos(_player->GetY());
		MovePkt.set_zpos(_player->GetZ());

		MovePkt.set_movedir(0);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(MovePkt);
		session->Send(sendBuffer);
	}
	if (INPUT->GetButton(KEY_TYPE::S))
	{
		Protocol::C_MOVE MovePkt;
		MovePkt.set_xpos(_player->GetX());
		MovePkt.set_ypos(_player->GetY());
		MovePkt.set_zpos(_player->GetZ());

		MovePkt.set_movedir(1);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(MovePkt);
		session->Send(sendBuffer);
	}
	if (INPUT->GetButton(KEY_TYPE::A))
	{
		Protocol::C_MOVE MovePkt;
		MovePkt.set_xpos(_player->GetX());
		MovePkt.set_ypos(_player->GetY());
		MovePkt.set_zpos(_player->GetZ());

		MovePkt.set_movedir(2);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(MovePkt);
		session->Send(sendBuffer);
	}
	if (INPUT->GetButton(KEY_TYPE::D))
	{
		Protocol::C_MOVE MovePkt;
		MovePkt.set_xpos(_player->GetX());
		MovePkt.set_ypos(_player->GetY());
		MovePkt.set_zpos(_player->GetZ());

		MovePkt.set_movedir(3);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(MovePkt);
		session->Send(sendBuffer);
	}


	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	//uint64 playerId = pkt.playerid();

	//std::cout << "ID: " << playerId << ") " << pkt.msg() << endl;

	////std::cout << pkt.msg() << endl;
	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	Protocol::C_MOVE MovePkt;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Player")
		{
			_player = gameObject;
			break;
		}
	}
	// ���� �浹�ؼ� ��ġ�� �ٲ�ٸ� ��ġ �ٽ� �ٲ��ش�.

	Vec3 pos = _player->GetTransform()->GetLocalPosition();

	// ���⼭ �������� ���� ��ġ�� player��ġ�� �ٲ��ִ� �ڵ尡 ��
	// ���� �浹�� ���¶�� success�� false�� ������?
	if (pkt.success() == true)
	{
		if (pkt.movedir() == 0)
		{
			pos += _player->GetTransform()->GetLook() * 5.f * DELTA_TIME;
			_player->GetTransform()->SetLocalPosition(pos);
		}
		if (pkt.movedir() == 1)
		{
			pos -= _player->GetTransform()->GetLook() * 5.f * DELTA_TIME;
			_player->GetTransform()->SetLocalPosition(pos);
		}
		if (pkt.movedir() == 2)
		{
			pos -= _player->GetTransform()->GetRight() * 5.f * DELTA_TIME;
			_player->GetTransform()->SetLocalPosition(pos);
		}
		if (pkt.movedir() == 3)
		{
			pos += _player->GetTransform()->GetRight() * 5.f * DELTA_TIME;
			_player->GetTransform()->SetLocalPosition(pos);
		}
	}
	else
	{
		// ������ �浹, ��,��,�� �̵�����
		if (pkt.movedir() == 0)
		{
			if (INPUT->GetButton(KEY_TYPE::S))
			{
				pos -= _player->GetTransform()->GetLook() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
			if (INPUT->GetButton(KEY_TYPE::A))
			{
				pos -= _player->GetTransform()->GetRight() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
			if (INPUT->GetButton(KEY_TYPE::D))
			{
				pos += _player->GetTransform()->GetRight() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
		}
		// �ڷ� �浹, ��,��,�� �̵�����
		if (pkt.movedir() == 1)
		{
			if (INPUT->GetButton(KEY_TYPE::W))
			{
				pos += _player->GetTransform()->GetLook() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
			if (INPUT->GetButton(KEY_TYPE::A))
			{
				pos -= _player->GetTransform()->GetRight() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
			if (INPUT->GetButton(KEY_TYPE::D))
			{
				pos += _player->GetTransform()->GetRight() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
		}
		// �������� �浹, ��,��,�� �̵�����
		if (pkt.movedir() == 2)
		{
			if (INPUT->GetButton(KEY_TYPE::W))
			{
				pos += _player->GetTransform()->GetLook() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
			if (INPUT->GetButton(KEY_TYPE::S))
			{
				pos -= _player->GetTransform()->GetLook() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
			if (INPUT->GetButton(KEY_TYPE::D))
			{
				pos += _player->GetTransform()->GetRight() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
		}
		// ���������� �浹, ��,��,�� �̵�����
		if (pkt.movedir() == 3)
		{
			if (INPUT->GetButton(KEY_TYPE::W))
			{
				pos += _player->GetTransform()->GetLook() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
			if (INPUT->GetButton(KEY_TYPE::S))
			{
				pos -= _player->GetTransform()->GetLook() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
			if (INPUT->GetButton(KEY_TYPE::A))
			{
				pos -= _player->GetTransform()->GetRight() * 5.f * DELTA_TIME;
				_player->GetTransform()->SetLocalPosition(pos);
			}
		}
	}

	/*_player->GetTransform()->SetLocalPosition(pos);*/

	if (INPUT->GetButton(KEY_TYPE::W))
	{
		MovePkt.set_xpos(_player->GetX());
		MovePkt.set_ypos(_player->GetY());
		MovePkt.set_zpos(_player->GetZ());

		MovePkt.set_movedir(0);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(MovePkt);
		session->Send(sendBuffer);
	}
	if (INPUT->GetButton(KEY_TYPE::S))
	{
		MovePkt.set_xpos(_player->GetX());
		MovePkt.set_ypos(_player->GetY());
		MovePkt.set_zpos(_player->GetZ());

		MovePkt.set_movedir(1);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(MovePkt);
		session->Send(sendBuffer);
	}
	if (INPUT->GetButton(KEY_TYPE::A))
	{
		MovePkt.set_xpos(_player->GetX());
		MovePkt.set_ypos(_player->GetY());
		MovePkt.set_zpos(_player->GetZ());

		MovePkt.set_movedir(2);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(MovePkt);
		session->Send(sendBuffer);
	}
	if (INPUT->GetButton(KEY_TYPE::D))
	{
		MovePkt.set_xpos(_player->GetX());
		MovePkt.set_ypos(_player->GetY());
		MovePkt.set_zpos(_player->GetZ());

		MovePkt.set_movedir(3);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(MovePkt);
		session->Send(sendBuffer);
	}

	return true;
}
