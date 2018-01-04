// ComToolDoc.cpp : implementation of the CComToolDoc class
//

#include "stdafx.h"
#include "ComTool.h"

#include "ComToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CComToolDoc

IMPLEMENT_DYNCREATE(CComToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CComToolDoc, CDocument)
END_MESSAGE_MAP()

// CComToolDoc construction/destruction

CComToolDoc::CComToolDoc()
{
	// TODO: add one-time construction code here

}

CComToolDoc::~CComToolDoc()
{
}

BOOL CComToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CComToolDoc serialization

void CComToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CComToolDoc diagnostics

#ifdef _DEBUG
void CComToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CComToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CComToolDoc commands
