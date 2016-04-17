#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "COverlappedWindow.h"
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include "Game.h"

COverlappedWindow::COverlappedWindow()
{
}

COverlappedWindow::~COverlappedWindow()
{

}

bool COverlappedWindow::RegisterClass(HINSTANCE instance)
{
	WNDCLASSEX windowClass;

	::ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = COverlappedWindow::windowProc;
	windowClass.hIcon;
	windowClass.hInstance = instance;
	windowClass.lpszClassName = L"COverlappedWindow";
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszMenuName = L"MainMenu";

	return ::RegisterClassEx(&windowClass) != 0;
}

bool COverlappedWindow::Create(HINSTANCE instance)
{
	hInstance = instance;

	COverlappedWindow::handle = ::CreateWindowEx(0,
		L"COverlappedWindow",
		L"GamePoints",
		WS_OVERLAPPEDWINDOW&~WS_THICKFRAME&~WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		instance,
		this);

	return COverlappedWindow::handle != 0;
}

LRESULT __stdcall COverlappedWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_ERASEBKGND) {
		return true;
	}

	if (message == WM_NCCREATE) {
		COverlappedWindow* window = reinterpret_cast<COverlappedWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		::SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG>(window));
	}
	else {
		COverlappedWindow* window = reinterpret_cast<COverlappedWindow*>(::GetWindowLongPtr(handle, GWLP_USERDATA));
		switch (message) {
		case WM_CREATE:
			window->OnCreate(handle);
			break;
		case WM_PAINT:
			window->OnPaint();
			break;
		case WM_COMMAND:
			window->OnCommand(wParam, lParam);
			break;
		case WM_LBUTTONDOWN:
			window->OnLButtonDown(wParam, lParam);
			break;
		case WM_CLOSE:
			if (!window->OnClose()) {
				return 0;
			}
			break;
		case WM_DESTROY:
			window->OnDestroy();
			break;
		default:
			break;
		}
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

void COverlappedWindow::setWindowSize() {
	int indent = getDrawInfo().lineIndent;
	int unexpectedXIndent = 20;
	int unexpectedYIndent = 65;
	int widthNumber = getGameInfo().widthGridNumber;
	int heightNumber = getGameInfo().heightGridNumber;
	int scoreboardSize = getDrawInfo().scoreboardSize;

	RECT rect;
	GetWindowRect(handle, &rect);
	int x = rect.left;
	int y = rect.top;

	::SetWindowPos(handle, 0, x, y,
		(widthNumber + 1) * indent + unexpectedXIndent,
		(heightNumber + 1) * indent + unexpectedYIndent + scoreboardSize, SWP_SHOWWINDOW);

}

void COverlappedWindow::startNewGame() {
	setWindowSize();

	game = Game(getGameInfo().widthGridNumber, getGameInfo().heightGridNumber);
	isGameStarted = true;
	isDoneFirstStep = false;

	::InvalidateRect(handle, 0, 0);
}

void COverlappedWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam) {
	if (!isPause) {
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(handle, &pos);

		int indent = getDrawInfo().lineIndent;

		int x_num = abs(pos.x - indent / 2) / indent;
		int y_num = abs(pos.y - indent / 2) / indent;

		if (game.isInField(x_num, y_num) && game.markPoint(x_num, y_num, getGameInfo().isFirstNextStep)) {
			getGameInfo().isFirstNextStep = !getGameInfo().isFirstNextStep;
			isDoneFirstStep = true;
			game.buildGame();
		}

		::InvalidateRect(handle, 0, 0);
	}
}

void COverlappedWindow::OnCreate(HWND handle) {
	this->handle = handle;

	applyedDrawInfo.lineStroke = 2;
	applyedDrawInfo.lineIndent = 40;
	applyedDrawInfo.pointRadius = 8;
	settingsDrawInfo = applyedDrawInfo;

	minSettings.lineStroke = 0;
	minSettings.lineIndent = 10;
	minSettings.pointRadius = 2;

	maxSettings.lineStroke = 5;
	maxSettings.lineIndent = 100;
	maxSettings.pointRadius = 10;

	minGameInfo.heightGridNumber = 4;
	minGameInfo.widthGridNumber = 4;

	maxGameInfo.heightGridNumber = 50;
	maxGameInfo.widthGridNumber = 50;

	startNewGame();
}

bool COverlappedWindow::OnClose() {
	if (!isDoneFirstStep) {
		return true;
	}

	int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)L"Сохранить игру?",
		(LPCWSTR)L"Выход из игры",
		MB_ICONWARNING | MB_YESNOCANCEL | MB_DEFBUTTON3
		);

	bool exit;
	switch (msgboxID)
	{
	case IDYES:
	{
		exit = true;
		break;
	}
	case IDNO:
		exit = true;
		break;
	case IDCANCEL:
		exit = false;
		break;
	}

	return exit;
}

void COverlappedWindow::OnCommand(WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
	case ID_EXIT:
		::PostMessage(handle, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		break;
	}
}

void COverlappedWindow::OnDestroy()
{
	::PostQuitMessage(0);
}

void COverlappedWindow::Show(int cmdShow)
{
	::ShowWindow(handle, cmdShow);
}
