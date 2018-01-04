#include "stdafx.h"
#include "resource.h"
#include "PortInfoDlg.h"

INT_PTR CALLBACK PortInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{

		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		case IDCANCEL:
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
	}

	return (INT_PTR)FALSE;
}
