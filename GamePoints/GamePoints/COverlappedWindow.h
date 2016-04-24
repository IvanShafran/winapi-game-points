#pragma once

#include <Windows.h>
#include "Game.h"

struct DrawInfo {
	int lineStroke;
	int lineIndent;
	int pointRadius;
	int scoreboardSize = 60;
	int scoreboardFont = 30;
	COLORREF lineColor = RGB(0, 0, 0);
	COLORREF firstPlayerColor = RGB(255, 0, 0);
	COLORREF secondPlayerColor = RGB(0, 0, 255);
	COLORREF playerPenColor = RGB(0, 0, 0);
	COLORREF innerFirstColor = RGB(127, 0, 0);
	COLORREF innerSecondColor = RGB(0, 0, 127);
	COLORREF innerColor = RGB(127, 127, 127);
	COLORREF backgroundColor = RGB(255, 255, 255);
};

struct GameInfo {
	int widthGridNumber = 12;
	int heightGridNumber = 12;
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
	HWND viewSettingsHandle;
	HWND customGameHandle;
protected:
	void OnCreate(HWND handle);
	void OnDestroy();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE hInstance;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	
	//dlg view
	static BOOL __stdcall settingsViewProc(HWND settingsHandle, UINT message, WPARAM wParam, LPARAM lParam);
	
	void OnViewSettings();
	void OnViewSettingsInit(HWND settingsHandle);
	void OnViewSettingsOk(HWND settingsHandle);
	void OnViewSettingsCancel(HWND settingsHandle);
	void OnViewSettingsColor(HWND settingsHandle, UINT buttonID);
	void OnViewSettingsScroll(HWND settingsHandle);
	void OnViewSettingsCheck(HWND settingsHandle);
	std::wstring getWC(const char *c);

	//custom game
	static BOOL __stdcall customGameProc(HWND customGameHandle, UINT message, WPARAM wParam, LPARAM lParam);

	void OnCustomGame();
	void OnCustomGameInit(HWND customGameHandle);
	void OnCustomGameOk(HWND customGameHandle);
	void OnCustomGameCancel(HWND customGameHandle);
	void OnCustomGameScroll(HWND customGameHandle);
	void OnCustomGameCheck(HWND customGameHandle);
	void COverlappedWindow::showCustomNumbers(HWND customGameHandle);

	//game
	bool OnClose();
	bool COverlappedWindow::OnSaveDlg();

	bool saveGame();
	bool loadGame();
	bool COverlappedWindow::readGame(const wchar_t* filename);
	std::wstring getGameDescription(int* textLen);

	void drawGame(HDC paintDC, const RECT& rect);
	void drawPoint(HDC paintDC, const RECT& rect, int x_num, int y_num, PointState state);
	void drawGrid(HDC paintDC, const RECT& rect);
	void drawBackground(HDC paintDC, const RECT& rect);
	void drawPoints(HDC paintDC, const RECT& rect);
	void drawScoreboard(HDC paintDC, const RECT& rect);
	void drawEdge(HDC paintDC, const RECT& rect, const Edge& e);
	void drawEdges(HDC paintDC, const RECT& rect);
	DrawInfo& getDrawInfo();
	GameInfo& getGameInfo();
	
	void startNewGame(int width, int height);
	void startNewGame();
	void setWindowSize();

	bool isCustomGameSettings = false;
	bool isGameStarted = false;
	bool isDoneFirstStep = false;
	bool isPause = false;
	bool isSettingsPreview = false;
	bool isViewPreviewChecked = false;
	Game game;
	DrawInfo applyedDrawInfo, settingsDrawInfo, minSettings, maxSettings;
	GameInfo applyedGameInfo, settingsGameInfo, minGameInfo, maxGameInfo;
};
