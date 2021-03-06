// SimpleImageProcessing.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SimpleImageProcessing.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWnd, hCanvas, hHistRGB, hHistGray, hScaleW, hScaleH, hBTN;		// window handlers

// GDI+ variables
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;
Bitmap *gpBitmap, *gpBitmapBackUp;                                           // The bitmap for displaying an image

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc2(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// File control varialbes and functions
OPENFILENAME ofn;								// open file handler
wchar_t szFileName[MAX_PATH] = L"";
wchar_t *szFileExt;

int GetEncoderClsid(const WCHAR*, CLSID*);
void MyOpenFile(HWND, bool);
void MySaveFile(HWND, const wchar_t *);

// variables and functions for undo/redo feature
void clear_undo();
vector<Bitmap*> undo_vec, redo_vec;
void Ondo();
Bitmap* Undo();
Bitmap* Redo();

//painting canvas and hist
unsigned int hist[4][256];
unsigned int *pHist = &hist[0][0];
unsigned int hist_max = 1;
bool newHist = false;
int newX, newH;

void MyOnPaint(HDC);
void MyOnPaint2(HDC, bool);

//other
void Refresh(HWND, bool);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	// Initialize GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	gpBitmap = NULL;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SimpleImageProcessing, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	//
	/*LoadStringW(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInst, IDC_CANVAS, hCanvasClass, MAX_LOADSTRING);*/

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SimpleImageProcessing));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | ES_AUTOVSCROLL;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SimpleImageProcessing));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SimpleImageProcessing);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_DESKTOP, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   hCanvas = CreateWindowW(szWindowClass, _T("Canvas"), WS_CHILDWINDOW | ES_MULTILINE | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL,
	   25, 25, 1000, 600, hWnd, nullptr, GetModuleHandle(NULL), nullptr);
   /*hCanvas = CreateWindowW(L"EDIT", 0, WS_CHILDWINDOW | ES_READONLY | ES_MULTILINE | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL,
	   25, 25, 1000, 600, hWnd, nullptr, GetModuleHandle(NULL), nullptr);*/
   /*hCanvas = CreateWindowEx(WS_EX_CLIENTEDGE,L"EDIT", 0, WS_CHILDWINDOW | ES_READONLY | ES_MULTILINE | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL,
	   25, 25, 1000, 600, hWnd, nullptr, GetModuleHandle(NULL), nullptr);*/

   hHistRGB = CreateWindowW(szWindowClass, _T("Histogram"), WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER /*| SS_BITMAP*/ ,
	   1050, 25, 258, 130, hWnd, nullptr, GetModuleHandle(NULL), nullptr);
   if (!hCanvas)
   {
	   return FALSE;
   }

   hHistGray = CreateWindowW(szWindowClass, _T("Histogram"), WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER /*| SS_BITMAP*/,
	   1050, 300, 258, 130, hWnd, nullptr, GetModuleHandle(NULL), nullptr);
   if (!hCanvas)
   {
	   return FALSE;
   }

   hScaleW = CreateWindowW(L"EDIT", L"newX",  WS_CHILD | WS_BORDER | WS_VISIBLE, 1050, 500, 50, 20, hWnd, 0, GetModuleHandle(NULL), 0);
   hScaleH = CreateWindowW(L"EDIT", L"newH",  WS_CHILD | WS_BORDER | WS_VISIBLE, 1150, 500, 50, 20, hWnd, 0, GetModuleHandle(NULL), 0);
   hBTN = CreateWindowEx(NULL, L"BUTTON", L"Set new dimentions", WS_CHILD | WS_VISIBLE, 1050, 550, 150, 30, hWnd, (HMENU)BTN_SCALE, GetModuleHandle(NULL), 0);
   SendMessage(GetDlgItem(hScaleW, W_SCALE), CB_ADDSTRING, 0, (LPARAM)L"new width");
   SendMessage(GetDlgItem(hScaleH, H_SCALE), CB_ADDSTRING, 0, (LPARAM)L"new height");
   //ShowWindow(hHist, nCmdShow);

   ShowWindow(hWnd, SW_MAXIMIZE);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case BTN_SCALE:
				/*newX = SendMessage(FindWindow(NULL, TEXT("newX")), WM_GETTEXTLENGTH, 0, 0);
				newH = SendMessage(FindWindow(NULL, TEXT("newH")), WM_GETTEXTLENGTH, 0, 0);*/
				newX = GetDlgItemInt(hScaleW,W_SCALE, 0, 0);
				newH = GetDlgItemInt(hScaleH, H_SCALE, 0, 0);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_OPEN:
				MyOpenFile(hWnd, false);
				Refresh(hWnd, true);
				break;
			case IDM_REOPEN:
				MyOpenFile(hWnd, true);
				Refresh(hWnd, true);
				break;
			case IDM_SAVE_BMP:
				MySaveFile(hWnd, L"bmp");
				break;
			case IDM_SAVE_JPG:
				MySaveFile(hWnd, L"jpg");
				break;
			case IDM_SAVE_GIF:
				MySaveFile(hWnd, L"gif");
				break;
			case IDM_SAVE_TIF:
				MySaveFile(hWnd, L"tif");
				break;
			case IDM_SAVE_PNG:
				MySaveFile(hWnd, L"png");
				break;

			case IDM_UNDO:
				gpBitmap = Undo();
				Refresh(hWnd, true);
				break;
			case IDM_REDO:
				gpBitmap = Redo();
				Refresh(hWnd, true);
				break;
			case IDM_HIST:
				Hist(gpBitmap, hist, &hist_max);
				newHist = true;
				Refresh(hWnd, true);
				break;
			case IDM_MIRROR:
				Ondo();
				Mirror(gpBitmap, 1);
				Refresh(hWnd, true);
				break;
			case IDM_R_ROTATE:
				undo_vec.push_back(gpBitmap);
				gpBitmap = Rotate(gpBitmap, 1);
				Refresh(hWnd, true);
				break;
			case IDM_L_ROTATE:
				undo_vec.push_back(gpBitmap);
				gpBitmap = Rotate(gpBitmap, 2);
				Refresh(hWnd, true);
				break;
			case IDM_R_MIRROR:
				undo_vec.push_back(gpBitmap);
				gpBitmap = Mirror2(gpBitmap, 1);
				Refresh(hWnd, true);
				break;
			case IDM_L_MIRROR:
				undo_vec.push_back(gpBitmap);
				gpBitmap = Mirror2(gpBitmap, 2);
				Refresh(hWnd, true);
				break;
			case IDM_SCALE:
				undo_vec.push_back(gpBitmap);
				//newX = GetDlgItemInt(hScaleW, W_SCALE, NULL, false);
				//newH = GetDlgItemInt(hScaleH, H_SCALE, NULL, false);
				gpBitmap = Scale(gpBitmap, 1000, 600);
				Refresh(hWnd, true);
				break;
			/*case IDM_CONV:
				TestConvolution(gpBitmap);
				Refresh(hWnd, true);
				break;
			case IDM_REM_BLUE:
				RemoveBlue(gpBitmap);
				Refresh(hWnd, true);
				break;*/
			case IDM_INVERT:
				Ondo();
				Invert(gpBitmap);
				Refresh(hWnd, true);
				break;
			case IDM_GRAY:
				Ondo();
				GrayScale(gpBitmap);
				Refresh(hWnd, true);
				break;
			case IDM_R_CHANNEL:
				Ondo();
				CChannel(gpBitmap,1);
				Refresh(hWnd, true);
				break;
			case IDM_G_CHANNEL:
				Ondo();
				CChannel(gpBitmap, 2);
				Refresh(hWnd, true);
				break;
			case IDM_B_CHANNEL:
				Ondo();
				CChannel(gpBitmap, 3);
				Refresh(hWnd, true);
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps, ps2, ps3, ps4;
			

            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			
			HDC hdc2 = BeginPaint(hCanvas, &ps2);
			MyOnPaint(hdc2);
			EndPaint(hCanvas, &ps2);
			if (newHist)
			{
				HDC hdc3 = BeginPaint(hHistRGB, &ps3);
				MyOnPaint2(hdc3, false);
				EndPaint(hHistRGB, &ps3);

				HDC hdc4 = BeginPaint(hHistGray, &ps4);
				MyOnPaint2(hdc4, true);
				EndPaint(hHistGray, &ps4);
				newHist = false;
			}
					
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

