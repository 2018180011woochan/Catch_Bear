#include "pch.h"
#include "StunState.h"
#include "Player.h"
#include "Timer.h"
#include "AnimationController.h"
#include "GameObject.h"
#include "IdleState.h"
#include "MoveState.h"
#include "JumpState.h"
#include "AttackState.h"
#include "DashState.h"
#include "DashRestState.h"
#include "SlowState.h"
#include "SlowRestState.h"
#include "StunState.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Resources.h"

PlayerState* StunState::KeyCheck(GameObject& player, STATE& ePlayer)
{
    return NULL;
}

PlayerState* StunState::Update(GameObject& player, STATE& ePlayer)
{
    // 3초 동안만 유지됨
    _fTime += DELTA_TIME;
    
    if (_fTime >= 3.f)
    {
        static_pointer_cast<Player>(player.GetScript(0))->SetCurItem(Player::ITEM::STUN, false);
        static_pointer_cast<Player>(player.GetScript(0))->SetPlayerStunned(false);

        ePlayer = _ePreState;

        switch (_ePreState)
        {
        case IDLE:
            return new IdleState;
            break;
        case WALK:
            return new MoveState;
            break;
        case DASH:
            return new DashState(_fTime);
            break;
        case JUMP:
            return new JumpState;
            break;
        case ATTACK:
            return new AttackState;
            break;
        case DASH_REST:
            return new DashRestState(_fTime);
            break;
        case WALK_DOWN:
            return new SlowState(_fTime);
            break;
        case SLOW_REST:
            return new SlowRestState(_fTime);
            break;
        default:
            return new IdleState;
            ePlayer = STATE::IDLE;
            break;
        }

        _fTime = 0.f;
    }

    return NULL;
}

void StunState::Enter(GameObject& player)
{
    _ePreState = player._curState;
    player.GetAnimationController()->SetTrackAnimationSet(0, 5);

    // 텍스처 매핑
    shared_ptr<Texture>	diffuseTex = GET_SINGLE(Resources)->Get<Texture>(L"StunState");
    player.GetMeshRenderer()->GetMaterial()->SetTexture(0, diffuseTex);
}

void StunState::End(GameObject& player)
{
    _fTime = 0.f;

    // 텍스처 매핑
    wstring key = static_pointer_cast<Player>(player.GetScript(0))->GetTextureKey();
    shared_ptr<Texture>	diffuseTex = GET_SINGLE(Resources)->Get<Texture>(key);
    player.GetMeshRenderer()->GetMaterial()->SetTexture(0, diffuseTex);
}
