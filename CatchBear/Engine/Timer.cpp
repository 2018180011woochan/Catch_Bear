#include "pch.h"
#include "Timer.h"

void Timer::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU Ŭ��
}

void Timer::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;

	// 1�ʵ��� �� �� ����ƴ��� üũ
	_frameCount++;

	// ���� �ð� ���
	_frameTime += _deltaTime;

	// 1�ʰ� �Ѿ�� 0���� ���õ�
	if (_frameTime > 1.f)
	{
		// ��������� 1�ʵ��� ����� ���� = ��� FPS
		_fps = static_cast<uint32>(_frameCount / _frameTime);

		_frameTime = 0.f;
		_frameCount = 0;
	}
}