#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "COverlappedWindow.h"
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include <Commctrl.h>

void COverlappedWindow::OnViewSettings() {
    viewSettingsHandle = ::CreateDialog(::GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1),
		handle, COverlappedWindow::settingsViewProc);

	::ShowWindow(viewSettingsHandle, SW_SHOW);
}

BOOL __stdcall COverlappedWindow::settingsViewProc(HWND settingsHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	COverlappedWindow* window = reinterpret_cast<COverlappedWindow*>(::GetWindowLongPtr(::GetParent(settingsHandle),
		GWLP_USERDATA));

	switch (message) {
	case WM_INITDIALOG:
	{
		window->OnViewSettingsInit(settingsHandle);
		return 1;
	}
	case WM_HSCROLL:
	{
		window->OnViewSettingsScroll(settingsHandle);
		return 1;
	}

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case IDCANCEL:
		{
			window->OnViewSettingsCancel(settingsHandle);
			return 1;
		}
		case IDOK:
		{
			window->OnViewSettingsOk(settingsHandle);
			return 1;
		}
		case IDC_BUTTON_FIRST_PLAYER_COLOR:
		{
			window->OnViewSettingsColor(settingsHandle, IDC_BUTTON_FIRST_PLAYER_COLOR);
			return 1;
		}
		case IDC_BUTTON_SECOND_PLAYER_COLOR:
		{
			window->OnViewSettingsColor(settingsHandle, IDC_BUTTON_SECOND_PLAYER_COLOR);
			return 1;
		}
		case IDC_BUTTON_GRID_COLOR:
		{
			window->OnViewSettingsColor(settingsHandle, IDC_BUTTON_GRID_COLOR);
			return 1;
		}
		case IDC_BUTTON_BACK_COLOR:
		{
			window->OnViewSettingsColor(settingsHandle, IDC_BUTTON_BACK_COLOR);
			return 1;
		}
		case IDC_BUTTON_DEAD_FIRST:
		{
			window->OnViewSettingsColor(settingsHandle, IDC_BUTTON_DEAD_FIRST);
			return 1;
		}
		case IDC_BUTTON_DEAD_SECOND:
		{
			window->OnViewSettingsColor(settingsHandle, IDC_BUTTON_DEAD_SECOND);
			return 1;
		}
		case IDC_CHECK1:
		{
			window->OnViewSettingsCheck(settingsHandle);
			return 1;
		}

		}
	}
	}
	return 0;
}

void COverlappedWindow::OnViewSettingsInit(HWND settingsHandle) {
	HWND lineStrokeSlider = ::GetDlgItem(settingsHandle, IDC_SLIDER2);
	HWND indentSlider = ::GetDlgItem(settingsHandle, IDC_SLIDER1);
	HWND radiusSlider = ::GetDlgItem(settingsHandle, IDC_SLIDER3);

	HWND checkBox = ::GetDlgItem(settingsHandle, IDC_CHECK1);

	::SendMessage(indentSlider, TBM_SETRANGE, 1, MAKELONG(minSettings.lineIndent, maxSettings.lineIndent));
	::SendMessage(lineStrokeSlider, TBM_SETRANGE, 1, MAKELONG(minSettings.lineStroke, maxSettings.lineStroke));
	::SendMessage(radiusSlider, TBM_SETRANGE, 1, MAKELONG(minSettings.pointRadius, maxSettings.pointRadius));

	::SendMessage(checkBox, BM_SETCHECK, isViewPreviewChecked ? BST_CHECKED : BST_UNCHECKED, 0);

	::SendMessage(indentSlider, TBM_SETPOS, 1, applyedDrawInfo.lineIndent);
	::SendMessage(lineStrokeSlider, TBM_SETPOS, 1, applyedDrawInfo.lineStroke);
	::SendMessage(radiusSlider, TBM_SETPOS, 1, applyedDrawInfo.pointRadius);
}

void COverlappedWindow::OnViewSettingsOk(HWND settingsHandle) {
	applyedDrawInfo = settingsDrawInfo;
	::DestroyWindow(settingsHandle);
	isPause = false;
	::InvalidateRect(handle, 0, 0);
}

void COverlappedWindow::OnViewSettingsCancel(HWND settingsHandle) {
	settingsDrawInfo = applyedDrawInfo;
	::DestroyWindow(settingsHandle);
	isPause = false;
	::InvalidateRect(handle, 0, 0);
}

void COverlappedWindow::OnViewSettingsColor(HWND settingsHandle, UINT buttonID) {
	CHOOSECOLOR cc;
	static COLORREF acrCustClr[16];

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = settingsHandle;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (::ChooseColor(&cc) == TRUE)
	{
		switch (buttonID) {
		case IDC_BUTTON_FIRST_PLAYER_COLOR:
		{
			settingsDrawInfo.firstPlayerColor = cc.rgbResult;
			break;
		}
		case IDC_BUTTON_SECOND_PLAYER_COLOR:
		{
			settingsDrawInfo.secondPlayerColor = cc.rgbResult;
			break;
		}
		case IDC_BUTTON_GRID_COLOR:
		{
			settingsDrawInfo.lineColor = cc.rgbResult;
			break;
		}
		case IDC_BUTTON_BACK_COLOR:
		{
			settingsDrawInfo.backgroundColor = cc.rgbResult;
			break;
		}
		case IDC_BUTTON_DEAD_FIRST:
		{
			settingsDrawInfo.innerFirstColor = cc.rgbResult;
			break;
		}
		case IDC_BUTTON_DEAD_SECOND:
		{
			settingsDrawInfo.innerSecondColor = cc.rgbResult;
			break;
		}
		}
	}

	::InvalidateRect(handle, 0, 0);
}

void COverlappedWindow::OnViewSettingsScroll(HWND settingsHandle) {
	settingsDrawInfo.lineStroke = ::SendMessage(::GetDlgItem(settingsHandle, IDC_SLIDER2), TBM_GETPOS, 0, 0);
	settingsDrawInfo.lineIndent = ::SendMessage(::GetDlgItem(settingsHandle, IDC_SLIDER1), TBM_GETPOS, 0, 0);
	settingsDrawInfo.pointRadius = ::SendMessage(::GetDlgItem(settingsHandle, IDC_SLIDER3), TBM_GETPOS, 0, 0);

	::InvalidateRect(handle, 0, 0);
}

void COverlappedWindow::OnViewSettingsCheck(HWND settingsHandle) {
	HWND checkBox = ::GetDlgItem(settingsHandle, IDC_CHECK1);
	isSettingsPreview = ::SendMessage(checkBox, BM_GETCHECK, 0, 0);
	::InvalidateRect(handle, 0, 0);
}