void Refresh(HWND hWnd, bool undo)
{
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

void clear_undo()
{
	while (!undo_vec.empty())
	{
		delete undo_vec.back();
		undo_vec.pop_back();
	}
	while (!redo_vec.empty())
	{
		delete redo_vec.back();
		redo_vec.pop_back();
	}
}

void MyOpenFile(HWND hWnd, bool reopen)
{
	if (!reopen)
	{
		//OPENFILENAME ofn;
		//wchar_t szFileName[MAX_PATH] = L"";
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFilter =
			L"Image Files (*.bmp; *.gif; *.jpeg; *.jpg; *.png, *.tif; *.tiff)\0*.bmp; *.gif; *.jpeg; *.jpg; *.png; *.tif; *.tiff\0"
			L"BMP (*.bmp)\0*.bmp\0"
			L"GIF (*.gif)\0*.gif\0"
			L"JPEG Files (*.jpeg; *.jpg)\0*.jpeg; *.jpg\0"
			L"PNG (*.png)\0*.png\0"
			L"TIF (*.tif; *.tiff)\0*.tif; *.tiff\0"
			L"All Files (*.*)\0*.*\0";
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
		ofn.lpstrDefExt = L"jpg";
		if (GetOpenFileName(&ofn))
		{
			// If we got this far, the user has chosen a file.
			// GDI+ supports BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, and EMF.
			if (gpBitmap)
			{
				// We need to destroy the old bitmap
				delete gpBitmap;
			}
			gpBitmap = new Bitmap(szFileName);
			clear_undo();
			//gpBitmapBackUp = new Bitmap(szFileName);
			// We need to force the window to redraw itself
			/*InvalidateRect(hCanvas, NULL, TRUE);
			UpdateWindow(hCanvas);*/
			//InvalidateRect(hWnd, NULL, TRUE);
			//UpdateWindow(hWnd);
		}
	}
	else
	{
		if (gpBitmap)
		{
			// We need to destroy the old bitmap
			delete gpBitmap;
		}
		gpBitmap = new Bitmap(szFileName);
		clear_undo();
		//gpBitmapBackUp = new Bitmap(szFileName);
		// We need to force the window to redraw itself
		/*InvalidateRect(hCanvas, NULL, TRUE);
		UpdateWindow(hCanvas);*/
		//Refresh(hWnd,false);
	}
}

void MySaveFile(HWND hWnd, const wchar_t * ext)
{
	//OPENFILENAME ofn2 = ofn;
	//wchar_t szFileName[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	if (ext == L"bmp")
	{
		ofn.lpstrFilter = L"BMP (*.bmp)\0*.bmp\0";
	}
	else if (ext == L"jpg")
	{
		ofn.lpstrFilter = L"JPG (*.jpg)\0*.jpg\0";
	}
	else if (ext == L"gif")
	{
		ofn.lpstrFilter = L"GIF (*.gif)\0*.gif\0";
	}
	else if (ext == L"tif")
	{
		ofn.lpstrFilter = L"TIF (*.tif)\0*.tif\0";
	}
	else if (ext == L"png")
	{
		ofn.lpstrFilter = L"PNG (*.png)\0*.png\0";
	}
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = ext;
	if (GetSaveFileName(&ofn))
	{
		if (!gpBitmap)
		{
			// We need to destroy the old bitmap
			return;
		}

		CLSID   encoderClsid;
		Status  stat;


		if (ext == L"bmp")
			GetEncoderClsid(L"image/bmp", &encoderClsid);
		else if (ext == L"jpg")
			GetEncoderClsid(L"image/jpeg", &encoderClsid);
		else if (ext == L"gif")
			GetEncoderClsid(L"image/gif", &encoderClsid);
		else if (ext == L"tif")
			GetEncoderClsid(L"image/tiff", &encoderClsid);
		else if (ext == L"png")
			GetEncoderClsid(L"image/png", &encoderClsid);

		stat = gpBitmap->Save(szFileName, &encoderClsid, NULL);

	}
}


void MyOnPaint2(HDC hdc, bool g)
{

	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));
	if (g)
	{
		for (int j = 0; j < 256; j++)
		{
			int pv = 130 - (hist[3][j] * 130 / hist_max);
			//int pv = 130 - hist[i][j];
			graphics.DrawLine(&pen, j, 130, j, pv);
		}
	}
	else
	{
		for (int j = 0; j < 256; j++)
		{
			int pvr = 130 - (hist[0][j] * 130 / hist_max);
			int pvg = 130 - (hist[1][j] * 130 / hist_max);
			int pvb = 130 - (hist[2][j] * 130 / hist_max);
			//int pv = 130 - hist[i][j];
			if (pvr <= pvg && pvr <= pvb)
			{
				pen.SetColor(Color(255, 255, 0, 0));
				graphics.DrawLine(&pen, j, 130, j, pvr);
				if (pvg <= pvb)
				{
					pen.SetColor(Color(255, 0, 255, 0));
					graphics.DrawLine(&pen, j, 130, j, pvg);
					pen.SetColor(Color(255, 0, 0, 255));
					graphics.DrawLine(&pen, j, 130, j, pvb);
				}
				else
				{
					pen.SetColor(Color(255, 0, 0, 255));
					graphics.DrawLine(&pen, j, 130, j, pvb);
					pen.SetColor(Color(255, 0, 255, 0));
					graphics.DrawLine(&pen, j, 130, j, pvg);
				}
			}
			else if (pvg <= pvr && pvg <= pvb)
			{
				pen.SetColor(Color(255, 0, 255, 0));
				graphics.DrawLine(&pen, j, 130, j, pvg);
				if (pvr <= pvb)
				{
					pen.SetColor(Color(255, 255, 0, 0));
					graphics.DrawLine(&pen, j, 130, j, pvr);
					pen.SetColor(Color(255, 0, 0, 255));
					graphics.DrawLine(&pen, j, 130, j, pvb);
				}
				else
				{
					pen.SetColor(Color(255, 0, 0, 255));
					graphics.DrawLine(&pen, j, 130, j, pvb);
					pen.SetColor(Color(255, 255, 0, 0));
					graphics.DrawLine(&pen, j, 130, j, pvr);
				}
			}
			else if (pvb <= pvr && pvb <= pvg)
			{
				pen.SetColor(Color(255, 0, 0, 255));
				graphics.DrawLine(&pen, j, 130, j, pvb);
				if (pvr <= pvg)
				{
					pen.SetColor(Color(255, 255, 0, 0));
					graphics.DrawLine(&pen, j, 130, j, pvr);
					pen.SetColor(Color(255, 0, 255, 0));
					graphics.DrawLine(&pen, j, 130, j, pvg);
				}
				else
				{
					pen.SetColor(Color(255, 0, 255, 0));
					graphics.DrawLine(&pen, j, 130, j, pvg);
					pen.SetColor(Color(255, 255, 0, 0));
					graphics.DrawLine(&pen, j, 130, j, pvr);
				}
			}
		}
	}
}

void MyOnPaint(HDC hdc)
{
	if (gpBitmap)
	{
		Graphics graphics(hdc);
		graphics.DrawImage(gpBitmap, 0, 0);
	}
}

void Ondo()
{
	if (gpBitmap)
	{
		Bitmap *tmp = gpBitmap->Clone(0, 0, gpBitmap->GetWidth(), gpBitmap->GetHeight(), PixelFormat32bppARGB);
		//memcpy(tmp, gpBitmap, sizeof(gpBitmap));
		undo_vec.push_back(tmp);
	}
}

Bitmap* Undo()
{
	if (!undo_vec.empty())
	{
		//iteration--;
		redo_vec.push_back(gpBitmap);
		Bitmap *tmp = undo_vec.back();
		undo_vec.pop_back();
		return tmp;
	}
	else
		return gpBitmap;
}

Bitmap* Redo()
{
	if (!redo_vec.empty())
	{
		//iteration++;
		undo_vec.push_back(gpBitmap);
		Bitmap *tmp = redo_vec.back();
		redo_vec.pop_back();
		return tmp;
	}
	else
		return gpBitmap;
}