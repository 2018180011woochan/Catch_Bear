#pragma once
#include "MonoBehaviour.h"

class Texture;

class Button : public MonoBehaviour
{
public:
	Button();
	virtual ~Button();

public:
	virtual void Update() override;
	virtual void LateUpdate() override;


public:
	void SetCollRect(RECT rect) { _rect = rect; _isCollCheck = true; }
	void SetCollTexture(shared_ptr<Texture> texture) { _collTexture = texture; }
	void SetOriginalTexture(shared_ptr<Texture> texture, const wstring& texKey) { _originalTexture = texture; _texKey = texKey; }



private:
	shared_ptr<GameObject>		_button = make_shared<GameObject>();

protected:
	RECT	_rect;
	POINT	_mousePos;

	shared_ptr<Texture>	_collTexture;
	shared_ptr<Texture>	_originalTexture;

	bool	_isCollCheck = false;
	wstring _texKey;
};

