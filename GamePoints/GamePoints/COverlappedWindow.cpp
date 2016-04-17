#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "COverlappedWindow.h"
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include "Game.h"
#include <string> 
#include <cstring>

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
		}

		::InvalidateRect(handle, 0, 0);
	}
}

void COverlappedWindow::OnPaint() {
	PAINTSTRUCT paintStruct;
	HDC paintDC = ::BeginPaint(handle, &paintStruct);
	RECT rect;
	::GetClientRect(handle, &rect);

	COverlappedWindow::drawGame(paintDC, rect);

	::EndPaint(handle, &paintStruct);
}

DrawInfo& COverlappedWindow::getDrawInfo() {
	if (isSettingsPreview) {
		return settingsDrawInfo;
	} else {
		return applyedDrawInfo;
	}
}

GameInfo& COverlappedWindow::getGameInfo() {
	if (isSettingsPreview) {
		return settingsGameInfo;
	}
	else {
		return applyedGameInfo;
	}
}

void COverlappedWindow::drawGame(HDC paintDC, const RECT& rect) {
	drawBackground(paintDC, rect);
	drawGrid(paintDC, rect);
	drawPoints(paintDC, rect);
	drawScoreboard(paintDC, rect);
}

void COverlappedWindow::drawBackground(HDC paintDC, const RECT& rect) {
	HBRUSH brush = CreateSolidBrush(getDrawInfo().backgroundColor);
	::FillRect(paintDC, &rect, brush);
	::DeleteObject(brush);
}

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
	::MoveToEx(hdc, x1, y1, NULL);
	return ::LineTo(hdc, x2, y2);
}

void COverlappedWindow::drawGrid(HDC paintDC, const RECT& rect) {
	HBRUSH brush = ::CreateSolidBrush(getDrawInfo().lineColor);
	HPEN pen = ::CreatePen(PS_SOLID, getDrawInfo().lineStroke, getDrawInfo().lineColor);
	::SelectObject(paintDC, pen);
	::SelectObject(paintDC, brush);

	int indent = getDrawInfo().lineIndent;
	int widthNumber = getGameInfo().widthGridNumber;
	int heightNumber = getGameInfo().heightGridNumber;
	for (int i = 0; i < widthNumber; ++i) {
		Line(paintDC, (i + 1) * indent, 0, (i + 1) * indent, (heightNumber + 1) * indent);
	}

	for (int i = 0; i < heightNumber; ++i) {
		Line(paintDC, 0, (i + 1) * indent, (widthNumber + 1) * indent, (i + 1) * indent);
	}

	::DeleteObject(brush);
	::DeleteObject(pen);
}

void COverlappedWindow::drawPoint(HDC paintDC, const RECT& rect, int x_num, int y_num, PointState state) {
	HBRUSH brush;
	switch (state)
	{
	case FIRST_PLAYER:
		brush = ::CreateSolidBrush(getDrawInfo().firstPlayerColor);
		break;
	case SECOND_PLAYER:
		brush = ::CreateSolidBrush(getDrawInfo().secondPlayerColor);
		break;
	case INNER:
		brush = ::CreateSolidBrush(getDrawInfo().innerColor);
		break;
	case INNER_FIRST:
		brush = ::CreateSolidBrush(getDrawInfo().innerFirstColor);
		break;
	case INNER_SECOND:
		brush = ::CreateSolidBrush(getDrawInfo().innerSecondColor);
		break;
	default:
		brush = ::CreateSolidBrush(getDrawInfo().innerColor);
		break;
	}

	HPEN pen = ::CreatePen(PS_SOLID, getDrawInfo().lineStroke, getDrawInfo().playerPenColor);
	::SelectObject(paintDC, pen);
	::SelectObject(paintDC, brush);
	
	int indent = getDrawInfo().lineIndent;
	int radius = getDrawInfo().pointRadius;
	int x1 = (x_num + 1) * indent - radius; 
	int y1 = (y_num + 1) * indent - radius;
	int x2 = (x_num + 1) * indent + radius;
	int y2 = (y_num + 1) * indent + radius;

	::Ellipse(paintDC, x1, y1, x2, y2);

	::DeleteObject(brush);
	::DeleteObject(pen);
}

void COverlappedWindow::drawPoints(HDC paintDC, const RECT& rect) {
	if (isGameStarted) {
		for (int x = 0; x < getGameInfo().widthGridNumber; ++x) {
			for (int y = 0; y < getGameInfo().heightGridNumber; ++y) {
				if (game.getPointState(x, y) != EMPTY) {
					drawPoint(paintDC, rect, x, y, game.getPointState(x, y));
				}
			}
		}
	}
}

std::wstring getWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	std::wstring wc(cSize, L'#');
	mbstowcs(&wc[0], c, cSize);

	return wc;
}

void setFont(HDC paintDC, int fontSize) {
	HFONT font = CreateFont(fontSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
	::DeleteObject(::SelectObject(paintDC, font));
}

void COverlappedWindow::drawScoreboard(HDC paintDC, const RECT& rect) {
	setFont(paintDC, 30);

	int indent = getDrawInfo().lineIndent;
	int y = indent * (getGameInfo().heightGridNumber + 1) + getDrawInfo().scoreboardSize / 2;
	int x = indent * (getGameInfo().widthGridNumber + 1) / 2;

	int firstScore = 0;
	int secondScore = 0;
	if (isGameStarted) {
		firstScore = game.getFirstResult();
		secondScore = game.getSecondResult();
	}

	std::string string = "First player " + std::to_string(firstScore) +
		":" + std::to_string(secondScore) + " Second player";
	std::wstring str = getWC(string.c_str());
	
	::SetTextAlign(paintDC, TA_CENTER);
	::TextOut(paintDC, x, y, str.c_str(), str.size());
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
