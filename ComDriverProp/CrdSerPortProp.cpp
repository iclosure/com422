// DriverProp.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "resource.h"
#include <prsht.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include "CrdSerProp.h"
#include <SetupAPI.h>

#pragma comment(lib, "comctl32.lib")

// 
typedef struct _PP_PARAMS_STRUCT
{
	HKEY    drvConfigKey;
	BOOL    drvConfigKeyValid;
	HKEY    svcConfigKey;
	BOOL    svcConfigKeyValid;

	ULONG   timeoutScaling;
	ULONG   statusLines;
	ULONG   activeConnectOnly;

	ULONG   oldTimeoutScaling;
	ULONG   oldStatusLines;
	ULONG   oldActiveConnectOnly;

} PP_PARAMS_STRUCT;

BOOL CALLBACK PortPropDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
UINT CALLBACK PortPropSheetPageCallback(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE lpPropSheetPage);

VOID CheckSettings(HWND hwnd);
VOID ReadConfig(HWND hwnd);
VOID WriteConfig(HWND hwnd);

/////////////////////////////////////////////////////////////////
// Por Properties

// BaudRate
ULONG _g_baudRateTab[] = { 
	50, 75, 300, 600, 1200, 1800, 2400, 4800, 7200, 9600, 
	14400, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 3125000
};

// DataBits
TCHAR* _g_dataBitsTab[] = {
	_T("5"), _T("6"), _T("7"), _T("8")
};

// Parity
TCHAR* _g_parityTab[] = {
	_T("None"), _T("Odd"), _T("Even"), _T("Mark"), _T("Space")
};

// StopBits
TCHAR* _g_stopBitsTab[] = {
	_T("1"), _T("1.5"), _T("2")
};

TCHAR* _g_flowCtrlTab[] = {
	_T("Xon/Xoff"), _T("Hardware"), _T("None")
};

/////////////////////////////////////////////////////////////////

BOOL APIENTRY ComPortPropPageProvider(
	PSP_PROPSHEETPAGE_REQUEST pPropPageRequest,
	LPFNADDPROPSHEETPAGE fAddFunc,
	LPARAM lParam)
{
	//PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData;
	PROPSHEETPAGE propSheetPage;
	HPROPSHEETPAGE hPropSheetPage;
	UINT dialogID;

	if (pPropPageRequest->PageRequested != SPPSR_ENUM_ADV_DEVICE_PROPERTIES)
	{
		HrUdPrint((_T("Add com422 port property page failed! <Check Inf --> EnumPropPages32>\n")));
		return FALSE;
	}

	switch (GetSystemDefaultUILanguage())
	{
// 	case 0x0404:	// МЁНе
// 		dialogID = IDD_COMM_PORT_PROP_EN;
// 		break;
	case 0x0409:	// US
		dialogID = IDD_COMM_PORT_PROP_EN;
		break;
	case 0x0804:	// Chinese Simplify
		dialogID = IDD_COMM_PORT_PROP_EN;
		break;
	default:
		dialogID = IDD_COMM_PORT_PROP_EN;
		break;
	}

	HrUdPrint((_T("Add com422 port property page successfully! -- %d"), 1));

	propSheetPage.dwSize = sizeof(PROPSHEETPAGE);
	propSheetPage.dwFlags = PSP_USECALLBACK | PSP_HASHELP /*| PSP_USEICONID | PSP_USETITLE*/;
	propSheetPage.hInstance = _g_hModule;
	propSheetPage.pszTemplate = MAKEINTRESOURCE(dialogID);
	propSheetPage.pfnDlgProc = PortPropDialogProc;
	propSheetPage.lParam = 0;
	propSheetPage.pfnCallback = PortPropSheetPageCallback;

	hPropSheetPage = CreatePropertySheetPage(&propSheetPage);
	if (hPropSheetPage == NULL)
	{
		return FALSE;
	}

	HrUdPrint((_T("Add com422 port property page successfully! -- %d"), 2));

	if (!(*fAddFunc)(hPropSheetPage, lParam))
	{
		DestroyPropertySheetPage(hPropSheetPage);
		return FALSE;
	}

	HrUdPrint((_T("Add com422 port property page successfully! -- %d"), 3));

	return TRUE;
}

