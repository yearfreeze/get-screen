#include"stdafx.h"
#include"Resource.h"
void drawbox(HWND hwnd,POINT ptbeg,POINT ptend){
	HDC hdc;
	hdc=GetDC(hwnd);
	SetROP2(hdc,R2_NOT);
	SelectObject(hdc,GetStockObject(NULL_BRUSH));
	Rectangle(hdc,ptbeg.x,ptbeg.y,ptend.x,ptend.y);
	ReleaseDC(hwnd,hdc);
}

LRESULT CALLBACK wndproc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam){
	static int xclient,yclient;
	HDC hdc,hdcdesktop;
	static HWND hdtop;
	PAINTSTRUCT ps;
	static POINT ptBeg,ptEnd;
	static BOOL flag;

	TCHAR pos[50];
	static int length;
	switch (message)
	{
	case WM_CREATE:
		flag=FALSE;
		
		hdtop=GetDesktopWindow();
		//LockWindowUpdate(hdtop);
		return 0;
	case WM_SIZE:
		xclient=LOWORD(lparam);
		yclient=HIWORD(lparam);
		return 0;
	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_SHIFT:
			SetCapture(hwnd);
			LockWindowUpdate(hdtop);
			//LockWindowUpdate(hwnd);
			break;
		}
		return 0;
	case WM_LBUTTONDOWN:
		//SetCapture(hwnd);
		ptBeg.x=ptEnd.x=LOWORD(lparam);
		ptBeg.y=ptEnd.y=HIWORD(lparam);
		ClientToScreen(hwnd,&ptBeg);
		length=wsprintf(pos,L"x:%d,y:%d",ptBeg.x,ptBeg.y);
		//drawbox(hdtop,ptBeg,ptEnd);
		//flag=TRUE;
		
		hdc=GetDC(hwnd);
		TextOut(hdc,0,0,pos,length);
		ReleaseDC(hwnd,hdc);
		return 0;
	case WM_MOUSEMOVE:
		//if(flag){
		//drawbox(hdtop,ptBeg,ptEnd);
		//SetCapture(hwnd);
		ptEnd.x=LOWORD(lparam);
		ptEnd.y=HIWORD(lparam);
		ClientToScreen(hwnd,&ptEnd);
		length=wsprintf(pos,L"x:%d,y:%d",ptEnd.x,ptEnd.y);
		hdc=GetDC(hwnd);
		TextOut(hdc,0,0,pos,length);
		ReleaseDC(hwnd,hdc);
		//drawbox(hdtop,ptBeg,ptEnd);
		//}
		return 0;
	case WM_LBUTTONUP:
		//LockWindowUpdate(NULL);
		length=wsprintf(pos,L"x:%d,y:%d",LOWORD(lparam),HIWORD(lparam));
		hdc=GetDC(hwnd);
		TextOut(hdc,0,0,pos,length);
		ReleaseDC(hwnd,hdc);
		flag=FALSE;
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hwnd,&ps);
		/*hdcdesktop=GetWindowDC(hdtop);
		
		BitBlt(hdc,0,0,xclient,yclient,hdcdesktop,800,800,SRCCOPY);
		
		ReleaseDC(hdtop,hdcdesktop);*/
		EndPaint(hwnd,&ps);
		return 0;
	case WM_DESTROY:
		ReleaseCapture();
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,message,wparam,lparam);
}

int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hprevinstance,PSTR szcmdline,int icmdshow){
	static TCHAR szappname[]=TEXT("screen");
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
	wndclass.lpszMenuName=NULL;
	wndclass.lpszClassName=szappname;
	if(!RegisterClass(&wndclass)){
		MessageBox(NULL,TEXT("Error"),szappname,MB_ICONERROR);
		return 0;
	}
	
	hwnd=CreateWindow(szappname,TEXT("get to screen"),
		WS_OVERLAPPEDWINDOW,100,100,
		1024,768,NULL,NULL,hinstance,NULL);
	ShowWindow(hwnd,icmdshow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
