// OutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ComTool.h"
#include "OutputDlg.h"
#include "UserWinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COutputDlg dialog

IMPLEMENT_DYNAMIC(COutputDlg, CSkinDialog)

COutputDlg::COutputDlg(CWnd* pParent /*=NULL*/)
	: CSkinDialog(COutputDlg::IDD, pParent)
{

}

COutputDlg::~COutputDlg()
{
}

void COutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TX_DATA, m_listCtrl[CHANNEL_INDEX_TX]);
	DDX_Control(pDX, IDC_LIST_RX_DATA, m_listCtrl[CHANNEL_INDEX_RX]);
	DDX_Control(pDX, IDC_REDIT_TX_LOG, m_rEditLog[CHANNEL_INDEX_TX]);
	DDX_Control(pDX, IDC_REDIT_RX_LOG, m_rEditLog[CHANNEL_INDEX_RX]);
}

BOOL COutputDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	// �Զ���������
	CBCGPStaticLayout* layout = (CBCGPStaticLayout*) GetLayout();
	if (layout != NULL)
	{
		layout->AddAnchor(IDC_GROUP_TX, LAYOUT_MOVE_NONE, LAYOUT_SIZE_BOTH, RPOINT(  1, 1), RPOINT(1/2, 1));
		layout->AddAnchor(IDC_GROUP_RX, LAYOUT_MOVE_HORZ, LAYOUT_SIZE_BOTH, RPOINT(1/2, 1), RPOINT(1/2, 1));

		//! IDC_GROUP_TX
		layout->AddAnchor(IDC_REDIT_TX_LOG,  LAYOUT_MOVE_NONE, LAYOUT_SIZE_HORZ, RPOINT(1, 1), RPOINT(1/2, 1));
		layout->AddAnchor(IDC_LIST_TX_DATA,  LAYOUT_MOVE_NONE, LAYOUT_SIZE_BOTH, RPOINT(1, 1), RPOINT(1/2, 1));
		layout->AddAnchor(IDC_LABEL_TX_DATA_FORMAT,LAYOUT_MOVE_VERT, LAYOUT_SIZE_NONE, RPOINT(1, 1));
		layout->AddAnchor(IDC_COMBO_TX_DATA_FORMAT,LAYOUT_MOVE_VERT, LAYOUT_SIZE_NONE, RPOINT(1, 1));
		layout->AddAnchor(IDC_LABEL_TX_DATA_SPLIT, LAYOUT_MOVE_VERT, LAYOUT_SIZE_NONE, RPOINT(1, 1));
		layout->AddAnchor(IDC_COMBO_TX_DATA_SPLIT, LAYOUT_MOVE_VERT, LAYOUT_SIZE_NONE, RPOINT(1, 1));
		layout->AddAnchor(IDC_BN_TX_CLEAR_CACHE,   LAYOUT_MOVE_BOTH, LAYOUT_SIZE_NONE, RPOINT(1/2, 1));

		//! IDC_GROUP_RX
		layout->AddAnchor(IDC_REDIT_RX_LOG,  LAYOUT_MOVE_HORZ, LAYOUT_SIZE_HORZ, RPOINT(1/2, 1/2), RPOINT(1/2, 1));
		layout->AddAnchor(IDC_LIST_RX_DATA,  LAYOUT_MOVE_HORZ, LAYOUT_SIZE_BOTH, RPOINT(1/2,   1), RPOINT(1/2, 1));
		layout->AddAnchor(IDC_LABEL_RX_DATA_FORMAT,LAYOUT_MOVE_BOTH, LAYOUT_SIZE_NONE, RPOINT(1/2, 1));
		layout->AddAnchor(IDC_COMBO_RX_DATA_FORMAT,LAYOUT_MOVE_BOTH, LAYOUT_SIZE_NONE, RPOINT(1/2, 1));
		layout->AddAnchor(IDC_LABEL_RX_DATA_SPLIT, LAYOUT_MOVE_BOTH, LAYOUT_SIZE_NONE, RPOINT(1/2, 1));
		layout->AddAnchor(IDC_COMBO_RX_DATA_SPLIT, LAYOUT_MOVE_BOTH, LAYOUT_SIZE_NONE, RPOINT(1/2, 1));
		layout->AddAnchor(IDC_BN_RX_CLEAR_CACHE,   LAYOUT_MOVE_BOTH, LAYOUT_SIZE_NONE, RPOINT(1, 1));
	}

	//! 
	for (int i = 0; i < CHANNEL_INDEX_COUNT; i++)
	{
		m_rEditLog[i].SetBackgroundColor(FALSE, RGB(230, 230, 230));
	}

	//InitializeGridCtrl();

	// ��ʼ������
	m_cfms[CFM_INDEX_PATH].cbSize = sizeof(m_cfms[CFM_INDEX_PATH]);
	m_cfms[CFM_INDEX_PATH].dwMask = CFM_COLOR | CFM_BOLD | CFM_SIZE | CFM_FACE;
	m_cfms[CFM_INDEX_PATH].dwEffects = CFM_BOLD;
	lstrcpy(m_cfms[CFM_INDEX_PATH].szFaceName, _T("Segoe UI"));
	m_cfms[CFM_INDEX_PATH].wWeight = FW_NORMAL;
	m_cfms[CFM_INDEX_PATH].yHeight = 200;
	m_cfms[CFM_INDEX_PATH].crTextColor = RGB(100, 64, 128);

	m_cfms[CFM_INDEX_CMD].cbSize = sizeof(m_cfms[CFM_INDEX_CMD]);
	m_cfms[CFM_INDEX_CMD].dwMask = CFM_COLOR/* | CFM_BOLD*/ | CFM_SIZE | CFM_FACE;
	m_cfms[CFM_INDEX_CMD].dwEffects = CFM_BOLD | CFM_SHADOW;
	lstrcpy(m_cfms[CFM_INDEX_CMD].szFaceName, _T("Segoe UI"));
	m_cfms[CFM_INDEX_CMD].wWeight = FW_BOLD;
	m_cfms[CFM_INDEX_CMD].yHeight = 200;
	m_cfms[CFM_INDEX_CMD].crTextColor = RGB(0, 0, 250);

	m_cfms[CFM_INDEX_ANS].cbSize = sizeof(m_cfms[CFM_INDEX_ANS]);
	m_cfms[CFM_INDEX_ANS].dwMask = CFM_COLOR/* | CFM_BOLD*/ | CFM_SIZE | CFM_FACE;
	m_cfms[CFM_INDEX_ANS].dwEffects = CFM_BOLD;
	lstrcpy(m_cfms[CFM_INDEX_ANS].szFaceName, _T("Segoe UI"));
	m_cfms[CFM_INDEX_ANS].wWeight = FW_NORMAL;
	m_cfms[CFM_INDEX_ANS].yHeight = 180;
	m_cfms[CFM_INDEX_ANS].crTextColor = RGB(0x00, 0x00, 0xA2);

	m_cfms[CFM_INDEX_TIMEOUT].cbSize = sizeof(m_cfms[CFM_INDEX_TIMEOUT]);
	m_cfms[CFM_INDEX_TIMEOUT].dwMask = CFM_COLOR | CFM_BOLD | CFM_SIZE | CFM_FACE;
	m_cfms[CFM_INDEX_TIMEOUT].dwEffects = CFM_BOLD;
	lstrcpy(m_cfms[CFM_INDEX_TIMEOUT].szFaceName, _T("Segoe UI"));
	m_cfms[CFM_INDEX_TIMEOUT].wWeight = CFM_ITALIC;
	m_cfms[CFM_INDEX_TIMEOUT].yHeight = 200;
	m_cfms[CFM_INDEX_TIMEOUT].crTextColor = RGB(255, 35, 20);

	m_cfms[CFM_INDEX_ERROR].cbSize = sizeof(m_cfms[CFM_INDEX_ERROR]);
	m_cfms[CFM_INDEX_ERROR].dwMask = CFM_COLOR | CFM_BOLD | CFM_SIZE | CFM_FACE;
	m_cfms[CFM_INDEX_ERROR].dwEffects = CFM_BOLD;
	lstrcpy(m_cfms[CFM_INDEX_ERROR].szFaceName, _T("Segoe UI"));
	m_cfms[CFM_INDEX_ERROR].wWeight = CFM_ITALIC;
	m_cfms[CFM_INDEX_ERROR].yHeight = 200;
	m_cfms[CFM_INDEX_ERROR].crTextColor = RGB(255, 0, 0);

	//! 
	COMM_INFO & commInfo = theApp.GetComInfo();
	((CComboBox*)GetDlgItem(IDC_COMBO_RX_DATA_FORMAT))->SetCurSel(commInfo.RX.DataFormat);

	// ����������ʾ��ʱ������
	SetTimer(TIMER_TX_ID, TIMER_TX_PERIOD, NULL);
	SetTimer(TIMER_RX_ID, TIMER_RX_PERIOD, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

VOID COutputDlg::InitializeGridCtrl()
{
	for (int i = 0; i < CHANNEL_INDEX_COUNT; i++)
	{
		// ��ʼ���б�ؼ�
		m_listCtrl[i].SetBkColor(RGB(0x8F, 0xE8, 0xE6));
		m_listCtrl[i].SetTextColor(RGB(64, 64, 255));
		DWORD style = m_listCtrl[i].GetExtendedStyle();
		style |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER;
		m_listCtrl[i].SetExtendedStyle(style);
		//m_listCtrl[i].GetHeaderCtrl()->EnableWindow(FALSE);	// ��ֹ�п��϶�
		m_listCtrl[i].InsertColumn(0, _T("#"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(1, _T("ϵͳʱ��"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(2, _T("ʱ���ʶ"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(3, _T("ͨ��"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(4, _T("���䷽��"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(5, _T("ID"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(6, _T("֡����"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(7, _T("֡��ʽ"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(8, _T("֡����"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].InsertColumn(9, _T("����(HEX)"), LVCFMT_CENTER, 0, 0);
		m_listCtrl[i].AutoSizeColumns();
		m_listCtrl[i].EnableResizeCol();
	}
}

BEGIN_MESSAGE_MAP(COutputDlg, CSkinDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_RX_DATA_FORMAT, &ThisClass::OnCbnSelchangeComboRxDataFormat)
	ON_BN_CLICKED(IDC_BN_RX_CLEAR_CACHE, &ThisClass::OnBnClickedBnRxClearCache)
	ON_MESSAGE(WM_USER_VSCROLLEND, &ThisClass::OnUserVScrollEnd)
	ON_MESSAGE(WM_USER_SAVE_TXT, &ThisClass::OnUserSaveTxt)
	ON_MESSAGE(WM_USER_CLEAN_TXT, &ThisClass::OnUserCleanTxt)
	ON_MESSAGE(WM_USER_CLEAN_REPORT, &ThisClass::OnUserCleanReport)
	ON_MESSAGE(WM_USER_START_RECV, &ThisClass::OnUserStartRecv)
	ON_MESSAGE(WM_USER_DISP_ANS, &ThisClass::OnUserDispAns)
	ON_MESSAGE(WM_USER_TIMEOUT, &ThisClass::OnUserTimeout)
	ON_MESSAGE(WM_USER_NOTIMEOUT, &ThisClass::OnUserNoTimeout)
	ON_MESSAGE(WM_USER_ANSERROR, &ThisClass::OnUserAnsError)
END_MESSAGE_MAP()

// COutputDlg message handlers

void COutputDlg::OnTimer(UINT_PTR nIDEvent)
{
}

void COutputDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
}

void COutputDlg::OnCbnSelchangeComboRxDataFormat()
{
	theApp.GetComInfo().RX.DataFormat =
		((CComboBox*)GetDlgItem(IDC_COMBO_RX_DATA_FORMAT))->GetCurSel();

	PTCHAR format[] = { _T("����(BIN)"), _T("����(OCT)"), _T("����(DEC)"), _T("����(HEX)") };
	LVCOLUMN column;
	column.mask = LVCF_TEXT;
	column.pszText = format[theApp.GetComInfo().RX.DataFormat];
	m_listCtrl[CHANNEL_INDEX_RX].SetColumn(9, &column);
	m_listCtrl[CHANNEL_INDEX_RX].AutoSizeColumns();
	m_listCtrl[CHANNEL_INDEX_RX].EnableResizeCol(TRUE);
}

void COutputDlg::OnBnClickedBnRxClearCache()
{
	//! �������Ϊæ״̬
	SetCursor(::LoadCursor(NULL, IDC_WAIT));

	//theApp.GetDataContainer().resetCountRx();
	//SendMessage(WM_USER_CLEAN_TXT, CHANNEL_INDEX_RX);

	//
	m_listCtrl[CHANNEL_INDEX_RX].RemoveAllRow();

	//! �ָ����״̬
	SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

/// ��Ϣ��	WM_USER_VSCROLLEND
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	�������༭�����һ��
/// ������	wParam, 0, 1, 2, 3, ..., CMD_THREAD_NUM
///			lParam, ��Ч
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserVScrollEnd(WPARAM wParam, LPARAM lParam)
{
	return m_rEditLog[wParam].SendMessage(WM_VSCROLL, SB_BOTTOM);
	return 0;
}

/// ��Ϣ��	WM_USER_SAVE_TXT
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	����༭���ı�
/// ������	wParam, �ļ����λ���ַ���
///			lParam, ��Ч
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserSaveTxt(WPARAM wParam, LPARAM lParam)
{
	ASSERT(wParam != NULL);

	CString* saveDir = reinterpret_cast<CString*>(wParam);
	CString* fileName = reinterpret_cast<CString*>(lParam);
	ASSERT(saveDir != NULL);
	ASSERT(fileName != NULL);

	if (! PathIsDirectory(saveDir->GetBuffer()))	// �ж��ļ���log�Ƿ����
	{
		// �����ڣ��򴴽�
		if (! ::CreateDirectory(saveDir->GetBuffer(), NULL))
		{
			AfxMessageBox(_T("Ȩ�޲�����\n���Թ���Աģʽ�����������ѡ�����Ϊ�˵��ı�洢λ��"));
		}
	}

	// ����༭���ı�
	CFile file;
	CString path = *saveDir + *fileName;
	if(!file.Open((*saveDir + *fileName).GetBuffer(), CFile::modeCreate| CFile::modeWrite))
	{
		ASSERT(FALSE);

		CString text;
		text.Format(_T("�ļ�\n%s\n����ʧ��!"), fileName);
		BCGPMessageBox(text, MB_ICONERROR);
		return -1;
	}
	
	// ��ȡ�༭���ı�������
	CString strText(_T("\r=========================== RECV ===========================\r\r"));
	m_rEditLog[CHANNEL_INDEX_RX].SetSel(0, -1);
	// Dump the selection text only if it contains at least one text character.
	if (m_rEditLog[CHANNEL_INDEX_RX].GetSelectionType() & (SEL_TEXT | SEL_MULTICHAR))
	{
		strText += m_rEditLog[CHANNEL_INDEX_RX].GetSelText();
	}

#ifdef UNICODE
	file.Write("\xFF\xFE", 2);	// Unicode
#endif	// UNICODE
	file.Write(strText.GetBuffer(), strText.GetLength() * sizeof(TCHAR));
	file.Close();

	return 0;
}

/// ��Ϣ��	WM_USER_CLEAN_TXT
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	����༭���ı�
/// ������	wParam, ��Ч
///			lParam, ��Ч
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserCleanTxt(WPARAM wParam, LPARAM lParam)
{
	ASSERT(CHANNEL_INDEX_TX<=wParam && wParam<=CHANNEL_INDEX_RX);
	m_rEditLog[wParam].SetSel(0, -1);
	m_rEditLog[wParam].SetReadOnly(FALSE);
	m_rEditLog[wParam].Clear();
	m_rEditLog[wParam].SetReadOnly();

	return 0;
}

/// ��Ϣ��	WM_USER_CLEAN_REPORT
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	�����������
/// ������	wParam, ��Ч
///			lParam, ��Ч
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserCleanReport(WPARAM wParam, LPARAM lParam)
{
	ASSERT(CHANNEL_INDEX_TX <= wParam && wParam <= CHANNEL_INDEX_RX);
	
	if (CHANNEL_INDEX_TX == wParam) {
		ASSERT(FALSE);
	} else {
		OnBnClickedBnRxClearCache();
	}

	return 0;
}

/// ��Ϣ��	WM_USER_START_RECV
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	����Ӧ����Ϣ��ʾ
/// ������	wParam, ͨ���ţ�0:A; 1:B; 2:C.
///			lParam, WM_ANS_LPARAM
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserStartRecv(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

/// ��Ϣ��	WM_USER_DISP_ANS
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	Ӧ���ַ���.��ʾ
/// ������	wParam, ͨ���ţ�0:A; 1:B; 2:C.
///			lParam, WM_ANS_LPARAM
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserDispAns(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

/// ��Ϣ��	WM_USER_TIMEOUT
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	Ӧ��ʱ����
/// ������	wParam, ͨ���ţ�0:A; 1:B; 2:C.
///			lParam, WM_ANS_LPARAM
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserTimeout(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

/// ��Ϣ��	WM_USER_NOTIMEOUT
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	Ӧ��ʱ����
/// ������	wParam, ͨ���ţ�0:A; 1:B; 2:C.
///			lParam, WM_ANS_LPARAM
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserNoTimeout(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

/// ��Ϣ��	WM_USER_ANSERROR
/// ���ࣺ	�û��Զ�����Ϣ
/// ���ܣ�	Ӧ�������
/// ������	wParam, ͨ���ţ�0:A; 1:B; 2:C.
///			lParam, WM_ANS_LPARAM
/// ���أ�	��Ч
LRESULT	COutputDlg::OnUserAnsError(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

// ��༭������ı�

void COutputDlg::TextOut(UINT index, const char* text, CFM_INDEX cfm_index)
{
	ASSERT(text != NULL);
	this->TextOut(index, CString(text), cfm_index);
}

void COutputDlg::TextOut(UINT index, std::string text, CFM_INDEX cfm_index)
{
	this->TextOut(index, CString(text.c_str()), cfm_index);
}

void COutputDlg::TextOut(UINT index, CString text, CFM_INDEX cfm_index)
{
	ASSERT(! text.IsEmpty());
	ASSERT(0<=cfm_index && cfm_index<CFM_INDEX_COUNT);
	ASSERT(CHANNEL_INDEX_TX<=index && index<=CHANNEL_INDEX_RX);
	
	m_rEditLog[index].SetSel(-1, -1);
	m_rEditLog[index].SetSelectionCharFormat(m_cfms[cfm_index]);
	m_rEditLog[index].ReplaceSel(text);
	m_rEditLog[index].SendMessage(WM_VSCROLL, SB_BOTTOM);
}

void COutputDlg::LogOut(UINT index, CString text, CFM_INDEX cfm_index/* = CFM_INDEX_ANS*/)
{
	ASSERT(! text.IsEmpty());
	ASSERT(0<=cfm_index && cfm_index<CFM_INDEX_COUNT);
	ASSERT(CHANNEL_INDEX_TX<=index && index<=CHANNEL_INDEX_RX);

	SYSTEMTIME time;
	GetLocalTime(&time);
	CString timeStr;
	timeStr.Format(_T("[%02d/%02d %02d:%02d:%02d]: "), time.wMonth, 
		time.wDay,time.wHour, time.wMinute, time.wSecond);

	m_rEditLog[index].SetSel(-1, -1);
	m_rEditLog[index].SetSelectionCharFormat(m_cfms[cfm_index]);
	m_rEditLog[index].ReplaceSel(timeStr + text + _T("\n"));
	m_rEditLog[index].SendMessage(WM_VSCROLL, SB_BOTTOM);
}

void COutputDlg::DisableReportTabResizeCol(UINT index)
{
	ASSERT(CHANNEL_INDEX_TX<=index && index<=CHANNEL_INDEX_RX);

	m_listCtrl[index].EnableResizeCol(FALSE);
}
