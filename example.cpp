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
HBITMAP CopyBitmap(HBITMAP hBitmapSrc){
	BITMAP bitmap;
	HBITMAP hBitmapDst;
	HDC hdcSrc,hdcDst;

	GetObject(hBitmapSrc,sizeof(BITMAP),&bitmap);
	hBitmapDst=CreateBitmapIndirect(&bitmap);
	hdcSrc=CreateCompatibleDC(NULL);
	hdcDst=CreateCompatibleDC(NULL);
	
	SelectObject(hdcSrc,hBitmapSrc);
	SelectObject(hdcDst,hBitmapDst);
	BitBlt(hdcDst,0,0,bitmap.bmWidth,bitmap.bmHeight,hdcSrc,0,0,SRCCOPY);

	DeleteDC(hdcSrc);
	DeleteDC(hdcDst);
	
	return hBitmapDst;
}


LRESULT CALLBACK wndproc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam){
	static BOOL bCapturing,bBlocking;
	static HBITMAP hBitmap;
	static HWND hwndSrc;
	static POINT ptBeg,ptEnd;
	BITMAP bm;
	HBITMAP hBitmapClip;
	HDC hdc,hdcMem;
	int iEable;
	PAINTSTRUCT ps;
	RECT rect;
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
			//InvertBlock(hwndSrc,hwnd,ptBeg,ptEnd);
			ptEnd.x=LOWORD(lparam);
			ptEnd.y=HIWORD(lparam);
			InvertBlock(hwndSrc,hwnd,ptBeg,ptEnd);
			if(hBitmap){
				DeleteObject(hBitmap);
				hBitmap=NULL;
			}
			hdc=GetDC(hwndSrc);  //???
			hdcMem=CreateCompatibleDC(hdc);
			hBitmap=CreateCompatibleBitmap(hdc,
				abs(ptEnd.x-ptBeg.x),abs(ptEnd.y-ptBeg.y));
			SelectObject(hdcMem,hBitmap);
			StretchBlt(hdcMem,0,0,abs(ptEnd.x-ptBeg.x),abs(ptEnd.y-ptBeg.y),
				hdc,ptBeg.x,ptBeg.y,ptEnd.x-ptBeg.x,ptEnd.y-ptBeg.y,SRCCOPY);
			DeleteDC(hdcMem);
			ReleaseDC(hwnd,hdc);
			InvalidateRect(hwnd,NULL,TRUE);
		}
		if(bBlocking||bCapturing){
			bBlocking=bCapturing=FALSE;
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			ReleaseCapture();
			LockWindowUpdate(NULL);
		}
		return 0;
	case WM_INITMENUPOPUP:
		iEable=IsClipboardFormatAvailable(CF_BITMAP)?MF_ENABLED:MF_GRAYED;
		EnableMenuItem((HMENU)wparam,IDM_EDIT_PASTE,iEable);
		
		iEable=hBitmap?MF_ENABLED:MF_GRAYED;
		EnableMenuItem((HMENU)wparam,IDM_EDIT_CUT,iEable);
		EnableMenuItem((HMENU)wparam,IDM_EDIT_COPY,iEable);
		EnableMenuItem((HMENU)wparam,IDM_EDIT_DELETE,iEable);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
			if(hBitmap){
				hBitmapClip=CopyBitmap(hBitmap);
				OpenClipboard(hwnd);
				EmptyClipboard();
				SetClipboardData(CF_BITMAP,hBitmapClip);
			}
			if(LOWORD(wparam)==IDM_EDIT_COPY)
				return 0;
		case IDM_EDIT_DELETE:
			if(hBitmap){
				DeleteObject(hBitmap);
				hBitmap=NULL;
			}
			InvalidateRect(hwnd,NULL,TRUE);
			return 0;
		case IDM_EDIT_PASTE:
			if(hBitmap){
				DeleteObject(hBitmap);
				hBitmap=NULL;
			}
			OpenClipboard(hwnd);
			hBitmapClip=(HBITMAP)GetClipboardData(CF_BITMAP);
			if(hBitmapClip)
				hBitmap=CopyBitmap(hBitmapClip);
			CloseClipboard();
			InvalidateRect(hwnd,NULL,TRUE);
			return 0;

		}
		break;
	case WM_PAINT:
		hdc=BeginPaint(hwnd,&ps);
		if(hBitmap){
			GetClientRect(hwnd,&rect);
			hdcMem=CreateCompatibleDC(hdc);
			SelectObject(hdcMem,hBitmap);
			GetObject(hBitmap,sizeof(BITMAP),(PSTR)&bm);
			SetStretchBltMode(hdc,COLORONCOLOR);
			StretchBlt(hdc,0,0,rect.right,rect.bottom,hdcMem,0,0,
				bm.bmWidth,bm.bmHeight,SRCCOPY);
			DeleteDC(hdcMem);
		}
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
	
	hwnd=CreateWindow(szappname,TEXT("blow up demo"),
		WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
		CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hinstance,NULL);
	ShowWindow(hwnd,icmdshow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
