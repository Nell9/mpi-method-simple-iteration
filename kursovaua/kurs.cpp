#undef UNICODE
#include <cmath>
#include <Windows.h>
#define _USE_MATH_DEFINES 
# define M_PI 3.14159265358979323846





void mult(float matr[][100][4], float A[][4]) {
	float*** D = new float** [18];
	for (int i = 0; i < 18; i++) {
		D[i] = new float* [100];
		for (int j = 0; j < 100; j++) {
			D[i][j] = new float[4];
		}
	}
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 100; j++) {
			for (int k = 0; k < 4; k++) {
				D[i][j][k] = 0;
				for (int k1 = 0; k1 < 4; k1++) {
					D[i][j][k] += matr[i][j][k1] * A[k1][k];
				}
			}
		}
		for (int j = 0; j < 100; j++) {
			for (int k = 0; k < 4; k++) {
				matr[i][j][k] = D[i][j][k];
			}
		}
	}
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 100; j++) {
			delete[] D[i][j];
		}
		delete[] D[i];
	}
	delete[] D;
}

void move(float matr[][100][4], int dx, int dy, int dz) {
	float m[4][4] = { 1,  0,  0,  0,
	  0,  1,  0,  0,
	  0,  0,  1,  0,
	  dx, dy, dz, 1 };
	mult(matr, m);
}

void scale(float matr[][100][4], float Sx, float Sy, float Sz, float S) {
	float m[4][4] = { Sx, 0,  0,  0,
	  0,  Sy, 0,  0,
	  0,  0,  Sz, 0,
	  0,  0,  0,  S };
	mult(matr, m);
}

void rotateX(float matr[][100][4], float ang, float x, float y, float z) {
	move(matr, -x, -y, -z);
	float m[4][4] = { 1, 0, 0, 0,
	  0, cos(ang), sin(ang), 0,
	  0, -sin(ang), cos(ang), 0,
	  0, 0, 0, 1 };
	mult(matr, m);
	move(matr, x, y, z);
}

void rotateY(float matr[][100][4], float ang, float x, float y, float z) {
	move(matr, -x, -y, -z);
	float m[4][4] = { cos(ang), 0,  -sin(ang),  0,
	  0,1,  0,0,
	  sin(ang), 0,  cos(ang),   0,
	  0,0,0,    1 };
	mult(matr, m);
	move(matr, x, y, z);
}

void rotateZ(float matr[][100][4], float ang, float x, float y, float z) {
	move(matr, -x, -y, -z);
	float m[4][4] = { cos(ang),  sin(ang),  0,  0,
	  -sin(ang), cos(ang),  0,0,
	  0, 0,1,  0,
	  0, 0,0,1 };
	mult(matr, m);
	move(matr, x, y, z);
}

float matr_proj[18][100][2];
void proj(float matr[][100][4]) {

	float d[4], m[4][4] = { 1, 0, 0, 0,
						  0, 1, 0, 0,
						  0, 0, 1, 0.001,
						  0, 0, 0, 1 };
	for (int k1 = 0; k1 < 18; k1++) {
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 4; j++) {
				d[j] = 0.0;
				for (int k = 0; k < 4; k++)
					d[j] += matr[k1][i][k] * m[k][j];
			}
			matr_proj[k1][i][0] = ((d[0] + d[2]) / d[3]) * 50;
			matr_proj[k1][i][1] = ((d[1] + d[2]) / d[3]) * 50;
		}
	}
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	static RECT rect;
	static HPEN pen[18];
	static float matr[18][100][4];
	static int ind = 0;
	static float x0;
	static float y0;
	static float z0;
	static const float K = 3;
	switch (uMsg) {
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		x0 = rect.right / 2;
		y0 = rect.bottom / 2 + 100;
		z0 = rect.bottom / 2;
		for (int i = 0; i < 18; i++) {
			pen[i] = CreatePen(PS_SOLID, 1, RGB(0, (int)(255 - i * 14.166), (int)(i * 14.166)));
		}
		for (float i = 0, u = 0; i < 18; u += M_PI / 18, i++) {

			for (float v = 0; ind < 100; v += 2 * M_PI / 100, ind++) {

				matr[(int)i][ind][0] = K * sinf(u) * sinf(v);
				matr[(int)i][ind][1] = K * (log1p(tanf(u / 2) + cosf(u)));
				matr[(int)i][ind][2] = K * sinf(u) * cosf(v);
				matr[(int)i][ind][3] = 1;

			}

			ind = 0;
		}

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		proj(matr);

		for (int i = 0; i < 18; i++)
		{
			MoveToEx(hdc, x0 - matr_proj[i][1][0], y0 - matr_proj[i][1][1], NULL);
			for (int j = 1; j < 100; j++)
			{
				LineTo(hdc, x0 - matr_proj[i][j][0], y0 - matr_proj[i][j][1]);
			}

		}

		for (int i = 0; i < 18; i++) {
			SelectObject(hdc, pen[i]);
			MoveToEx(hdc, x0 - matr_proj[i][0][0], y0 - matr_proj[i][0][1],
				NULL);
			for (int j = 0; j < 100; j++) {
				LineTo(hdc, x0 - matr_proj[i][j][0], y0 - matr_proj[i][j][1]);
			}
			LineTo(hdc, x0 - matr_proj[i][0][0], y0 - matr_proj[i][0][1]);
		}
		for (int j = 0; j < 100; j++) {
			MoveToEx(hdc, x0 - matr_proj[0][j][0], y0 - matr_proj[0][j][1], NULL);

			for (int i = 0; i < 18; i++) {
				SelectObject(hdc, pen[i]);
				LineTo(hdc, x0 - matr_proj[i][j][0], y0 - matr_proj[i][j][1]);
			}
		}

		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		GetClientRect(hWnd, &rect);
		switch (wParam) {
		case VK_LEFT:
			move(matr, 1, 0, 0);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case VK_RIGHT:
			move(matr, -1, 0, 0);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case VK_UP:
			move(matr, 0, 1, 0);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case VK_DOWN:
			move(matr, 0, -1, 0);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 189:
			move(matr, 0, 0, -5);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 187:
			move(matr, 0, 0, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'A':
			scale(matr, 1.1, 1, 1, 1);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'D':
			scale(matr, 0.9, 1, 1, 1);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'W':
			scale(matr, 1, 1.1, 1, 1);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'S':
			scale(matr, 1, 0.9, 1, 1);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'Q':
			scale(matr, 1, 1, 1.1, 1);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'E':
			scale(matr, 1, 1, 0.9, 1);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case VK_TAB:
			scale(matr, 1, 1, 1, 0.9);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case VK_CAPITAL:
			scale(matr, 1, 1, 1, 1.1);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'Z':
			rotateX(matr, 0.15, matr[0][0][0], matr[0][0][1], matr[0][0][2]);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'X':
			rotateY(matr, 0.15, matr[0][0][0], matr[0][0][1], matr[0][0][2]);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case 'C':
			rotateZ(matr, 0.15, matr[0][0][0], matr[0][0][1], matr[0][0][2]);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "WinClass";
	wc.lpszMenuName = NULL;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Cannot register class", "ERROR", MB_OK);
		return 0;
	}
	hWnd = CreateWindowEx(NULL, "WinClass", "Курсовой проект", WS_OVERLAPPEDWINDOW |
		WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
		hInst, NULL);
	if (!hWnd) {
		MessageBox(NULL, "Cannot create window", "ERROR", MB_OK);
		return 0;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
