#pragma once

#include <Windows.h>
#include "Game.h"

class COverlappedWindow {
public:
	COverlappedWindow();
	~COverlappedWindow();

	static bool RegisterClass(HINSTANCE instance);

	bool Create(HINSTANCE instance);

	void Show(int cmdShow);

	HACCEL haccel;
	HWND handle;
protected:
	void OnCreate(HWND handle);
	void OnSize(LPARAM lParam);
	void OnSetFocus();
	void OnDestroy();
	void OnCommand(WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE hInstance;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	bool OnClose();
	bool writeGameToFile();

	//game
	bool isGameStarted = true;
	Game game;

	//game drawing
	int lineStroke = 2;
	int lineStrokeMin = 2;
	int lineStrokeMax = 2;

	int lineDistance = 30;
	int lineDistanceMin = 30;
	int lineDistanceMax = 30;

	int pointRadius = 10;
	int pointRadiusMin = 10;
	int pointRadiusMax = 10;

	COLORREF lineColor = RGB(0, 0, 0);
	COLORREF firstPlayerColor = RGB(0, 255, 255);
	COLORREF secondPlayerColor = RGB(255, 255, 0);
	COLORREF backgroundColor = RGB(255, 255, 255);
};
