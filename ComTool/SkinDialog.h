#pragma once

#define LAYOUT_MOVE_NONE	CBCGPStaticLayout::e_MoveTypeNone
#define LAYOUT_MOVE_HORZ	CBCGPStaticLayout::e_MoveTypeHorz
#define LAYOUT_MOVE_VERT	CBCGPStaticLayout::e_MoveTypeVert
#define LAYOUT_MOVE_BOTH	CBCGPStaticLayout::e_MoveTypeBoth

#define LAYOUT_SIZE_NONE	CBCGPStaticLayout::e_SizeTypeNone
#define LAYOUT_SIZE_HORZ	CBCGPStaticLayout::e_SizeTypeHorz
#define LAYOUT_SIZE_VERT	CBCGPStaticLayout::e_SizeTypeVert
#define LAYOUT_SIZE_BOTH	CBCGPStaticLayout::e_SizeTypeBoth

#define RPOINT(rate_x, rate_y)		CPoint(100*rate_x, 100*rate_y)

class CBCGPDialog;

// CSkinDialog

class CSkinDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(CSkinDialog)

	// Construction
public:
	CSkinDialog();
	CSkinDialog(UINT nIDTemplate, CWnd *pParent = NULL);
	CSkinDialog(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL);
	virtual ~CSkinDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	virtual BOOL FinalInitialze();		// 最后的初始化操作
	virtual BOOL DestroyWindow();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void TimerProc() { }

protected:
	enum { SKIN_DLG_TITLE_HEIGHT = 30 };

public:		int  GetTitleHeight() { return SKIN_DLG_TITLE_HEIGHT; }
protected:	void ShowTitle(BOOL value = TRUE) { m_bShowTitle = value; }
	
protected:	void SetTimerPeriod(UINT nMillisecond) { ASSERT(nMillisecond > 0); m_nMillisecond = nMillisecond; }
public:		virtual void SetTimerActive(BOOL bActive = TRUE, UINT nMillisecond = 0); 

protected:	void SetRateY(int value = 100) { m_rateY = value; }
public:		int  GetRateY(void) { return m_rateY; }

private:
	BOOL		m_bShowTitle;			// 是否显示标题栏
	UINT		m_nMillisecond;			// 定时器周期( uints: ms )
	UINT_PTR	m_InternalTimerID;		// 内部定时器ID
	BOOL		m_bAutoStatus;			// 是否自动开启/关闭定时器
	int			m_rateY;				// 窗口高度实际显示比率（百分比，例如60%即为60）（在有输出窗口时有效）
};
