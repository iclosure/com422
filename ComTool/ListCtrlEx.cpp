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

//��ֹList�ؼ�������֮�������  
BOOL CListCtrlEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)  
{  
	HD_NOTIFY *pHDN   =   (HD_NOTIFY*)lParam;   
	switch(((NMHDR*)lParam)->code)  
	{  
	case HDN_BEGINTRACKW:		// �϶�
	case HDN_BEGINTRACKA: 
	case HDN_DIVIDERDBLCLICK:	// ˫��
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
