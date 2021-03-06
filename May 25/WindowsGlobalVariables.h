//******************************************************//
//  WindowsGlobalVariable.h by Tianjiao Huang, 2016-2018//
//******************************************************//
#pragma once

#ifndef WINDOWSGLOBALVARIABLES_H
#define WINDOWSGLOBALVARIABLES_H

// Windows IDs
#define ID_DX							000						//DX window ID
#define ID_UI							001						//UI window ID
#define ID_METHOD						100
#define ID_EQUATION_1					101
#define ID_EQUATION_2					102
#define ID_LEFTBOUND					103
#define ID_RIGHTBOUND					104
#define ID_NCOUNT						105
#define ID_GOINFINITE					106

#define ID_BUTTON						200					//Button ID
#define ID_LEFTCHECK					201
#define ID_RIGHTCHECK					202

#define ID_FUNCTION1ADV					203
#define ID_FUNCTION2ADV					204
#define ID_LEFTBOUNDADV					205
#define ID_RIGHTBOUNDADV				206

// Global Variables
extern std::ofstream					LogFileObject;

extern std::array <std::wstring, 6>		SolidMethodDropDownList;
extern POINT							mLastMousePos;

extern UINT								EditIndent;
extern UINT								EditSpacing;
extern UINT								EditLength;

extern SolidMethod						CurrentMethod;

// HWND
extern HINSTANCE						g_hInstance;
extern HWND								g_hWnd;
extern HWND								g_hWndDX;
extern HWND								g_hWndUI;
extern HWND								g_hWndMethod;
extern HWND								g_hWndEquation_1;
extern HWND								g_hWndEquation_1_Adv;
extern HWND								g_hWndEquation_2;
extern HWND								g_hWndEquation_2_Adv;
extern HWND								g_hWndLeftBound;
extern HWND								g_hWndRightBound;
extern HWND								g_hWndLeftBoundAdv;
extern HWND								g_hWndRightBoundAdv;
extern HWND								g_hWndNCount;
extern HWND								g_hWndGoInfinite;
extern HWND								g_hWndButton;
extern HWND								g_hWndLeftCheck;
extern HWND								g_hWndRightCheck;

// Input File
extern float							g_LeftBound;
extern float							g_RightBound;
extern UINT								g_NCount;
extern std::string						Expression_1;
extern std::string						g_UnparsedExpression_1;
extern std::string						Expression_2;
extern std::string						g_UnparsedExpression_2;
extern SolidMethod						g_SolidMethod;
extern BoundToWhat						g_BoundToWhat;
extern bool								g_bRotateAlongX;

// Setting File
extern float							mTheta;
extern float							mPhi;
extern float							mRadius;
extern bool								AxisOn;
extern bool								bMathInput;
extern FPOINT							g_pos;

extern float							IntegrationResult;

// Camera matrix
extern XMMATRIX							g_View;
extern XMMATRIX							g_Projection;
extern XMMATRIX							g_World;
extern XMFLOAT3							g_LightDir;
extern XMMATRIX							g_LightView;
extern XMFLOAT3							mEyePosW;

extern std::wstring						LogFileName;
extern std::wstring						InputFile;
extern std::wstring						SettingFile;

extern class MathInput*					g_pMathInput;
#endif // !1