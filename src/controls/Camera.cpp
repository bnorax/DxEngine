#include <dxpch.h>
#include "Camera.h"
#include "render/InitBuff.h"

void DxEngine::EditorCamera::EditorCameraUpdate()
{
	float a = 1;
	if (GetAsyncKeyState(VK_RBUTTON) < 0) {
		GetCursorPos(&curPoint);
		ScreenToClient(currentWindowHandler, &curPoint);
		if (&curPoint || &prevPoint != NULL) {
			if (curPoint.y - prevPoint.y > 1) {
				g_View *= XMMatrixRotationX(-0.01f);
			}
			else if (curPoint.y - prevPoint.y < -1) {
				g_View *= XMMatrixRotationX(0.01f);
			}
			if (curPoint.x - prevPoint.x > 1) {
				g_View *= XMMatrixRotationY(-0.02f);
			}
			else if (curPoint.x - prevPoint.x < -1) {
				g_View *= XMMatrixRotationY(0.02f);
			}
		}
	}
	if (GetAsyncKeyState(VK_SHIFT) < 0) {
		a = 10;
	}
	if (GetAsyncKeyState(0x57) < 0) {
		g_View *= XMMatrixTranslation(0, 0, -0.01f*a);
	}
	if (GetAsyncKeyState(0x53) < 0) {
		g_View *= XMMatrixTranslation(0, 0, 0.01f*a);
	}
	if (GetAsyncKeyState(0x44) < 0) {
		g_View *= XMMatrixTranslation(-0.01f*a, 0, 0);
	}
	if (GetAsyncKeyState(0x41) < 0) {
		g_View *= XMMatrixTranslation(0.01f*a, 0, 0);
	}
	GetCursorPos(&prevPoint);
	ScreenToClient(currentWindowHandler, &prevPoint);
}
