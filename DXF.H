// Dxf.h: interface for the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXF_H__4CD92A21_9DBC_11D4_BF15_0000E8A36431__INCLUDED_)
#define AFX_DXF_H__4CD92A21_9DBC_11D4_BF15_0000E8A36431__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Entity.h"

class CDxf : public CFile  
{
private:
	CDocument*	m_pDoc; // �ĵ�ָ��
	FILE*		fp;		// ����ļ�ָ��
public:
	CDxf();
	virtual ~CDxf();
public:
	// ���DXF�ļ�
	BOOL	OutputDxf(CDocument* pDoc,CString filename);
private:
	// д�ļ�ͷ
	BOOL	WriteHeader();
	// д�ļ�β
	BOOL	WriteTail();
	// дʵ����Ϣ���ú����������͵���дͼԪ��Ϣ�ĺ���
	BOOL	WriteContent();

	// дͼԪ��Ϣ
	BOOL	WriteLine(MLines*	pLine);
	BOOL	WriteRectangle(MRectangle*	pRectangle);
	BOOL	WriteCircle(CCircle*	pCircle);
	BOOL	WriteArc(CArc*	pArc);
};

#endif // !defined(AFX_DXF_H__4CD92A21_9DBC_11D4_BF15_0000E8A36431__INCLUDED_)