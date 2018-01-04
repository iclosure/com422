// ComToolDoc.h : interface of the CComToolDoc class
//


#pragma once


class CComToolDoc : public CDocument
{
protected: // create from serialization only
	CComToolDoc();
	DECLARE_DYNCREATE(CComToolDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CComToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


