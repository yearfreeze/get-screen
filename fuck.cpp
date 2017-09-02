#include"stdafx.h"
#include<stdlib.h>
#include"Resource.h"
void InvertBlock(HWND hwndScr,HWND hwnd,POINT ptbeg,POINT ptend){
	HDC hdc;
	hdc=GetDCEx(hwndScr,NULL,DCX_CACHE|DCX_LOCKWINDOWUPDATE);
	ClientToScreen(hwnd,&ptbeg);
	ClientToScreen(hwnd,&ptend);
	PatBlt(hdc,ptbeg.x,ptbeg.y,ptend.x-ptbeg.x,ptend.y-ptbeg.y,DSTINVERT);
	ReleaseDC(hwndScr,hdc);
}
LRESULT CALLBACK wndproc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam){
	static BOOL bCapturing,bBlocking;
	static HBITMAP hBitmap;
	static HWND hwndSrc;
	static POINT ptBeg,ptEnd;
	HDC hdc,hdcSrc;
	PAINTSTRUCT ps;
	switch (message)
	{
	
	case WM_LBUTTONDOWN:
		if(!bCapturing){
			if(LockWindowUpdate(hwndSrc=GetDesktopWindow())){
				bCapturing=TRUE;
				SetCapture(hwnd);
				SetCursor(LoadCursor(NULL,IDC_CROSS));
			}
			else
				MessageBeep(0);
		}
		return 0;
	case WM_RBUTTONDOWN:
		if(bCapturing){
			bBlocking=TRUE;
			ptBeg.x=LOWORD(lparam);
			ptBeg.y=HIWORD(lparam);
			if(ptBeg.x<0)
				MessageBox(hwnd,L"",L"<0",0);
			ptEnd=ptBeg;
			InvertBlock(hwndSrc,hwnd,ptBeg,ptEnd);
		}
		return 0;
	case WM_MOUSEMOVE:
		if(bBlocking){
			InvertBlock(hwndSrc,hwnd,ptBeg,ptEnd);
			ptEnd.x=LOWORD(lparam);
			ptEnd.y=HIWORD(lparam);
			InvertBlock(hwndSrc,hwnd,ptBeg,ptEnd);
		}
		return 0;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if(bBlocking){
			InvertBlock(hwndSrc,hwnd,ptBeg,ptEnd);
			ptEnd.x=LOWORD(lparam);
			ptEnd.y=HIWORD(lparam);
			InvertBlock(hwndSrc,hwnd,ptBeg,ptEnd);
			if(hBitmap){
				DeleteObject(hBitmap);
				hBitmap=NULL;
			}
			InvalidateRect(hwnd,NULL,TRUE);
		}
		if(bBlocking||bCapturing){
			bBlocking=bCapturing=FALSE;
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			ReleaseCapture();
			LockWindowUpdate(NULL);
		}
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hwnd,&ps);
		hdcSrc=GetWindowDC(hwndSrc);
		ClientToScreen(hwnd,&ptBeg);
		ClientToScreen(hwnd,&ptEnd);
		BitBlt(hdc,0,0,ptEnd.x-ptBeg.x,ptEnd.y-ptBeg.y
			,hdcSrc,ptBeg.x,ptBeg.y,SRCCOPY);
		ReleaseDC(hwndSrc,hdcSrc);
		EndPaint(hwnd,&ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,message,wparam,lparam);
}

int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hprevinstance,PSTR szcmdline,int icmdshow){
	static TCHAR szappname[]=TEXT("blowup");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc=wndproc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance=hinstance;
	wndclass.hIcon=LoadIcon(hinstance,MAKEINTRESOURCE(IDI_freeze));
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName=MAKEINTRESOURCE(IDT_SCR);
	wndclass.lpszClassName=szappname;
	if(!RegisterClass(&wndclass)){
		MessageBox(NULL,TEXT("Error"),szappname,MB_ICONERROR);
		return 0;
	}
	
	hwnd=CreateWindow(szappname,TEXT("fuck screen"),
		WS_OVERLAPPEDWINDOW,0,0,
		800,300,NULL,NULL,hinstance,NULL);
	ShowWindow(hwnd,icmdshow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
