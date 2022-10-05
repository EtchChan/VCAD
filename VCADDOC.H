// VCadDoc.h : interface of the CVCadDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCADDOC_H__C161FBAA_7612_11D4_BF0F_0000E8A36431__INCLUDED_)
#define AFX_VCADDOC_H__C161FBAA_7612_11D4_BF0F_0000E8A36431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"

class Position;
class MEntity;

class CVCadDoc : public CDocument
{
private:
	CPoint	 m_basePos;
	CPoint   m_desPos;
	int		 m_nStep;	

protected: // create from serialization only
	CVCadDoc();
	DECLARE_DYNCREATE(CVCadDoc)

// Attributes
public:
	CObList		m_EntityList;  // ͼԪ��������
	CObArray	m_selectArray; // ѡ�� 
	CObArray	m_clpArray;    //������
	MEntity*	m_pPmtEntity;  // ����ѡ�е�ͼԪ��������ʾ��
// Operations
public:
	void SetModifiedFlag(BOOL bModified = TRUE);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCadDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCadDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVCadDoc)
	afx_msg void OnSaveDxf();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void	Draw(CDC *pDC);
	void	RemoveAllSelected(); // ���ѡ��
	BOOL	IsSelected(MEntity* pEntity); // �ж�ĳһͼԪ�����Ƿ�ѡ��
	void	OnMouseMove(UINT nFlags, const Position& pos) ;
	void	OnLButtonDown(UINT nFlags, const Position& pos) ; 
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCADDOC_H__C161FBAA_7612_11D4_BF0F_0000E8A36431__INCLUDED_)