BOOL CALLBACK PortPropDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		ReadConfig(hwnd);

		HWND subHwnd;

		// 
		// BaudRate
		// 
		subHwnd = GetDlgItem(hwnd, IDC_COMBO_BAUDRATE);
		for (int i = 0; i < sizeof(_g_baudRateTab) / sizeof(_g_baudRateTab[0]); i++)
		{
			TCHAR text[20] = { 0 };

			_sntprintf_s(text, 20 * sizeof(TCHAR), _T("%d"), _g_baudRateTab[i]);

			SendMessage(subHwnd, CB_ADDSTRING, 0, (LPARAM)text);
		}

		SendMessage(subHwnd, CB_SETCURSEL, 9, 0);

		// 
		// DataBits
		// 
		subHwnd = GetDlgItem(hwnd, IDC_COMBO_DATABITS);
		for (int i = 0; i < sizeof(_g_dataBitsTab) / sizeof(_g_dataBitsTab[0]); i++)
		{
			SendMessage(subHwnd, CB_ADDSTRING, 0, (LPARAM)_g_dataBitsTab[i]);
		}

		SendMessage(subHwnd, CB_SETCURSEL, 3, 0);

		// 
		// parity
		// 
		subHwnd = GetDlgItem(hwnd, IDC_COMBO_PARITY);
		for (int i = 0; i < sizeof(_g_parityTab) / sizeof(_g_parityTab[0]); i++)
		{
			SendMessage(subHwnd, CB_ADDSTRING, 0, (LPARAM)_g_parityTab[i]);
		}

		SendMessage(subHwnd, CB_SETCURSEL, 0, 0);

		// 
		// StopBits
		// 
		subHwnd = GetDlgItem(hwnd, IDC_COMBO_STOPBITS);
		for (int i = 0; i < sizeof(_g_stopBitsTab) / sizeof(_g_stopBitsTab[0]); i++)
		{
			SendMessage(subHwnd, CB_ADDSTRING, 0, (LPARAM)_g_stopBitsTab[i]);
		}

		SendMessage(subHwnd, CB_SETCURSEL, 0, 0);

		// 
		// FlowCtrl
		// 
		subHwnd = GetDlgItem(hwnd, IDC_COMBO_FLOWCTRL);
		for (int i = 0; i < sizeof(_g_flowCtrlTab) / sizeof(_g_flowCtrlTab[0]); i++)
		{
			SendMessage(subHwnd, CB_ADDSTRING, 0, (LPARAM)_g_flowCtrlTab[i]);
		}

		SendMessage(subHwnd, CB_SETCURSEL, 0, 0);

		return TRUE;
	}
	case WM_COMMAND:
	{
		return TRUE;
	}
	case WM_NOTIFY:
	{
		return TRUE;
	}
	default:
		break;
	}

	return FALSE;
}

UINT CALLBACK PortPropSheetPageCallback(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE lpPropSheetPage)
{
	switch (uMsg)
	{
	case PSPCB_CREATE:
		break;
	case PSPCB_ADDREF:
		break;
	case PSPCB_RELEASE:
		{/*
			PP_PARAMS_STRUCT* pParams;
			pParams = (PP_PARAMS_STRUCT*)lpPropSheetPage->lParam;
			if (pParams->drvConfigKeyValid)
			{
				RegCloseKey(pParams->drvConfigKey);
			}

			if (pParams->svcConfigKeyValid)
			{
				RegCloseKey(pParams->svcConfigKey);
			}

			LocalFree(pParams);*/
		}
		break;
	default:
		break;
	}

	return TRUE;
}

VOID CheckSettings(HWND hwnd)
{

}

VOID ReadConfig(HWND hwnd)
{

}

VOID WriteConfig(HWND hwnd)
{

}
