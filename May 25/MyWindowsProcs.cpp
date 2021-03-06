//******************************************************//
//	MyWindowsProcs.cpp by Tianjiao Huang, 2016-2018		//
//******************************************************//
#include "stdafx.h"
#include "Resource.h"
#include "MyWindowsProcs.h"
#include "MyDirectXStuff.h"
#include "MathInput.h"

UINT								EditIndent;
UINT								EditSpacing;
UINT								EditLength;
RECT								ClientRect;				//Client rectangle
RECT								DXClientRect;				//DX client rectangle
RECT								UIClientRect;				//UI client rectangle

static TCHAR						szTitle[] = TEXT("Solid of Revolution");
static TCHAR						szWindowClass[] = TEXT("MainClass");
static TCHAR						UIClass[] = TEXT("UIClass");
static TCHAR						DXClass[] = TEXT("DXClass");

HINSTANCE							g_hInstance = nullptr;
HWND								g_hWnd = nullptr;
HWND								g_hWndDX = nullptr;
HWND								g_hWndUI = nullptr;
HWND								g_hWndMethod = nullptr;
HWND								g_hWndEquation_1 = nullptr;
HWND								g_hWndEquation_1_Adv = nullptr;
HWND								g_hWndEquation_2 = nullptr;
HWND								g_hWndEquation_2_Adv = nullptr;
HWND								g_hWndLeftBound = nullptr;
HWND								g_hWndRightBound = nullptr;
HWND								g_hWndLeftBoundAdv = nullptr;
HWND								g_hWndRightBoundAdv = nullptr;
HWND								g_hWndNCount = nullptr;
HWND								g_hWndGoInfinite = nullptr;
HWND								g_hWndButton = nullptr;
HWND								g_hWndLeftCheck = nullptr;
HWND								g_hWndRightCheck = nullptr;

HBITMAP								g_hbmAdv = NULL;
HBITMAP								g_hbmAdvActi = NULL;
WNDPROC								wpOrigEditProc;


LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		BITMAP bm;
		PAINTSTRUCT ps;

		HDC hdc = BeginPaint(hWnd, &ps);

		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmAdvActi);

		GetObject(g_hbmAdvActi, sizeof(bm), &bm);

		BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);
	}
	break;

	default:
		break;
	}
	return CallWindowProc(wpOrigEditProc, hWnd, message, wParam, lParam); 
}

