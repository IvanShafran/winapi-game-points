#pragma once

#include "COverlappedWindow.h"
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include <Commctrl.h>
#include <string>
#include <cstring>
#include <sstream>


bool getFileName(HWND handle, LPWSTR fileName, bool isOpenDlg=false) {
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrDefExt = L"omg";

	if (isOpenDlg) {
		return ::GetOpenFileName(&ofn);
	} else {
		return ::GetSaveFileName(&ofn);
	}
}

bool writeToFile(LPWSTR fileName, int textLen, LPTSTR lString) {
	HANDLE hFile = ::CreateFile(
		fileName,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile) {
		return false;
	}

	DWORD writtenBytesNumber;
	if (!::WriteFile(hFile, lString, textLen * sizeof(wchar_t), &writtenBytesNumber, 0)) {
		return false;
	}

	::CloseHandle(hFile);
	return true;
}

std::wstring COverlappedWindow::getGameDescription(int* textLen) {
	std::string result;

	//Game results
	result += std::to_string(game.getFirstResult()) + ' ';
	result += std::to_string(game.getSecondResult()) + ' ';

	//Game Info
	result += std::to_string(applyedGameInfo.widthGridNumber) + ' ';
	result += std::to_string(applyedGameInfo.heightGridNumber) + ' ';
	result += std::to_string(applyedGameInfo.isFirstNextStep) + ' ';

	//Draw Info
	result += std::to_string(applyedDrawInfo.lineIndent) + ' ';
	result += std::to_string(applyedDrawInfo.lineStroke) + ' ';
	result += std::to_string(applyedDrawInfo.pointRadius) + ' ';
	result += std::to_string(applyedDrawInfo.scoreboardFont) + ' ';
	
	result += std::to_string(applyedDrawInfo.backgroundColor) + ' ';
	result += std::to_string(applyedDrawInfo.firstPlayerColor) + ' ';
	result += std::to_string(applyedDrawInfo.innerColor) + ' ';
	result += std::to_string(applyedDrawInfo.innerFirstColor) + ' ';
	result += std::to_string(applyedDrawInfo.innerSecondColor) + ' ';
	result += std::to_string(applyedDrawInfo.lineColor) + ' ';
	result += std::to_string(applyedDrawInfo.playerPenColor) + ' ';
	result += std::to_string(applyedDrawInfo.secondPlayerColor) + ' ';

	for (int x = 0; x < applyedGameInfo.widthGridNumber; ++x) {
		for (int y = 0; y < applyedGameInfo.heightGridNumber; ++y) {
			result += std::to_string(game.getPointState(x, y)) + ' ';
		}
	}

	*textLen = result.size();

	return getWC(result.c_str());
}

bool COverlappedWindow::saveGame() {
	//text
	int textLen = 0;
	std::wstring outputString = getGameDescription(&textLen);

	wchar_t fileName[MAX_PATH] = L"";
	if (!getFileName(handle, fileName))
	{
		return false;
	}

	if (!writeToFile(fileName, textLen, (LPTSTR)outputString.c_str())) {
		return false;
	}

	return true;
}

int readNext(std::stringstream& ss) {
	if (ss.eof()) {
		return -1;
	}

	int t;
	ss >> t;

	return t;
}

template<typename T>
bool setNext(std::stringstream& ss, T* next) {
	int n = readNext(ss);
	if (n == -1) {
		return false;
	}
	
	*next = (T)n;

	return true;
}

bool getDataFromStream(std::stringstream& ss, DrawInfo& di, GameInfo& gi,
	std::vector<std::vector<PointState>>& points, int& firstResult, int& secondResult) {

	bool isLoad = true;

	isLoad &= setNext(ss, &firstResult);
	isLoad &= setNext(ss, &secondResult);

	//Game Info
	isLoad &= setNext(ss, &gi.widthGridNumber);
	isLoad &= setNext(ss, &gi.heightGridNumber);
	isLoad &= setNext(ss, &gi.isFirstNextStep);

	//Draw Info
	isLoad &= setNext(ss, &di.lineIndent);
	isLoad &= setNext(ss, &di.lineStroke);
	isLoad &= setNext(ss, &di.pointRadius);
	isLoad &= setNext(ss, &di.scoreboardFont);
	
	isLoad &= setNext(ss, &di.backgroundColor);
	isLoad &= setNext(ss, &di.firstPlayerColor);
	isLoad &= setNext(ss, &di.innerColor);
	isLoad &= setNext(ss, &di.innerFirstColor);
	isLoad &= setNext(ss, &di.innerSecondColor);
	isLoad &= setNext(ss, &di.lineColor);
	isLoad &= setNext(ss, &di.playerPenColor);
	isLoad &= setNext(ss, &di.secondPlayerColor);

	if (!isLoad) {
		return false;
	}

	for (int x = 0; x < gi.widthGridNumber; ++x) {
		points.push_back(std::vector<PointState>());
		for (int y = 0; y < gi.heightGridNumber; ++y) {
			points[x].push_back(PointState());
			isLoad &= setNext(ss, &points[x][y]);
		}
	}

	return isLoad;
}

bool COverlappedWindow::readGame(const wchar_t* filename) {
	DrawInfo drawInfo = applyedDrawInfo;
	GameInfo gameInfo = applyedGameInfo;

	HANDLE hFile = ::CreateFile(
		(LPWSTR)filename,
		GENERIC_READ,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile) {
		return false;
	}

	std::stringstream stringStream;
	DWORD tryReadBytesNumber = 64 * sizeof(wchar_t);
	DWORD writtenBytesNumber = tryReadBytesNumber;
	wchar_t* str = new wchar_t[tryReadBytesNumber];
	while (writtenBytesNumber == tryReadBytesNumber) {
		::ReadFile(hFile, str, tryReadBytesNumber, &writtenBytesNumber, 0);
		for (int i = 0; i < writtenBytesNumber / sizeof(wchar_t); ++i) {
			stringStream << static_cast<char>(str[i]);
		}
	}

	::CloseHandle(hFile);

	std::vector< std::vector<PointState> > points;

	int firstResult, secondResult;
	if (!getDataFromStream(stringStream, drawInfo, gameInfo, points, firstResult, secondResult)) {
		return false;
	}

	applyedDrawInfo = drawInfo;
	applyedGameInfo = gameInfo;
	settingsDrawInfo = drawInfo;
	settingsGameInfo = gameInfo;

	startNewGame();
	applyedGameInfo.isFirstNextStep = gameInfo.isFirstNextStep;

	for (int x = 0; x < applyedGameInfo.widthGridNumber; ++x) {
		for (int y = 0; y < applyedGameInfo.heightGridNumber; ++y) {
			game.setPoint(x, y, points[x][y]);
		}
	}

	game.buildGame();
	game.setFirstResult(firstResult);
	game.setSecondResult(secondResult);

	return true;
}

bool COverlappedWindow::loadGame() {
	wchar_t fileName[MAX_PATH] = L"";
	if (!getFileName(handle, fileName, true))
	{
		return false;
	}

	return readGame(fileName);
}
