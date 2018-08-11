#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gl\GL.h>
#include <gl\glu.h>
#include <mmsystem.h>
#include <math.h>
extern "C" {
#include "resource.h"
#pragma comment(lib, "winmm.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
	char buffer[8 * 60] = {};
	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	
	extern	bool	Solve();
	bool	Done_Effect = true;
	int		Effect_Time[81];
	HWND	PhWnd;
	HWND	VhWnd;
	HWND	IIhWnd;
	HWND	IJhWnd;
	HDC		PhDC;
	HGLRC	PhGLRC;
	int		texture;
	int		EFT = 0;
	bool	Rot[81] = { 0 };
	int		RotateYA[81] = { 0 };
	char	Face[81] = { 0 };
	int		Squar_Situation[4][81] = { 0 };
	extern int		Sudoku_Values[9][9] = { 0 };
	float	i = 0;
	float	t = 0;
	float	SC = 0, x1, x2;
	void SolveSudoku(HWND hWnd);
	void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
	void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
	void Draw();
	void	Draw_Squar(int a, int b, int c, int d);
	GLuint LoadTextureRAW(const char * filename, int wrap);

	INT_PTR CALLBACK WinProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	HINSTANCE hInst;

	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR    lpCmdLine, int       nCmdShow){
		hInst = hInstance;
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), 0, WinProc);
		return 0;
	}


	INT_PTR CALLBACK WinProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		char	t[16] = { 0 };
		int		i = 0, j = 0, k = 0, x, y;
		HWAVEIN hwi = 0;
		UINT	vv = WAVE_MAPPER;
		UINT_PTR yy = (UINT_PTR)&vv;
		UNREFERENCED_PARAMETER(lParam);
		tagWAVEINCAPSA wc;
		switch (message)
		{
		case	WM_LBUTTONDOWN:
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			x = lParam & 0xFFFF;
			y = (lParam & 0xFFFF0000) >> 16;
			if (x > 12 && x < 320 && y>12 && y < 250 && SC == 90){
				j = 8 - (x - 15) / 36;
				i = 8 - (y - 10) / 28;
				if (Rot[i + j * 9] == 0){
					Rot[i + j * 9] = 1;
					if (Sudoku_Values[i][j] == 9)
						Sudoku_Values[i][j] = -1;
					Sudoku_Values[i][j]++;
					Sudoku_Values[i][j] = Sudoku_Values[i][j] % 10;
					Squar_Situation[Face[i + j * 9]][i + j * 9] = Sudoku_Values[i][j];
				}
			}
			else{
				MessageBox(hDlg, "Sudoku Solver (By CSP Method)\n", "About...", MB_ICONINFORMATION);
			}
			break;
		case	WM_RBUTTONDOWN:
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			x = lParam & 0xFFFF;
			y = (lParam & 0xFFFF0000) >> 16;
			if (x > 12 && x < 320 && y>12 && y < 250 && SC == 90){
				j = 8 - (x - 15) / 36;
				i = 8 - (y - 10) / 28;
				if (Rot[i + j * 9] == 0){
					Rot[i + j * 9] = 1;
					if (Sudoku_Values[i][j] == 0)
						Sudoku_Values[i][j] = 10;
					Sudoku_Values[i][j]--;
					Squar_Situation[Face[i + j * 9]][i + j * 9] = Sudoku_Values[i][j];
				}
			}
			else{
				MessageBox(hDlg, "Sudoku Solver (By CSP Method)", "About...", MB_ICONINFORMATION);
			}
			break;
		case WM_INITDIALOG:
			waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
			for (DWORD t = 0; t < sizeof(buffer); ++t)
				buffer[t] = (t&(t + 100)) % 256;
			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			EnableOpenGL(GetDlgItem(hDlg, IDC_DRAW), &PhDC, &PhGLRC);
			SetTimer(hDlg, 0, 50, 0);
			return (INT_PTR)TRUE;
		case WM_TIMER:
			Draw();
			break;
		case WM_DESTROY:
			waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutClose(hWaveOut);
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDCANCEL)
			{
				DisableOpenGL(hDlg, PhDC, PhGLRC);
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			if (LOWORD(wParam) == IDC_SOLVE)
			{
				SolveSudoku(hDlg);
				return (INT_PTR)TRUE;
			}
			break;
		}
		return (INT_PTR)FALSE;
	}


	void SolveSudoku(HWND hWnd){
		if (Solve()){

			for (int i = 0; i < 81; i++)
				Effect_Time[i] = rand() % 50;
			EFT = 0;
			Done_Effect = false;
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++){
					Squar_Situation[Face[j + i * 9]][j + i * 9] = Sudoku_Values[j][i];
				}
		}
	}

	void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
	{
		PIXELFORMATDESCRIPTOR pfd;
		int iFormat;
		*hDC = GetDC(hWnd);
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 16;
		pfd.iLayerType = PFD_MAIN_PLANE;
		iFormat = ChoosePixelFormat(*hDC, &pfd);
		SetPixelFormat(*hDC, iFormat, &pfd);
		*hRC = wglCreateContext(*hDC);
		wglMakeCurrent(*hDC, *hRC);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		float	v[4] = { 0, 0, -1, 0 };
		glLightfv(GL_LIGHT0, GL_POSITION, v);
		//glGenTextures( 1, &texture );
		texture = LoadTextureRAW("DIGITS.raw", TRUE);
		//glGenTextures( 1, &texture );

	}

	void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(hWnd, hDC);
	}

	void Draw(){
		float	h, v, x = -1, z = 0;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, 305, 300);
		gluPerspective(45, 1, 1, 100);

		if (!Done_Effect){
			if (EFT == 50){
				Done_Effect = true;
			}
			for (int i = 0; i < 81; i++){
				if (EFT == Effect_Time[i])
					Rot[i] = true;
			}
			t += 0.1;
			EFT++;
		}

		if (SC != 90){
			SC++;
			x1 = -sin(((SC - 90) / 180.0f)*3.14)*2.5;
			x2 = -cos(((SC - 90) / 180.0f)*3.14)*2.5;
		}
		gluLookAt(0, x1, x2, 0, 0, 0.01, 0, 1, 0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glPushMatrix();
		glTranslatef(0.4, 0.19, 0);
		for (int a = 0; a < 3; a++)
			for (int b = 0; b < 3; b++){
				glPushMatrix();
				glScalef(0.8, 0.8, 0.8);
				glTranslatef((a - 1.5f) / 1.2f, (b - 1.5f) / 1.5f, 0);
				for (int j = 0; j < 3; j++)
					for (int k = 0; k < 3; k++){
						glPushMatrix();
						glTranslatef((k - 1.5) / 4.0f, (j - 1.5) / 4.8f, 0);
						if (Rot[(a * 3 + k) * 9 + b * 3 + j] == 1){
							Face[(a * 3 + k) * 9 + b * 3 + j]++;
							Face[(a * 3 + k) * 9 + b * 3 + j] %= 4;
							RotateYA[(a * 3 + k) * 9 + b * 3 + j] += 10;
							if (RotateYA[(a * 3 + k) * 9 + b * 3 + j] % 90 == 0){
								RotateYA[(a * 3 + k) * 9 + b * 3 + j] %= 360;
								Rot[(a * 3 + k) * 9 + b * 3 + j] = 0;
							}
						}
						glRotatef(RotateYA[(a * 3 + k) * 9 + b * 3 + j], 0, 1, 0);
						Draw_Squar(Squar_Situation[1][(a * 3 + k) * 9 + b * 3 + j], Squar_Situation[0][(a * 3 + k) * 9 + b * 3 + j], Squar_Situation[2][(a * 3 + k) * 9 + b * 3 + j], Squar_Situation[3][(a * 3 + k) * 9 + b * 3 + j]);
						glPopMatrix();
						//glPopMatrix();
					}
				glPopMatrix();
			}
		glPopMatrix();


		SwapBuffers(PhDC);
	}


	void	Draw_Squar(int a, int b, int c, int d){
		int	n = a;
		glColor3f(1, 1, 1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (a != 0) ? texture : 10);
		n = a - 1;;
		glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glTexCoord2d(float(n % 3)*0.33, float(n / 3 + 1)*0.33);
		glVertex3f(-0.1, -0.1, .1);
		glTexCoord2d(float(n % 3)*0.33, float(n / 3)*0.33);
		glVertex3f(-0.1, 0.1, 0.1);
		glTexCoord2d(float(n % 3 + 1)*0.33, float(n / 3)*0.33);
		glVertex3f(0.1, 0.1, 0.1);
		glTexCoord2d(float(n % 3 + 1)*0.33, float(n / 3 + 1)*0.33);
		glVertex3f(0.1, -0.1, 0.1);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, (b != 0) ? texture : 10);

		n = b - 1;

		glBegin(GL_QUADS);
		glNormal3f(1, 0, 0);
		glTexCoord2d(float(n % 3)*0.33, float(n / 3 + 1)*0.33);
		glVertex3f(0.1, -0.1, 0.1);
		glTexCoord2d(float(n % 3)*0.33, float(n / 3)*0.33);
		glVertex3f(0.1, 0.1, 0.1);
		glTexCoord2d(float(n % 3 + 1)*0.33, float(n / 3)*0.33);
		glVertex3f(0.1, 0.1, -0.1);
		glTexCoord2d(float(n % 3 + 1)*0.33, float(n / 3 + 1)*0.33);
		glVertex3f(0.1, -0.1, -0.1);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, (c != 0) ? texture : 10);

		n = c - 1;
		glBegin(GL_QUADS);
		glNormal3f(-1, 0, 0);
		glTexCoord2d(float(n % 3 + 1)*0.33, float(n / 3 + 1)*0.33);
		glVertex3f(-0.1, -0.1, 0.1);
		glTexCoord2d(float(n % 3 + 1)*0.33, float(n / 3)*0.33);
		glVertex3f(-0.1, 0.1, 0.1);
		glTexCoord2d(float(n % 3)*0.33, float(n / 3)*0.33);
		glVertex3f(-0.1, 0.1, -0.1);
		glTexCoord2d(float(n % 3)*0.33, float(n / 3 + 1)*0.33);
		glVertex3f(-0.1, -0.1, -0.1);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, (d != 0) ? texture : 10);

		n = d - 1;
		glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glTexCoord2d(float(n % 3 + 1)*0.33, float(n / 3 + 1)*0.33);
		glVertex3f(-0.1, -0.1, -0.1);
		glTexCoord2d(float(n % 3 + 1)*0.33, float(n / 3)*0.33);
		glVertex3f(-0.1, 0.1, -0.1);
		glTexCoord2d(float(n % 3)*0.33, float(n / 3)*0.33);
		glVertex3f(0.1, 0.1, -0.1);
		glTexCoord2d(float(n % 3)*0.33, float(n / 3 + 1)*0.33);
		glVertex3f(0.1, -0.1, -0.1);
		glEnd();
	}


	GLuint LoadTextureRAW(const char * filename, int wrap)
	{
		GLuint texture;
		int width, height;
		BYTE * data;
		FILE * file;
		file = fopen(filename, "rb");
		if (file == NULL) return 0;
		width = 512;
		height = 512;
		data = (BYTE *)malloc(width * height * 3);
		fread(data, width * height * 3, 1, file);
		fclose(file);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width,
			height, GL_RGB, GL_UNSIGNED_BYTE, data);
		free(data);
		return texture;

	}
}