#pragma once

#include "COverlappedWindow.h"
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include <Commctrl.h>
#include <string>
#include <cstring>


bool getFileName(HWND handle, LPWSTR fileName) {
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrDefExt = L"omg";

	return GetSaveFileName(&ofn);
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
