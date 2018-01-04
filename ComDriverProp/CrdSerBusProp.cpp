// DriverProp.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "resource.h"
#include <SetupAPI.h>
#include <prsht.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include "CrdSerProp.h"
#include "PortInfoDlg.h"
#include "PortSettingDlg.h"

BOOL CALLBACK BusPropDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
UINT CALLBACK BusPropSheetPageCallback(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE lpPropSheetPage);

BOOL APIENTRY ComBusPropPageProvider(
	PSP_PROPSHEETPAGE_REQUEST pPropPageRequest,
	LPFNADDPROPSHEETPAGE fAddFunc,
	LPARAM lParam)
{
	//PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData;
	PROPSHEETPAGE propSheetPage;
	HPROPSHEETPAGE hPropSheetPage;
	UINT dialogID;

	HR_PRINT_METHOD_BEGIN();

	if (pPropPageRequest->PageRequested != SPPSR_ENUM_ADV_DEVICE_PROPERTIES)
	{
		HrUdPrint((_T("Add com422 bus property page failed! <Check Inf --> EnumPropPages32>\n")));
		return FALSE;
	}

	switch (GetSystemDefaultUILanguage())
	{
// 	case 0x0404:	// МЁНе
// 		dialogID = IDD_COMM_BUS_PROP_ZH;
// 		break;
	case 0x0409:	// US
		dialogID = IDD_COMM_BUS_PROP_EN;
		break;
	case 0x0804:	// Chinese Simplify
		dialogID = IDD_COMM_BUS_PROP_EN;
		break;
	default:
		dialogID = IDD_COMM_BUS_PROP_EN;
		break;
	}

	propSheetPage.dwSize = sizeof(PROPSHEETPAGE);
	propSheetPage.dwFlags = PSP_USECALLBACK | PSP_HASHELP /*| PSP_USEICONID | PSP_USETITLE*/;
	propSheetPage.hInstance = _g_hModule;
	propSheetPage.pszTemplate = MAKEINTRESOURCE(dialogID);
	propSheetPage.pfnDlgProc = BusPropDialogProc;
	propSheetPage.lParam = lParam;
	propSheetPage.pfnCallback = BusPropSheetPageCallback;

	hPropSheetPage = CreatePropertySheetPage(&propSheetPage);
	if (hPropSheetPage == NULL)
	{
		HrUdPrint((_T("CreatePropertySheetPage() failed!\n")));
		return FALSE;
	}

	if (!(*fAddFunc)(hPropSheetPage, lParam))
	{
		DestroyPropertySheetPage(hPropSheetPage);
		HrUdPrint((_T("Add com422 bus property page failed!\n")));
		return FALSE;
	}

	HR_PRINT_METHOD_END();

	return TRUE;
}

BOOL CALLBACK BusPropDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND subHwnd;

		// 
		// ListCtrl - Port
		// 

		subHwnd = GetDlgItem(hWnd, IDC_LIST_PORT);

		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_PORT_INFO:
		{
			DialogBox(_g_hModule, MAKEINTRESOURCE(IDD_PORT_INFO), hWnd, PortInfoDlgProc);
			break;
		}
		case IDC_BUTTON_SETTING:
		{
			DialogBox(_g_hModule, MAKEINTRESOURCE(IDD_PORT_SETTING), hWnd, PortSettingDlgProc);
			break;
		}
		case IDC_BUTTON_HELP:
		{
			MessageBox(hWnd, _T("Help Button"), _T("Port Information"), MB_OK);
			break;
		}
		default:
			break;
		}
		return TRUE;
	}
	case WM_NOTIFY:
	{
		return TRUE;
	}
	case WM_CONTEXTMENU:
	{
		return TRUE;
	}
	case WM_HELP:
	{
		//WinHelp((HWND)((LPHELPINFO)lParam)->hItemHandle, HELP_WM_HELP, (DWORD)helpIds);
		return TRUE;
	}
	case WM_DESTROY:
	{
		return TRUE;
	}
	default:
		break;
	}

	return FALSE;
}

UINT CALLBACK BusPropSheetPageCallback(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE lpPropSheetPage)
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
		break;
	}
	default:
		break;
	}

	return TRUE;
}
