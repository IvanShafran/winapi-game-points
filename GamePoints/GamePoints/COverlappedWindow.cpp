#include "COverlappedWindow.h"
#include <Windows.h>
#include <iostream>
#include "resource.h"

COverlappedWindow::COverlappedWindow()
{

}

COverlappedWindow::~COverlappedWindow()
{

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
		case WM_SIZE:
			window->OnSize(lParam);
			break;
		case WM_SETFOCUS:
			window->OnSetFocus();
			break;
		case WM_COMMAND:
			window->OnCommand(wParam, lParam);
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

void COverlappedWindow::OnCreate(HWND handle) {
}

void COverlappedWindow::OnSize(LPARAM lParam) {
}

void COverlappedWindow::OnSetFocus() {
}

bool COverlappedWindow::OnClose() {
	if (!gameStarted) {
		return true;
	}

	int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)L"��������� ����?",
		(LPCWSTR)L"����� �� ����",
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
