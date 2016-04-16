#pragma once

#include <Windows.h>
#include "Game.h"

struct DrawInfo {
	int lineStroke;
	int lineIndent;
	int pointRadius;
	COLORREF lineColor = RGB(0, 0, 0);
	COLORREF firstPlayerColor = RGB(255, 0, 0);
	COLORREF secondPlayerColor = RGB(0, 0, 255);
	COLORREF playerPenColor = RGB(0, 0, 0);
	COLORREF innerFirstColor = RGB(0, 0, 0);
	COLORREF innerSecondColor = RGB(0, 0, 0);
	COLORREF innerColor = RGB(0, 0, 0);
	COLORREF backgroundColor = RGB(255, 255, 255);
};

struct GameInfo {
	int widthGridNumber = 5;
	int heightGridNumber = 5;
	bool isFirstNextStep = true;
};

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
	void OnDestroy();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnPaint();
private:
	HINSTANCE hInstance;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	bool OnClose();
	bool writeGameToFile();

	void drawGame(HDC paintDC, const RECT& rect);
	void COverlappedWindow::drawPoint(HDC paintDC, const RECT& rect, int x_num, int y_num, PointState state);
	void COverlappedWindow::drawGrid(HDC paintDC, const RECT& rect);
	void COverlappedWindow::drawBackground(HDC paintDC, const RECT& rect);
	void COverlappedWindow::drawPoints(HDC paintDC, const RECT& rect);
	DrawInfo COverlappedWindow::getDrawInfo();
	GameInfo COverlappedWindow::getGameInfo();
	//game
	bool isGameStarted = false;
	bool isSettingsPreview = false;
	Game game;
	DrawInfo applyedDrawInfo, settingsDrawInfo, minSettings, maxSettings;
	GameInfo applyedGameInfo, settingsGameInfo, minGameInfo, maxGameInfo;
};
