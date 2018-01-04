// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"


// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
	: m_needResize(TRUE)
{

}

CListCtrlEx::~CListCtrlEx()
{
}

//禁止List控件列与列之间的拖拉  
BOOL CListCtrlEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)  
{  
	HD_NOTIFY *pHDN   =   (HD_NOTIFY*)lParam;   
	switch(((NMHDR*)lParam)->code)  
	{  
	case HDN_BEGINTRACKW:		// 拖动
	case HDN_BEGINTRACKA: 
	case HDN_DIVIDERDBLCLICK:	// 双击
		*pResult = TRUE;  
		return TRUE;
	default:
		break;
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
} 

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
END_MESSAGE_MAP()

// CListCtrlEx message handlers

void CListCtrlEx::AutoSizeColumns()
{
	SetRedraw(FALSE);

	int nColumnCount = GetColumnCount();

	for (int i = 0; i < nColumnCount; i++)
	{
		SetColumnWidth(i, LVSCW_AUTOSIZE);
		int nColumnWidth = GetColumnWidth(i);
		SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		int nHeaderWidth = GetColumnWidth(i);
		SetColumnWidth(i, max(nColumnWidth, nHeaderWidth));
		Sleep(1);
	}

	SetRedraw();
}

int CListCtrlEx::GetColumnCount()
{
	return GetHeaderCtrl()->GetItemCount();
}

void CListCtrlEx::RemoveAllRow()
{
	SetRedraw(FALSE);

	while (DeleteItem(0))
	{
		Sleep(1);
	}

	SetRedraw();
}
