/**
 * \file FillPatternCmd.cpp
 * \ingroup ModifyCmd->CFill
 * \brief implement class CFillPattern
 * \description: TODO DEV
 *
 * Architect: Yixuan.Chen
 * Owner: TODO DEV
 * Last Reviewer: Yixuan.Chen || 2022-Oct-08 13:07
 */

 /******************************************************************************/
 /*           Include headers of the STL(standard template library)            */
 /******************************************************************************/
#include "stdafx.h"
#include "math.h"

/******************************************************************************/
/*                      Include headers of the component                      */
/******************************************************************************/
#include "Entity.h"
#include "MainFrm.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "Command.h"
#include "ModifyCmd.h"

#include "Fill.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFillPattern::CFillPattern()
	: m_basePos(0, 0), m_desPos(0, 0)
{
	m_nStep = 0;// ��ʼ��������Ϊ 0
}
CFillPattern::~CFillPattern()
{
}
int	CFillPattern::GetType()
{
	return ctFillPattern;
}
int	CFillPattern::OnLButtonDown(UINT nFlags, const Position& pos)
{
	m_nStep++;
	switch (m_nStep)
	{
		/*
		* \brief ����������ȷ�����
		*/
	case 1:
	{
		// ������ӵ�λ�ã��������ο������������������
		m_basePos = m_desPos = pos;
		break;
	}
	case 2:
	{
		m_desPos = pos;

		CDC* pDC = g_pView->GetDC(); // ���������豸����ָ��
		// �������ƶ�ʱ��������µ���Ƥ��
		MLines* pTempLine = new MLines(m_basePos, m_desPos);
		pTempLine->Draw(pDC, dmDrag);
		delete pTempLine;

		int i, n;
		for (n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity* pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			FillPattern(m_basePos, m_desPos, pEntity->GetColor());
			g_pDoc->SetModifiedFlag(TRUE);
		}
		g_pDoc->m_selectArray.RemoveAll();
		m_nStep = 0;
		Cancel();
		break;
	}
	default:
		break;
	}
	return 0;
}

/*
* \brief
*/
int	CFillPattern::OnMouseMove(UINT nFlags, const Position& pos)
{
	// ��һ��̬����nPreRefresh��¼����OnMouseMove״̬ʱ��ˢ�´���
	static	int nPreRefresh = g_nRefresh;
	// ��������bRefresh˵����OnMouseMove�������Ӵ��Ƿ�ˢ��
	BOOL	bRefresh = FALSE;
	// nCurRefresh���ڼ�¼��ǰ��ˢ�´���
	int		nCurRefresh = g_nRefresh;
	// ���nCurRefresh��nPreRefresh����ȣ�˵���Ӵ�����ˢ�¹�
	if (nCurRefresh != nPreRefresh) {
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh;
	}

	switch (m_nStep)
	{
	case 0:
	{
		::Prompt("ѡ��������ӵ㣺");
		break;
	}
	case 1:
	{
		::Prompt("ѡ���������з���");
		Position	prePos, curPos;
		prePos = m_desPos; // �����һ��Ŀ��λ��
		curPos = pos; // �õ���ǰλ��

		CDC* pDC = g_pView->GetDC();
		// ����ڲ��������д���û�б�ˢ�£���Ҫ�����һ�λ��Ƶ���Ƥ��
		if (!bRefresh)
		{
			MLines* pTempLine1 = new MLines(m_basePos, prePos);
			pTempLine1->Draw(pDC, dmDrag);
			delete pTempLine1;
		}
		// �ڵ�ǰλ�û�����Ƥ��
		MLines* pTempLine2 = new MLines(m_basePos, curPos);
		pTempLine2->Draw(pDC, dmDrag);
		delete pTempLine2;

		g_pView->ReleaseDC(pDC);
		m_desPos = pos; // ��Ŀ������Ϊ��ǰλ��
		break;
	}
	default:
		break;
	}
	return 0;
}
// ��������Ҽ�ȡ�����ڽ��еĲ���
int	CFillPattern::OnRButtonDown(UINT nFlags, const Position& pos)
{
	Position	prePos = m_desPos; // �õ���һ�����λ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC();
		// �����һ�����Ƶ���Ƥ��
		MLines* pTempLine = new MLines(m_basePos, prePos);
		pTempLine->Draw(pDC, dmDrag);
		delete	pTempLine;
		g_pView->ReleaseDC(pDC);
	}
	m_nStep = 0;
	::Prompt("ѡ��������ӵ㣺");
	return 0;
}

// ����Cancel ����ȡ�����β���
int CFillPattern::Cancel()
{
	Position	prePos = m_desPos; // �õ���һ�����λ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC();
		// �����һ�����Ƶ���Ƥ��
		MLines* pTempLine = new MLines(m_basePos, prePos);
		pTempLine->Draw(pDC, dmDrag);
		delete	pTempLine;
		g_pView->ReleaseDC(pDC);
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("����"); // �ȴ���ʾ�����͵��������
	return 0;
}

