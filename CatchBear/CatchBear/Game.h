#pragma once

class Game
{
public:
	void Init(const WindowInfo& info);
	void Update();
	void LoginSceneUpdate();

	void setString(const char* strText);
	const wstring& GetFontString();
	const bool isIPAddrEnter();

public:
	bool	_isEnd = false;
	bool	_isFirstEnter = true;
};

