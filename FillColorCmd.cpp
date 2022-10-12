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

#include "Fill.h"

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
	/*
	* \brief ����������ȷ�����
	*/
	case 1:
	{
		m_basePos = m_desPos = pos;

		int i, n;
		for (n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity* pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			FillColor(m_basePos, pEntity->GetColor());
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
		::Prompt("ѡ��������ӵ㣺");
		break;
	default:
		break;
	}
	return 0;
}
// ��������Ҽ�ȡ�����ڽ��еĲ���
int	CFillColor::OnRButtonDown(UINT nFlags, const Position& pos)
{
	m_nStep = 0;
	::Prompt("������");
	return 0;
}

// ����Cancel ����ȡ�����β���
int CFillColor::Cancel()
{
	m_nStep = 0; // ������������Ϊ 0
	g_pDoc->m_selectArray.RemoveAll();
	::Prompt("������"); // �ȴ���ʾ�����͵��������
	return 0;
}

