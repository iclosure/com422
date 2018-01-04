// NumericEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NumericEdit.h"
#include <string>

// CNumericEdit

IMPLEMENT_DYNAMIC(CNumericEdit, CEdit)

CNumericEdit::CNumericEdit()
	: m_radix(10)
{

}

CNumericEdit::~CNumericEdit()
{
}

BEGIN_MESSAGE_MAP(CNumericEdit, CEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, &ThisClass::OnEnChange)
END_MESSAGE_MAP()


// CNumericEdit message handlers

void CNumericEdit::OnEnChange()
{
	CString text;

	GetWindowText(text);
	if (text.IsEmpty())
	{
		SetWindowText(_T("0"));
	}
	else if (text == _T("0"))
	{
		return ;
	}
	else
	{
		CString newText = text;
		newText.TrimLeft(_T("0"));
		if (newText.IsEmpty())
		{
			SetWindowText(_T("0"));
		}
		else if (newText != text)
		{
			SetWindowText(newText);
		}
	}
}

void CNumericEdit::SetRadix(int radix)
{
	ASSERT((radix == 2) || (radix == 8) || (radix == 10) || (radix == 16));

	DWORD value = GetData();

	m_radix = radix;

	SetData(value);
}

void CNumericEdit::SetData(DWORD value)
{
	char buff[33] = {0};

	_ultoa_s(value, buff, 33, m_radix);
	std::string str1(buff);

// 	switch (m_radix)
// 	{
// 	case 2:
// 		str1.insert(0, max(0, (8 - str1.length())), '0');
// 		break;
// 	case 8:
// 		str1.insert(0, max(0, (8 - str1.length())), '0');
// 		break;
// 	case 10:
// 		str1.insert(0, max(0, (8 - str1.length())), '0');
// 		break;
// 	case 16:
// 		str1.insert(0, max(0, (8 - str1.length())), '0');
// 		break;
// 	default:
// 		return;
// 	}

	SetWindowText(CString(str1.c_str()));
}

DWORD CNumericEdit::GetData()
{
	CString text;

	GetWindowText(text);
	if (text.IsEmpty())
	{
		return 0;
	}

	return _tcstoul(text, 0, m_radix);
}
