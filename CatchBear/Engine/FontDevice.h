#pragma once
#include "GameObject.h"
struct TextBlock
{
	wstring		wstrText;
	D2D1_RECT_F	d2dLayoutRect;
	IDWriteTextFormat* pdwFormat;
	Vec2	position;
};

class FontDevice : public GameObject
{
public:
	FontDevice(UINT nFrame);

public:
	void Initialize(ComPtr<ID3D12Device> pd3dDevice, ComPtr<ID3D12CommandQueue> pd3dCommandQueue);
	void Resize(UINT nWidth, UINT nHeight);

public:
	void UpdateFont(const wstring& wstrText);
	void Server_UpdateFont(const wstring& wstrText);
	void PushFont(const wstring& wstrText);
	void SendChatPacket(const wstring& wstrText);	// Server
	const wstring& GetFontString() { return _vTextBlocks[0].wstrText; }

	void Render(UINT nFrame);
	void InitFont();

private:
	UINT GetRenderTargetsCount() { return static_cast<UINT>(_vWrappedRenderTargets.size()); }
		
private:
	float	_fWidth;
	float	_fHeight;

	ID3D11DeviceContext*	_pd3d11DeviceContext = NULL;
	ID3D11On12Device*		_pd3d11On12Device = NULL;
	IDWriteFactory*			_pd2dWriteFactory = NULL;
	ID2D1Factory3*			_pd2dFactory = NULL;
	ID2D1Device2*			_pd2dDevice = NULL;
	ID2D1DeviceContext2*		_pd2dDeviceContext = NULL;
	ID2D1SolidColorBrush*	_pd2dTextBrush = NULL;
	IDWriteTextFormat*		_pdwTextFormat = NULL;

	vector<ID3D11Resource*>	_vWrappedRenderTargets;
	vector<ID2D1Bitmap1*>	_vd2dRenderTargets;
	deque<TextBlock>					_vTextBlocks;
	array< D2D1_RECT_F, 5>	_textPos;

	wstring		_writingStr = L"";
	D2D1_RECT_F	_writingStrPos;
};