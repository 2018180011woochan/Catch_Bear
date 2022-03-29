#include "pch.h"
#include "InstancingManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

// Render()�� ȣ���ϴ� ����, ī�޶󿡼� �з��� �� ������Ʈ�� ������ ���鼭 �ϳ��� �׷��ִ°� �ƴ϶�
// InstancingManager���� �������ش޶�� ��� ��ü���� �� ���Ѱ��ְ� �ȴ�.
// �׷��� �� ��ü����(gameObjects) �з��� �ؾ� �Ѵ�.
// GetInstanceID() �Լ��� ���� �갡 �׷���� �ϴ� �ν��Ͻ��� ���°���� �޾ƿ� ���̰�
// �װͿ� ���� ������Ʈ�� �з��ؼ� �־��ش�.

void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects)
{
	// ���� instanceID(uint64)�� �ش��ϴ� ��ü�鳢�� ����ֱ� ���� ���� ����, ĳ���ϴ� �뵵
	// ���⿡ ��ü���� �ϳ��� �� ��ġ���ش�.
	// ��ü���� �� �ϳ��� �׷��� �ϴ� ��쵵 ���� ���̰�,
	// �ν��Ͻ� ���̵� ���ļ� ��ü�� �ν��Ͻ��� ���ļ� �׷���� �ϴ� ���� �з��ȴ�.
	map<uint64, vector<shared_ptr<GameObject>>> cache;
	
	for (shared_ptr<GameObject>& gameObject : gameObjects)
	{
		const uint64 instanceId = gameObject->GetMeshRenderer()->GetInstanceID();
		cache[instanceId].push_back(gameObject);
	}

	// ��� ��ü�� �� �з������� ���ݱ��� ����� �ʿ� �ִ� ��� ������ ���鼭
	for (auto& pair : cache)
	{
		const vector<shared_ptr<GameObject>>& vec = pair.second;

		// ���� ��ü�� �ϳ��ۿ� ���ٸ� ������ ���������� �Ϲ����� Render() Ȱ��
		if (vec.size() == 1)
		{
			vec[0]->GetMeshRenderer()->Render();
		}

		// �ν��Ͻ��� ������ �Ǿ�� �Ѵٸ�
		// ��� ��ü���� �� �ѹ��� �׷��ֱ� ���ؼ� ��ü���� ����ְ� �ִ�.
		else
		{
			const uint64 instanceId = pair.first;

			for (const shared_ptr<GameObject>& gameObject : vec)
			{
				// �ν��Ͻ� �Ķ���� ������ ����, �����, ����� ���������� ���� �ǽð����� ������ְ� �ְ�
				// AddParam()�� ���� �� �Լ����ٰ� �ش� �ν��Ͻ� ���̵� �ش��ϴ� �Ķ���� ������ �߰��ش޶�� �־��ְ� �ִ�.
				// AddParam(): ���� ���ϴ� �ν��Ͻ� ���̵� �ش��ϴ� ���۰� �ִ��� ã�� ������
				// ������ ������ش�. ���� ���� ���⿡ AddData()�� ���ؼ� ���� ������ �߰����ش�.
				InstancingParams params;
				params.matWorld = gameObject->GetTransform()->GetLocalToWorldMatrix();
				params.matWV = params.matWorld * Camera::S_MatView;
				params.matWVP = params.matWorld * Camera::S_MatView * Camera::S_MatProjection;

				AddParam(instanceId, params);
			}

			// �ν��Ͻ� ���ۿ� ��� �������� ä��������
			// ����� �ѹ濡 �׸��� �ϱ� ���� InstancingBuffer�� �Ѱ��༭ �������ϴ�
			// �ι�° ������ Render()�� ȣ�����ش�.
			// InstancingBuffer�� �޾Ƽ� �ѹ��� �׷��ִ� ����
			shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
			vec[0]->GetMeshRenderer()->Render(buffer);
		}
	}
}

void InstancingManager::ClearBuffer()
{
	for (auto& pair : _buffers)
	{
		shared_ptr<InstancingBuffer>& buffer = pair.second;
		buffer->Clear();
	}
}

void InstancingManager::AddParam(uint64 instanceId, InstancingParams& data)
{
	if (_buffers.find(instanceId) == _buffers.end())
		_buffers[instanceId] = make_shared<InstancingBuffer>();

	_buffers[instanceId]->AddData(data);
}