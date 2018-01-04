// ComTool.h : main header file for the ComTool application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "DataContainer.h"

// CComToolApp:
// See ComTool.cpp for the implementation of this class
//

class CComToolApp : public CWinApp,
	public CBCGPWorkspace
{
public:
	CComToolApp();

	// Override from CBCGPWorkspace
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void PreLoadState ();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	static CString GetAppVersion();
	COMM_INFO & GetComInfo() { return m_dataContainer.commInfo(); }
	CDataContainer & GetDataContainer() { return m_dataContainer; }

private:
	CDataContainer		m_dataContainer;
};

extern CComToolApp theApp;