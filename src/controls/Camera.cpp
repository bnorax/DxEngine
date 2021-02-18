#include <dxpch.h>
#include "Camera.h"
#include "render/InitBuff.h"

void DxEngine::EditorCamera::EditorCameraUpdate()
{
	POINT mCur;
	float a = 1;
	if (GetAsyncKeyState(VK_RBUTTON) < 0) {
		GetCursorPos(&mCur);
		ScreenToClient(*currentWindowHandler, &mCur);
		if (&mPrev || &mCur != NULL) {
			if (mCur.y - mPrev.y > 1) {
				g_View *= XMMatrixRotationX(-0.01f);
			}
			else if (mCur.y - mPrev.y < -1) {
				g_View *= XMMatrixRotationX(0.01f);
			}
			if (mCur.x - mPrev.x > 1) {
				g_View *= XMMatrixRotationY(-0.02f);
			}
			else if (mCur.x - mPrev.x < -1) {
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
	/*if (GetAsyncKeyState(0x27) < 0) {
		g_directionLight.x -= 0.05f;
	}
	if (GetAsyncKeyState(0x25) < 0) {
		g_directionLight.x += 0.05f;
	}
	if (GetAsyncKeyState(0x26) < 0) {
		g_directionLight.y -= 0.05f;
	}
	if (GetAsyncKeyState(0x28) < 0) {
		g_directionLight.y += 0.05f;
	}*/
	GetCursorPos(&mPrev);
	ScreenToClient(*currentWindowHandler, &mPrev);
}
