#include"stdafx.h"
#include"Resource.h"
BOOL SaveBitmapToFile(LPCTSTR lpszFilePath, HBITMAP hBm)
{
    //  定义位图文件表头
    BITMAPFILEHEADER bmfh;
    //  定义位图信息表头
    BITMAPINFOHEADER bmih;
    //  调色板长度
    int nColorLen = 0;
    //  调色表大小
    DWORD dwRgbQuadSize = 0;
    //  位图大小
    DWORD dwBmSize = 0;
    //  分配内存的指针
    HGLOBAL    hMem = NULL;
    LPBITMAPINFOHEADER     lpbi;
    BITMAP bm;
	HDC hdc;
    HANDLE hFile = NULL;
    DWORD dwWritten;
    GetObject(hBm, sizeof(BITMAP), &bm);
    bmih.biSize    = sizeof(BITMAPINFOHEADER);    // 本结构所占的字节
    bmih.biWidth    = bm.bmWidth;            // 位图宽
    bmih.biHeight    = bm.bmHeight;            // 位图高
    bmih.biPlanes    = 1;
    bmih.biBitCount        = bm.bmBitsPixel;    // 每一图素的位数
    bmih.biCompression    = BI_RGB;            // 不压缩
    bmih.biSizeImage        = 0;  //  位图大小
    bmih.biXPelsPerMeter    = 0;
    bmih.biYPelsPerMeter    = 0;
    bmih.biClrUsed        = 0;
    bmih.biClrImportant    = 0;

    //  计算位图图素数据区大小 
    dwBmSize = 4 * ((bm.bmWidth * bmih.biBitCount + 31) / 32) * bm.bmHeight;
    //  如果图素位 <= 8bit，则有调色板
    if (bmih.biBitCount <= 8)
    {
        nColorLen = (1 << bm.bmBitsPixel);
    }
   //  计算调色板大小
    dwRgbQuadSize = nColorLen * sizeof(RGBQUAD);
    //  分配内存
    hMem = GlobalAlloc(GHND, dwBmSize + dwRgbQuadSize + sizeof(BITMAPINFOHEADER));
    if (NULL == hMem)
    {
        return FALSE;
    }
    //  锁定内存
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hMem);
    //  将bmih中的内容写入分配的内存中
    *lpbi = bmih;
	hdc=GetDC(NULL);
    //  将位图中的数据以bits的形式放入lpData中。
    GetDIBits(hdc, hBm, 0, (DWORD)bmih.biHeight, (LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwRgbQuadSize, (BITMAPINFO *)lpbi, (DWORD)DIB_RGB_COLORS);  
    bmfh.bfType = 0x4D42;  // 位图文件类型：BM
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwRgbQuadSize + dwBmSize;  // 位图大小
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits    = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwRgbQuadSize;  // 位图数据与文件头部的偏移量
    //  把上面的数据写入文件中
    hFile = CreateFile(lpszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return FALSE;
    }
    //  写入位图文件头
    WriteFile(hFile, (LPSTR)&bmfh, sizeof(BITMAPFILEHEADER), (DWORD *)&dwWritten, NULL);
    //  写入位图数据
    WriteFile(hFile, (LPBITMAPINFOHEADER)lpbi, bmfh.bfSize - sizeof(BITMAPFILEHEADER), (DWORD *)&dwWritten, NULL);
    GlobalFree(hMem);
    CloseHandle(hFile);
    return TRUE;
}
void drawbox(HWND hwnd,POINT ptbeg,POINT ptend){
	HDC hdc;
	hdc=GetDCEx(hwnd,NULL,DCX_CACHE|DCX_LOCKWINDOWUPDATE);
	SetROP2(hdc,R2_NOT);
	SelectObject(hdc,GetStockObject(NULL_BRUSH));
	ClientToScreen(hwnd,&ptbeg);
	ClientToScreen(hwnd,&ptend);
	Rectangle(hdc,ptbeg.x,ptbeg.y,ptend.x,ptend.y);
	ReleaseDC(hwnd,hdc);
}
LRESULT CALLBACK wndproc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam){
	static int xclient,yclient;
	//TCHAR pos[50];
	static BOOL bCapturing,bBlocking;
	static int length;
	static HWND hwnd_Screen;
	static POINT pt;
	static POINT ptBegin,ptEnd;
	HDC hdc,hdc_Screen,hdcmem;
	PAINTSTRUCT ps;
	static HBITMAP hbitmap;
	switch (message)
	{
	case WM_CREATE:
		hwnd_Screen=GetDesktopWindow();
		return 0;
	case WM_SIZE:
		xclient=LOWORD(lparam);
		yclient=HIWORD(lparam);
		return 0;
	case WM_LBUTTONDOWN:
		if(!bCapturing){
			bCapturing=TRUE;
			SetCapture(hwnd);
			SetCursor(LoadCursor(NULL,IDC_CROSS));
			//ShowWindow(hwnd,SW_HIDE);
		}
		return 0;
	case WM_RBUTTONDOWN:
		if(bCapturing){
			bBlocking=TRUE;
			ptBegin.x=ptEnd.x=LOWORD(lparam);
			ptBegin.y=ptEnd.y=HIWORD(lparam);
		
			drawbox(hwnd_Screen,ptBegin,ptEnd);
		}
		return 0;
	case WM_MOUSEMOVE:
		if(bBlocking){
			drawbox(hwnd_Screen,ptBegin,ptEnd);
			ptEnd.x=LOWORD(lparam);
			ptEnd.y=HIWORD(lparam);
			//ClientToScreen(hwnd,&ptBegin);
			//ClientToScreen(hwnd,&ptEnd);
			drawbox(hwnd_Screen,ptBegin,ptEnd);
		}
		return 0;

	case WM_RBUTTONUP:
		if(bBlocking){
		ptEnd.x=LOWORD(lparam);
		ptEnd.y=HIWORD(lparam);
		//ClientToScreen(hwnd,&ptBegin);
		//ClientToScreen(hwnd,&ptEnd);
		drawbox(hwnd_Screen,ptBegin,ptEnd);
		hdc=GetDC(hwnd);
		hdc_Screen=GetDC(hwnd_Screen);
		BitBlt(hdc,0,0,ptEnd.x-ptBegin.x,ptEnd.y-ptBegin.y
			,hdc_Screen,ptBegin.x,ptBegin.y,SRCCOPY);

		hbitmap=CreateCompatibleBitmap(hdc_Screen,ptEnd.x-ptBegin.x,ptEnd.y-ptBegin.y);
		hdcmem=CreateCompatibleDC(hdc_Screen);
		SelectObject(hdcmem,hbitmap);
		BitBlt(hdcmem,0,0,ptEnd.x-ptBegin.x,ptEnd.y-ptBegin.y
			,hdc,0,0,SRCCOPY);
		if(SaveBitmapToFile(L"f:\\a12.bmp",hbitmap)==TRUE)
			MessageBox(hwnd,L"",L"1",MB_OK);

		ReleaseDC(hwnd_Screen,hdc_Screen);
		ReleaseDC(hwnd,hdc);

		if(bBlocking||bCapturing){
			bBlocking=bCapturing=FALSE;
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			ReleaseCapture();
		}
		}
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hwnd,&ps);
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
		WS_OVERLAPPEDWINDOW,0,0,
		400,200,NULL,NULL,hinstance,NULL);
	ShowWindow(hwnd,icmdshow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
