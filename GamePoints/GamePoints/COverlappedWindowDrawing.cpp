#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "COverlappedWindow.h"
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include "Game.h"
#include <string> 
#include <cstring>

void COverlappedWindow::OnPaint() {
	setWindowSize();

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
	}
	else {
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
	if (!isCustomGameSettings) {
		drawPoints(paintDC, rect);
		drawScoreboard(paintDC, rect);
		drawEdges(paintDC, rect);
	}
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

void writeText(HDC paintDC, int fontSize, int x, int y, const std::wstring& str) {
	HFONT font = CreateFont(fontSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
	::SelectObject(paintDC, font);
	
	::SetTextAlign(paintDC, TA_CENTER);
	::TextOut(paintDC, x, y, str.c_str(), str.size());
	
	::DeleteObject(font);
}

void COverlappedWindow::drawScoreboard(HDC paintDC, const RECT& rect) {

	int indent = getDrawInfo().lineIndent;
	int y = indent * (getGameInfo().heightGridNumber + 1) + getDrawInfo().scoreboardSize / 3;
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

	writeText(paintDC, getDrawInfo().scoreboardFont, x, y, str);
}

void COverlappedWindow::drawEdge(HDC paintDC, const RECT& rect, const Edge& e) {
	HBRUSH brush;
	HPEN pen;
	if (e.isFirstPlayer) {
		brush = ::CreateSolidBrush(getDrawInfo().firstPlayerColor);
		pen = ::CreatePen(PS_SOLID, getDrawInfo().lineStroke, getDrawInfo().firstPlayerColor);
	} else {
		brush = ::CreateSolidBrush(getDrawInfo().secondPlayerColor);
		pen = ::CreatePen(PS_SOLID, getDrawInfo().lineStroke, getDrawInfo().secondPlayerColor);
	}
	
	::SelectObject(paintDC, brush);
	::SelectObject(paintDC, pen);

	int indent = getDrawInfo().lineIndent;
	int x1 = (e.start.x + 1) * indent;
	int y1 = (e.start.y + 1)* indent;

	int x2 = (e.end.x + 1) * indent;
	int y2 = (e.end.y + 1) * indent;
	
	Line(paintDC, x1, y1, x2, y2);

	::DeleteObject(brush);
	::DeleteObject(pen);
}

void COverlappedWindow::drawEdges(HDC paintDC, const RECT& rect) {
	if (isGameStarted) {
		for (Edge e : game.getEdges()) {
			drawEdge(paintDC, rect, e);
		}
	}
}
