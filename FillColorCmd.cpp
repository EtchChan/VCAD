/**
 * \file FillColorCmd.cpp
 * \ingroup ModifyCmd->CFill
 * \brief implement class CFillColor
 * \description: TODO DEV
 *
 * Architect: Yixuan.Chen
 * Owner: TODO DEV
 * Last Reviewer: Yixuan.Chen || 2022-Oct-05 14:24
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFillColor::CFillColor()
	: m_basePos(0, 0), m_desPos(0, 0)
{
	m_nStep = 0;// ��ʼ��������Ϊ 0
}
CFillColor::~CFillColor()
{
}
int	CFillColor::GetType()
{
	return ctFillColor;
}
int	CFillColor::OnLButtonDown(UINT nFlags, const Position& pos)
{
	m_nStep++;
	switch (m_nStep)
	{
	case 1:
		// ��һ�ε���������ʱ�õ�����λ�ã��������趨Ŀ��λ��
		m_basePos = m_desPos = pos;
		::Prompt("�������ƶ���Ŀ��㣺��������Ҽ�ȡ��");
		break;
	case 2:
	{
		m_desPos = pos;
		CDC* pDC = g_pView->GetDC(); // ���������豸����ָ��
		// �������ƶ�ʱ��������µ���Ƥ��
		MLines* pTempLine = new MLines(m_basePos, m_desPos);
		pTempLine->Draw(pDC, dmDrag);
		delete pTempLine;
		// ��ѡ���е�ͼԪ�ƶ���Ŀ��λ�ò����л���
		int i, n;
		for (n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity* pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			pEntity->Draw(pDC, dmInvalid); // ���ԭ��λ���ϵ�ͼԪ
			//pEntity->Mirror(m_basePos, m_desPos); // ��ͼԪ�ƶ���Ŀ��λ��
			//pEntity->Draw(pDC,dmNormal); // ��Ŀ��λ���ϻ���ͼԪ
			MEntity* pNewEnt = pEntity->Copy();
			pNewEnt->Mirror(m_basePos, m_desPos); // ��ͼԪ�ƶ���Ŀ��λ��
			pNewEnt->Draw(pDC, dmNormal); // ��Ŀ��λ���ϻ���ͼԪ
			g_pDoc->SetModifiedFlag(TRUE); // ��־�ĵ������ѱ��޸�
			pNewEnt->m_nOperationNum = g_pView->m_nCurrentOperation;
			pEntity->Append(pNewEnt);
		}
		g_pDoc->m_selectArray.RemoveAll(); //  ���ѡ��
		//			g_pDoc->SetModifiedFlag(TRUE); // ��־�ĵ������ѱ��޸�
		g_pView->ReleaseDC(pDC); // �ͷ�������豸����ָ��
		m_nStep = 0;
		break;
	}
	default:
		break;
	}
	return 0;
}
int	CFillColor::OnMouseMove(UINT nFlags, const Position& pos)
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
		::Prompt("�����뾵��ĵڶ��㣺");
		break;
	case 1:
	{
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
		// ���ݵ�ǰλ�ø���ѡ��ͼԪ��ʵʱλ��
		int i, n;
		for (n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity* pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			pEntity->Draw(pDC, dmSelect);
			// ����ڲ��������д���û�б�ˢ�£���Ҫ�����һ��λ���ϻ��Ƶ�ͼԪ
			if (!bRefresh)
			{
				// �����һ��λ���ϻ��Ƶ�ͼԪ
				MEntity* pCopyEntity1 = pEntity->Copy(); // �õ�ͼԪ�Ŀ���
				pCopyEntity1->Mirror(m_basePos, prePos); // �������ƶ�����һ��λ��
				pCopyEntity1->Draw(pDC, dmDrag); // ������һ��λ�öԿ��������ػ�
				delete pCopyEntity1; // ɾ����ʱ����
			}
			// �ڵ�ǰλ���ϻ���ͼԪ
			MEntity* pCopyEntity2 = pEntity->Copy();// �õ�ͼԪ�Ŀ���
			pCopyEntity2->Mirror(m_basePos, curPos);// �������ƶ�����ǰλ��
			pCopyEntity2->Draw(pDC, dmDrag);	// �Ե�ǰλ�û��ƿ���
			delete pCopyEntity2; // ɾ����ʱ���� 
		}
		g_pView->ReleaseDC(pDC);
		m_desPos = pos; // ��Ŀ������Ϊ��ǰλ��
	}
	default:
		break;
	}
	return 0;
}
// ��������Ҽ�ȡ�����ڽ��еĲ���
int	CFillColor::OnRButtonDown(UINT nFlags, const Position& pos)
{
	Position	prePos = m_desPos; // �õ���һ�����λ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC();
		// �����һ�����Ƶ���Ƥ��
		MLines* pTempLine = new MLines(m_basePos, prePos);
		pTempLine->Draw(pDC, dmDrag);
		delete	pTempLine;

		int i, n;
		for (n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity* pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			// �����һ�λ��Ƶ���ʱ����
			MEntity* pCopyEntity = pEntity->Copy();
			pCopyEntity->Mirror(m_basePos, prePos);
			pCopyEntity->Draw(pDC, dmDrag);
			delete		pCopyEntity;
			// ���»���ѡ���еĶ���
			pEntity->Draw(pDC, dmSelect); // redraw the selected entity
		}
		g_pView->ReleaseDC(pDC); // don't forget this
	}
	m_nStep = 0;
	::Prompt("�����뾵��ĵ�һ�㣺");
	return 0;
}
// ����Cancel ����ȡ�����β���
int CFillColor::Cancel()
{
	Position	prePos = m_desPos; // �õ���һ�����λ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC();
		// �����һ�����Ƶ���Ƥ��
		MLines* pTempLine = new MLines(m_basePos, prePos);
		pTempLine->Draw(pDC, dmDrag);
		delete	pTempLine;

		int i, n;
		for (n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity* pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			// �����һ�λ��Ƶ���ʱ����
			MEntity* pCopyEntity = pEntity->Copy();
			pCopyEntity->Mirror(m_basePos, prePos);
			pCopyEntity->Draw(pDC, dmDrag);
			delete		pCopyEntity;
			// ���»���ѡ���еĶ���
			pEntity->Draw(pDC, dmSelect); // redraw the selected entity
		}
		g_pView->ReleaseDC(pDC); // don't forget this
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("����"); // �ȴ���ʾ�����͵��������
	return 0;
}
