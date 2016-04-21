#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "COverlappedWindow.h"
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include <Commctrl.h>

void COverlappedWindow::OnCustomGame() {
	isCustomGameSettings = true;
	customGameHandle = ::CreateDialog(::GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG2),
		handle, COverlappedWindow::customGameProc);

	::ShowWindow(customGameHandle, SW_SHOW);
}

BOOL __stdcall COverlappedWindow::customGameProc(HWND customGameHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	COverlappedWindow* window = reinterpret_cast<COverlappedWindow*>(::GetWindowLongPtr(::GetParent(customGameHandle),
		GWLP_USERDATA));

	switch (message) {
	case WM_INITDIALOG:
	{
		window->OnCustomGameInit(customGameHandle);
		return 1;
	}
	case WM_HSCROLL:
	{
		window->OnCustomGameScroll(customGameHandle);
		return 1;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case IDCANCEL:
		{
			window->OnCustomGameCancel(customGameHandle);
			return 1;
		}
		case IDOK:
		{
			window->OnCustomGameOk(customGameHandle);
			return 1;
		}
		case IDC_CHECK1:
		{
			window->OnCustomGameCheck(customGameHandle);
			return 1;
		}

		}
	}
	}
	return 0;
}

void COverlappedWindow::OnCustomGameInit(HWND customGameHandle) {
	HWND widthSlider = ::GetDlgItem(customGameHandle, IDC_SLIDER1);
	HWND heightSlider = ::GetDlgItem(customGameHandle, IDC_SLIDER2);

	HWND checkBox = ::GetDlgItem(customGameHandle, IDC_CHECK1);

	::SendMessage(widthSlider, TBM_SETRANGE, 1, MAKELONG(minGameInfo.widthGridNumber, 
		maxGameInfo.widthGridNumber));
	::SendMessage(heightSlider, TBM_SETRANGE, 1, MAKELONG(minGameInfo.heightGridNumber,
		maxGameInfo.heightGridNumber));

	::SendMessage(checkBox, BM_SETCHECK, isViewPreviewChecked ? BST_CHECKED : BST_UNCHECKED, 0);

	::SendMessage(widthSlider, TBM_SETPOS, 1, applyedGameInfo.widthGridNumber);
	::SendMessage(heightSlider, TBM_SETPOS, 1, applyedGameInfo.heightGridNumber);
}

void COverlappedWindow::OnCustomGameOk(HWND customGameHandle) {
	isCustomGameSettings = false;
	applyedGameInfo = settingsGameInfo;
	::DestroyWindow(customGameHandle);
	isPause = false;
	isSettingsPreview = false;
	startNewGame();
	::InvalidateRect(handle, 0, 0);
}

void COverlappedWindow::OnCustomGameCancel(HWND customGameHandle) {
	isCustomGameSettings = false;
	settingsGameInfo = applyedGameInfo;
	::DestroyWindow(customGameHandle);
	isPause = false;
	isSettingsPreview = false;
	::InvalidateRect(handle, 0, 0);
}

void COverlappedWindow::OnCustomGameScroll(HWND customGameHandle) {
	settingsGameInfo.widthGridNumber = ::SendMessage(::GetDlgItem(customGameHandle, IDC_SLIDER1), TBM_GETPOS, 0, 0);
	settingsGameInfo.heightGridNumber = ::SendMessage(::GetDlgItem(customGameHandle, IDC_SLIDER2), TBM_GETPOS, 0, 0);

	::InvalidateRect(handle, 0, 0);
}

void COverlappedWindow::OnCustomGameCheck(HWND customGameHandle) {
	HWND checkBox = ::GetDlgItem(customGameHandle, IDC_CHECK1);
	isSettingsPreview = ::SendMessage(checkBox, BM_GETCHECK, 0, 0);
	::InvalidateRect(handle, 0, 0);
}
