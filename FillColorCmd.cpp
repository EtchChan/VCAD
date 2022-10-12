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
	m_nStep = 0;// 初始化操作步为 0
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
	* \brief 单击鼠标左键确认填充
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
	// 用一静态变量nPreRefresh记录进入OnMouseMove状态时的刷新次数
	static	int nPreRefresh = g_nRefresh;
	// 布尔变量bRefresh说明在OnMouseMove过程中视窗是否被刷新
	BOOL	bRefresh = FALSE;
	// nCurRefresh用于记录当前的刷新次数
	int		nCurRefresh = g_nRefresh;
	// 如果nCurRefresh和nPreRefresh不相等，说明视窗曾被刷新过
	if (nCurRefresh != nPreRefresh) {
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh;
	}

	switch (m_nStep)
	{
	case 0:
		::Prompt("选择填充种子点：");
		break;
	default:
		break;
	}
	return 0;
}
// 单击鼠标右键取消正在进行的操作
int	CFillColor::OnRButtonDown(UINT nFlags, const Position& pos)
{
	m_nStep = 0;
	::Prompt("就绪：");
	return 0;
}

// 调用Cancel 函数取消本次操作
int CFillColor::Cancel()
{
	m_nStep = 0; // 将操作步重置为 0
	g_pDoc->m_selectArray.RemoveAll();
	::Prompt("就绪："); // 等待提示新类型的命令操作
	return 0;
}

