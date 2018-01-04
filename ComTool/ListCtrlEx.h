#pragma once

// CListCtrlEx

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);  

public:
	void AutoSizeColumns();
	int GetColumnCount();
	void RemoveAllRow();
	void EnableResizeCol(BOOL bEnable = TRUE) { 
		m_needResize = bEnable; 
	}
	BOOL IsResizeCol() {
		return m_needResize;
	}

protected:
	DECLARE_MESSAGE_MAP()

private:
	BOOL		m_needResize;
};