// Functions
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		if (g_hWndDX == 0 || g_hWndButton == 0)
		{
			RECT ClientRect;
			RegisterMyClass(g_hInstance);
			GetClientRect(hWnd, &ClientRect);
			AdjustClientRect(ClientRect, &DXClientRect, &UIClientRect);
			// Create UI first so that DX can read cfg
			g_hWndUI = CreateWindowEx(NULL, UIClass, TEXT("UI window"), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
				DXClientRect.right, 0, UIClientRect.right, UIClientRect.bottom, hWnd, (HMENU)ID_UI, g_hInstance, NULL);
			g_hWndDX = CreateWindow(DXClass, TEXT("DX window"), WS_CHILD | WS_VISIBLE,
				0, 0, DXClientRect.right, DXClientRect.bottom, hWnd, (HMENU)ID_DX, g_hInstance, NULL);

			// Load bitmap into application
			g_hbmAdvActi = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_ADVICONACTI));
			if (g_hbmAdvActi == NULL)
				MessageBox(NULL, L"Could not load IDB_ADVACTI", NULL, NULL);
			g_hbmAdv = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_ADVICON));
			if (g_hbmAdv == NULL)
				MessageBox(NULL, L"Could not load IDB_ADV", NULL, NULL);

			SetFocus(g_hWndDX);
		}
		return 0;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case 0:
			break;
		default:
			break;
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_CLOSE:
	{
		if (IDNO == MessageBoxA(NULL, "Do you want to Quit?", "Quit", MB_YESNO | MB_DEFBUTTON1))
			return 0;

		SetInputs();
		SetSetting();
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//This program does not handle WM_SIZE

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK UIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE:
	{
		UIClientRect;
		GetWindowRect(hWnd, &UIClientRect);
		LONG UIWidth = UIClientRect.right - UIClientRect.left;
		LONG UIHeight = UIClientRect.bottom - UIClientRect.top;
		static UINT cxChar = LOWORD(GetDialogBaseUnits());
		static UINT cyChar = (UINT)(HIWORD(GetDialogBaseUnits()) * 1.4f);
		// Create grandchild windows
		EditSpacing = (UINT)(UIClientRect.bottom / 9.5f);
		EditIndent = 100;
		UINT EditHeight = (UINT)(cyChar * 1.4f);
		UINT EditWidth = UIWidth * 4 / 5;
		UINT k = 0;
		g_hWndMethod = CreateWindowExA(NULL, "combobox", "g_NCount", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL | WS_GROUP,
			40, EditIndent + k++ * EditSpacing, EditWidth, 200, hWnd, (HMENU)ID_METHOD, g_hInstance, NULL);
		g_hWndEquation_1 = CreateWindowExA(NULL, "edit", "Equation", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
			40, EditIndent + k++ * EditSpacing, EditWidth, EditHeight, hWnd, (HMENU)ID_EQUATION_1, g_hInstance, NULL);
		g_hWndEquation_2 = CreateWindowExA(NULL, "edit", "Equation", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
			40, EditIndent + k++ * EditSpacing, EditWidth, EditHeight, hWnd, (HMENU)ID_EQUATION_2, g_hInstance, NULL);

		g_hWndLeftBound = CreateWindowExA(NULL, "edit", "g_LeftBound", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
			40, EditIndent + k++ * EditSpacing, EditWidth, EditHeight, hWnd, (HMENU)ID_LEFTBOUND, g_hInstance, NULL);
		g_hWndRightBound = CreateWindowExA(NULL, "edit", "g_RightBound", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
			40, EditIndent + k++ * EditSpacing, EditWidth, EditHeight, hWnd, (HMENU)ID_RIGHTBOUND, g_hInstance, NULL);
		//g_hWndNCount = CreateWindowExA(NULL, "combobox", "g_NCount", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | CBS_DROPDOWN | WS_VSCROLL,
		//	40, EditIndent + k++ * EditSpacing, EditWidth, 1000, hWnd, (HMENU)ID_NCOUNT, g_hInstance, NULL);
		g_hWndNCount = CreateWindowExA(NULL, "edit", "g_NCount", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
			40, EditIndent + k++ * EditSpacing, EditWidth, EditHeight, hWnd, (HMENU)ID_NCOUNT, g_hInstance, NULL);
		g_hWndGoInfinite = CreateWindowExA(NULL, "button", "Toggle Axes", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_CENTER | WS_TABSTOP,
			40, EditIndent + 30 + (k - 1) * EditSpacing, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_GOINFINITE, g_hInstance, NULL);
		g_hWndButton = CreateWindow(TEXT("button"), TEXT("Compute"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
			UIWidth / 2 - 100 / 2, UIHeight - 60 - 20, 100, cyChar * 7 / 4, hWnd, (HMENU)ID_BUTTON, g_hInstance, NULL);

		// Creat raido buttons
		g_hWndLeftCheck = CreateWindowExA(NULL, "button", "", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CENTER | WS_TABSTOP | WS_GROUP,
			10, EditIndent + (k - 3) * EditSpacing + (int)(0.4f * cyChar), cyChar, cyChar, hWnd, (HMENU)ID_LEFTCHECK, g_hInstance, NULL);
		g_hWndRightCheck = CreateWindowExA(NULL, "button", "", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CENTER | WS_TABSTOP,
			10, EditIndent + (k - 2) * EditSpacing + (int)(0.4f * cyChar), cyChar, cyChar, hWnd, (HMENU)ID_RIGHTCHECK, g_hInstance, NULL);
		g_hWndEquation_1_Adv = CreateWindow(TEXT("button"), L"1", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | BS_OWNERDRAW,
			UIWidth - EditHeight - 3, EditIndent + 1 * EditSpacing, EditHeight, EditHeight, hWnd,
			(HMENU)ID_FUNCTION1ADV, g_hInstance, NULL);
		g_hWndEquation_2_Adv = CreateWindow(TEXT("button"), L"2", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | BS_OWNERDRAW,
			UIWidth - EditHeight - 3, EditIndent + 2 * EditSpacing, EditHeight, EditHeight, hWnd,
			(HMENU)ID_FUNCTION2ADV, g_hInstance, NULL);
		g_hWndLeftBoundAdv = CreateWindow(TEXT("button"), L"3", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | BS_OWNERDRAW,
			UIWidth - EditHeight - 3, EditIndent + 3 * EditSpacing, EditHeight, EditHeight, hWnd,
			(HMENU)ID_LEFTBOUNDADV, g_hInstance, NULL);
		g_hWndRightBoundAdv = CreateWindow(TEXT("button"), L"4", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | BS_OWNERDRAW,
			UIWidth - EditHeight - 3, EditIndent + 4 * EditSpacing, EditHeight, EditHeight, hWnd,
			(HMENU)ID_RIGHTBOUNDADV, g_hInstance, NULL);

		// Set combo box text
		for (auto a : SolidMethodDropDownList)
		{
			if (a.empty())
				continue;
			SendMessage(g_hWndMethod, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)a.c_str());
		}
		SendMessage(g_hWndMethod, CB_SETCURSEL, (WPARAM)(g_SolidMethod), NULL);

		//SendMessage(g_hWndNCount, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"infinite");
		//for (UINT i = 1; i <= g_WorldCount; i++)
		//{
		//	std::wstring a;
		//	if (i < 10)
		//		a.append(L"0");
		//	a.append(std::to_wstring(i));
		//	SendMessage(g_hWndNCount, CB_ADDSTRING, (WPARAM)0, (LPARAM)a.c_str());
		//}

		// Set font
		HFONT hFont = CreateFont(cyChar, cxChar, -0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
		SetWindowFont(g_hWndMethod, hFont, FALSE);
		SetWindowFont(g_hWndEquation_1, hFont, FALSE);
		SetWindowFont(g_hWndEquation_2, hFont, FALSE);
		SetWindowFont(g_hWndLeftBound, hFont, FALSE);
		SetWindowFont(g_hWndRightBound, hFont, FALSE);
		SetWindowFont(g_hWndLeftBoundAdv, hFont, FALSE);
		SetWindowFont(g_hWndRightBoundAdv, hFont, FALSE);
		SetWindowFont(g_hWndNCount, hFont, FALSE);
		SetWindowFont(g_hWndGoInfinite, hFont, FALSE);
		SetWindowFont(g_hWndButton, hFont, FALSE);

		UpdateVariables();
		// Variables have to be updated before integration
		Integration();
		ShowWindow(g_hWndGoInfinite, SW_HIDE);
		AdjustControlCoords(hWnd);
		SetFocus(g_hWndDX);
	}

	case WM_INITDIALOG:
	{

		// Subclass the edit control. 
		wpOrigEditProc = (WNDPROC)SetWindowLongPtr(g_hWndEquation_1_Adv, (LONG)EditSubclassProc, GWLP_WNDPROC);
		break;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		int wmNotificationCode = HIWORD(wParam);
		switch (wmNotificationCode)
		{
		case CBN_SELCHANGE:
		{
			AdjustControlCoords(hWnd);
			break;
		}
		default:
			break;
		}

		// Parse the menu selections:
		switch (wmId)
		{
		case ID_BUTTON:
		{
			bool MethodChanged = FALSE;
			char cFunction_1[NumOfChar] = "";
			char cFunction_2[NumOfChar] = "";
			char cLeftBound[NumOfChar] = "";
			char cRightBound[NumOfChar] = "";
			char cNCount[NumOfChar] = "";

			int Method = 0;
			Method = (int)SendMessage(g_hWndMethod, CB_GETCURSEL, NULL, NULL);
			if ((SolidMethod)Method != g_SolidMethod)
			{
				MethodChanged = TRUE;
			}
			MethodChanged = TRUE; // Let it always to be true now because I don't have time to change.
			// Get text from edit controls
			Edit_GetLine(g_hWndEquation_1, NULL, (LPTSTR)&cFunction_1, NumOfChar);
			TcharToChar(cFunction_1);
			Edit_GetLine(g_hWndEquation_2, NULL, (LPTSTR)&cFunction_2, NumOfChar);
			TcharToChar(cFunction_2);
			Edit_GetLine(g_hWndLeftBound, NULL, (LPTSTR)&cLeftBound, NumOfChar);
			TcharToChar(cLeftBound);
			Edit_GetLine(g_hWndRightBound, NULL, (LPTSTR)&cRightBound, NumOfChar);
			TcharToChar(cRightBound);

			
			// Get selection from g_NCount
			std::string s
			Edit_GetLine(g_hWndNCount, NULL, (LPTSTR)&cNCount, NumOfChar);
			TcharToChar(cNCount);
			std::string sNCount(cNCount);
			UINT NCount = std::stoi(sNCount);

			// Get check state from radio buttons
			g_BoundToWhat =
				(BST_CHECKED == SendMessage(g_hWndLeftCheck, BM_GETCHECK, NULL, NULL)) ? BoundToLeft : BoundToRight;

			std::string sFunction_1(cFunction_1);
			std::string sFunction_2(cFunction_2);
			std::string sLeftBound(cLeftBound);
			std::string sRightBound(cRightBound);

			

			/*if (Method == ShellMethod)
			{
				for (UINT i = 0; i < sFunction_1.size(); i++)
				{
					if (sFunction_1[i] == 'x')
					{
						MessageBoxA(NULL, "Please enter the function1 in terms of y", NULL, NULL);
						return 0;
					}
					else if (sFunction_1[i] == 'y')
					{
						sFunction_1[i] = 'x';
					}
				}
				for (UINT i = 0; i < sFunction_2.size(); i++)
				{
					if (sFunction_2[i] == 'x')
					{
						MessageBoxA(NULL, "Please enter the function2 in terms of y", NULL, NULL);
						return 0;
					}
					else if (sFunction_2[i] == 'y')
					{
						sFunction_2[i] = 'x';
					}
				}
			}*/
			if (sFunction_1.find('y') != std::string::npos)
			{
				g_bRotateAlongX = false;
				std::replace(sFunction_1.begin(), sFunction_1.end(), 'y', 'x');
				if (Method == Washer)
				{
					// If function2 is not in terms of y
					if (sFunction_2.find('y') == std::string::npos)
					{
						MessageBoxA(NULL, "Function 1 is in terms of y, but function 2 is not. Please make them agree with each other", NULL, NULL);
						return 0;
					}
					else
					{
						std::replace(sFunction_2.begin(), sFunction_2.end(), 'y', 'x');
					}
				}
			}
			else
			{
				g_bRotateAlongX = true;
			}
			// If method it not washer, don't check function 2
			ErrorCode bValid = (Method == Washer) ? ValidateInput(sFunction_1, sFunction_2, sLeftBound, sRightBound) : 
													ValidateInput(sFunction_1, "1*x", sLeftBound, sRightBound);
			switch (bValid)
			{
			case INPUT_OK:
				SetInputs(sFunction_1, sFunction_2, sLeftBound, sRightBound, NCount);
				SetFocus(g_hWndDX);
				break;

			case InvalidFunction_1:
			case InvalidFunction_2:
			case InvalidLeftBound:
			case InvalidRightBound:
				PopInvalideMessage(bValid);
				return 0;
				break;

			default:
				break;
			}

			if (bValid == INPUT_OK)
			{
				UINT	NumOfVertices;
				GeometryPointers mGeoPointers;
				g_SolidMethod = (SolidMethod)Method;
				g_NCount = NCount;
				switch (g_SolidMethod)
				{
				case Disk:
					if (MethodChanged)
						mGeoPointers = BuildDiskGeometryBuffers(&NumOfVertices, &NumOfIndices_Solid);
					break;
				case Washer:
					mGeoPointers = BuildEntireWasherGeometryBuffers(&NumOfVertices, &NumOfIndices_Solid,
						g_NCount, g_LeftBound, g_RightBound, Expression_1, Expression_2);
					break;
				//case ShellMethod:
				//	mGeoPointers = BuildEntireShellGeometryBuffers(&NumOfVertices, &NumOfIndices_Solid,
				//		g_NCount, g_LeftBound, g_RightBound, Expression_1, g_BoundToWhat);
				//	break;
				case CrossSection_Semicircle:
				case CrossSection_EquilateralTriangle:
				case CrossSection_Square:
					if (MethodChanged)
						mGeoPointers = BuildCrossSectionGeometryBuffers(&NumOfVertices, &NumOfIndices_Solid,
							g_SolidMethod);
					break;
				default:
					break;
				}

				if (!g_bRotateAlongX)
				{
					SwapXAndYVertices(mGeoPointers, NumOfVertices, NumOfIndices_Solid);
				}
				
				if (mGeoPointers.pVertexPointer)
				{
					BuildGeometryBuffers(mGeoPointers, g_pVertexBuffer, g_pIndexBuffer, NumOfVertices, NumOfIndices_Solid);
				}

				//BuildFunctionVertexBuffer();
				Integration();
				InvalidateRect(g_hWndUI, NULL, TRUE);
			}
			break;
		}
		
		case ID_GOINFINITE:
		{
			AxisOn = (AxisOn == FALSE) ? TRUE : FALSE;
			break;
		}

		case ID_FUNCTION1ADV:
		{
			g_pMathInput->spMIC->Show();
			g_pMathInput->TargetHWND = g_hWndEquation_1;
			break;
		}

		case ID_FUNCTION2ADV:
		{
			g_pMathInput->spMIC->Show();
			g_pMathInput->TargetHWND = g_hWndEquation_2;
			break;
		}

		case ID_LEFTBOUNDADV:
		{
			GuessZero(g_hWndLeftBound);
			break;
		}

		case ID_RIGHTBOUNDADV:
		{
			GuessZero(g_hWndRightBound);
			break;
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
	
		return 0;
	}

	case WM_CTLCOLORBTN:
	{
		HDC hdc = (HDC)wParam;
		SetBkColor(hdc, (COLORREF)(COLOR_WINDOW + 1));
		return 0;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HFONT hFont;
		UINT FontHeight = 36;
		UINT FontWidth = 10;
		hFont = CreateFont(FontHeight, FontWidth, -0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
		SelectObject(hdc, hFont);

		//Sets the coordinates for the rectangle in which the text is to be formatted.
		SetTextColor(hdc, RGB(0, 0, 0));

		// Draw captions for controls
		UINT k = 0;
		TextOutA(hdc, 40, EditIndent - FontHeight + EditSpacing * k++, "Method", 6);
		TextOutA(hdc, 40, EditIndent - FontHeight + EditSpacing * k++, "Function 1", 10);
		if (/*(CurrentMethod == ShellMethod) ||*/ (CurrentMethod == Washer))
		{
			TextOutA(hdc, 40, EditIndent - FontHeight + EditSpacing * k++, "Function 2", 10);
		}
		TextOutA(hdc, 40, EditIndent - FontHeight + EditSpacing * k++, "Left Bound", 10);
		TextOutA(hdc, 40, EditIndent - FontHeight + EditSpacing * k++, "Right Bound", 11);
		TextOutA(hdc, 40, EditIndent - FontHeight + EditSpacing * k++, "Number Of Solids", 16);

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));

		// Draw result of integration
		static UINT cxChar = (UINT)(1.4f * LOWORD(GetDialogBaseUnits()));
		static UINT cyChar = (UINT)(1.4f * HIWORD(GetDialogBaseUnits()) * 1.4f);

		hFont = CreateFont(cyChar, cxChar, -0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
		SelectObject(hdc, hFont);

		char Result[50] = "Result = ";
		sprintf_s(Result, 50, "Result = %.4f", IntegrationResult);
		std::string Buffer = std::to_string(IntegrationResult);
		int a = (int)Buffer.find('.');
		TextOutA(hdc, 40, 40 + EditIndent - FontHeight + EditSpacing * k++, Result, 9 + a + 5);
		

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		EndPaint(hWnd, &ps);

		ShowWindow(g_hWndLeftBound, SW_SHOW);
		ShowWindow(g_hWndLeftBoundAdv, SW_SHOW);
		ShowWindow(g_hWndRightBound, SW_SHOW);
		ShowWindow(g_hWndRightBoundAdv, SW_SHOW);
		RECT LeftBoundRect;
		GetWindowRect(g_hWndRightBound, &LeftBoundRect);
		return 0;
	}

	case WM_DRAWITEM:
	{
		if (wParam == ID_FUNCTION1ADV || wParam == ID_FUNCTION2ADV || wParam == ID_LEFTBOUNDADV || wParam == ID_RIGHTBOUNDADV)
		{
			LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
			HWND hwnd = pDIS->hwndItem;
			BITMAP bm;

			HDC hdc = pDIS->hDC;


			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmAdv);

			GetObject(g_hbmAdv, sizeof(bm), &bm);

			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
			return 0;
		}
		break;
					
	}

	case WM_DESTROY:
		// Remove the subclass from the edit control. 
		SetWindowLongPtr(g_hWndEquation_1_Adv, (LONG)wpOrigEditProc, GWLP_WNDPROC);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK DXWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		try {
			InitDevice(hWnd, g_hInstance);
		}
		catch (DxException& e)
		{
			MessageBox(nullptr, e.ToString().c_str(), L"HR Failed In Initializing The Device", MB_OK);
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(hWnd, &mLastMousePos, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		SetFocus(hWnd);
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		SetFocus(hWnd);
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(wParam, &mLastMousePos, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
			&mTheta, &mPhi, &mRadius);
		return 0;

	case WM_MOUSEWHEEL:
		mRadius -= GET_WHEEL_DELTA_WPARAM(wParam) / 120;
		mRadius = Clamp(mRadius, MinRaidus, MaxRaidus);
		return 0;

	case WM_KEYDOWN:
		//OnKeyDown(wParam);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

ATOM RegisterMyClass(HINSTANCE hInstance)
{
	//UI class
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = UIWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = UIClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));
	RegisterClassEx(&wcex);

	WNDCLASSEXW nWcex;
	nWcex.cbSize = sizeof(WNDCLASSEX);
	nWcex.style = CS_HREDRAW | CS_VREDRAW;
	nWcex.lpfnWndProc = DXWndProc;
	nWcex.cbClsExtra = 0;
	nWcex.cbWndExtra = 0;
	nWcex.hInstance = hInstance;
	nWcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	nWcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	nWcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	nWcex.lpszMenuName = nullptr;
	nWcex.lpszClassName = DXClass;
	nWcex.hIconSm = LoadIcon(nWcex.hInstance, MAKEINTRESOURCE(IDI_ICON));
	return	RegisterClassEx(&nWcex);
}

bool InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	//Get screen dimensions
	RECT WorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &WorkArea, NULL);
	UINT ScreenWidth = WorkArea.right;
	UINT ScreenHeight = WorkArea.bottom;

	//Set window dimension
	UINT WindowWidth = ScreenWidth;
	UINT WindowHeight = ScreenHeight;

	//Register window class
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));
	if (!RegisterClassExW(&wcex))
	{
		return FALSE;
	}

	g_hInstance = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, WindowWidth, WindowHeight, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = hWnd; // Store instance handle in our global variable

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
