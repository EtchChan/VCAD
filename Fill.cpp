#include "stdafx.h"
#include "math.h"


#include "Fill.h"

#include "Entity.h"
#include "MainFrm.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "Command.h"
#include "ModifyCmd.h"

int Foo(const Position& m_seed, const COLORREF& b_color)
{
	CDC* pDC = g_pView->GetDC();

	CPoint pt_seed;
	g_pView->WorldtoScreen(m_seed, pt_seed);

	int		n = GetROP2(pDC->GetSafeHdc());
	CBrush brush;
	brush.CreateSolidBrush(RGB(0,0,255));
	CBrush* pBrush = (CBrush*)pDC->SelectObject(&brush);
	pDC->FloodFill(pt_seed.x, pt_seed.y, b_color);
	pDC->SetROP2(n);

	g_pView->ReleaseDC(pDC);
	return 0;
}
