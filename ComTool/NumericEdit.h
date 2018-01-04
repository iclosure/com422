#pragma once


// CNumericEdit

class CNumericEdit : public CEdit
{
	DECLARE_DYNAMIC(CNumericEdit)

public:
	CNumericEdit();
	virtual ~CNumericEdit();

public:
	void SetRadix(int radix);
	int GetRadix() { return m_radix; }
	void SetData(DWORD value);
	DWORD GetData();

protected:
	afx_msg void OnEnChange();

	DECLARE_MESSAGE_MAP()

private:
	UINT		m_radix;
};
