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
	m_nStep = 0;// 初始化操作步为 0
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
		* \brief 单击鼠标左键确认填充
		*/
	case 1:
	{
		// 获得种子点位置，并用作参考基点设置填充纹理方向
		m_basePos = m_desPos = pos;
		break;
	}
	case 2:
	{
		m_desPos = pos;

		CDC* pDC = g_pView->GetDC(); // 获得视类的设备环境指针
		// 清除鼠标移动时最后遗留下的橡皮线
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
	{
		::Prompt("选择填充种子点：");
		break;
	}
	case 1:
	{
		::Prompt("选择纹理排列方向：");
		Position	prePos, curPos;
		prePos = m_desPos; // 获得上一个目标位置
		curPos = pos; // 得到当前位置

		CDC* pDC = g_pView->GetDC();
		// 如果在操作过程中窗口没有被刷新，则要清除上一次绘制的橡皮线
		if (!bRefresh)
		{
			MLines* pTempLine1 = new MLines(m_basePos, prePos);
			pTempLine1->Draw(pDC, dmDrag);
			delete pTempLine1;
		}
		// 在当前位置绘制橡皮线
		MLines* pTempLine2 = new MLines(m_basePos, curPos);
		pTempLine2->Draw(pDC, dmDrag);
		delete pTempLine2;

		g_pView->ReleaseDC(pDC);
		m_desPos = pos; // 将目标设置为当前位置
		break;
	}
	default:
		break;
	}
	return 0;
}
// 单击鼠标右键取消正在进行的操作
int	CFillPattern::OnRButtonDown(UINT nFlags, const Position& pos)
{
	Position	prePos = m_desPos; // 得到上一个鼠标位置
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC();
		// 清除上一个绘制的橡皮线
		MLines* pTempLine = new MLines(m_basePos, prePos);
		pTempLine->Draw(pDC, dmDrag);
		delete	pTempLine;
		g_pView->ReleaseDC(pDC);
	}
	m_nStep = 0;
	::Prompt("选择填充种子点：");
	return 0;
}

// 调用Cancel 函数取消本次操作
int CFillPattern::Cancel()
{
	Position	prePos = m_desPos; // 得到上一个鼠标位置
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC();
		// 清除上一个绘制的橡皮线
		MLines* pTempLine = new MLines(m_basePos, prePos);
		pTempLine->Draw(pDC, dmDrag);
		delete	pTempLine;
		g_pView->ReleaseDC(pDC);
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("就绪"); // 等待提示新类型的命令操作
	return 0;
}

