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

void RotatePattern(const Position& m_seed, const Position& m_dir, COLORREF pattern[8][8], COLORREF blankColor);

int FillColor(const Position& m_seed, const COLORREF& b_color)
{
	CDC* pDC = g_pView->GetDC();

	CPoint pt_seed;
	g_pView->WorldtoScreen(m_seed, pt_seed);

	int		n = GetROP2(pDC->GetSafeHdc());
	CBrush brush;
	brush.CreateSolidBrush(g_CurColor);
	CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brush);
	pDC->FloodFill(pt_seed.x, pt_seed.y, b_color);
	pDC->SelectObject(pOldBrush);
	pDC->SetROP2(n);

	g_pView->ReleaseDC(pDC);
	return 0;
}

int FillPattern(const Position& m_seed, const Position& m_dir, const COLORREF& b_color)
{
	CDC* pDC = g_pView->GetDC();

	CPoint pt_seed;
	g_pView->WorldtoScreen(m_seed, pt_seed);

	int		n = GetROP2(pDC->GetSafeHdc());

	// Create a hatched bit pattern.
	COLORREF blankColor = RGB(127, 191, 255);
	COLORREF HatchBits[8][8] = {0};
	

	for (UINT pi = 0; pi < 8; pi++)
	{
		for (UINT pj = 0; pj < 8; pj++)
		{
			HatchBits[pi][pj] = blankColor;
		}
	}

	double dx = (double)m_dir.x - m_seed.x;
	double dy = (double)m_dir.y - m_seed.y;
	double k;
	if (dx != 0) { k = dy / dx; }
	else { k = 1000.0; }
	if (k < 0 && k >= -1)
	{
		int x0 = 0, y0 = 7;
		while (x0 < 8 && y0 > 0)
		{
			HatchBits[x0][y0] = g_CurColor;
			x0++;
			y0 = y0 + k * x0 + 0.5;
		}
		x0 = 0; y0 = 3;
		while (x0 < 8 && y0 > 0)
		{
			HatchBits[x0][y0] = g_CurColor;
			x0++;
			y0 = y0 + k * x0 + 0.5;
		}
	}
	else if (k >= 0 && k <= 1)
	{
		int x0 = 0, y0 = 0;
		while (x0 < 8 && y0 < 8)
		{
			HatchBits[x0][y0] = g_CurColor;
			x0++;
			y0 = y0 + k * x0 + 0.5;
		}
		x0 = 0; y0 = 3;
		while (x0 < 8 && y0 < 8)
		{
			HatchBits[x0][y0] = g_CurColor;
			x0++;
			y0 = y0 + k * x0 + 0.5;
		}
	}
	else if (k > 1)
	{
		k = 1 / k;
		int x0 = 0, y0 = 0;
		while (x0 < 8 && y0 < 8)
		{
			HatchBits[x0][y0] = g_CurColor;
			y0++;
			x0 = x0 + k * y0 + 0.5;
		}
		x0 = 3; y0 = 0;
		while (x0 < 8 && y0 < 8)
		{
			HatchBits[x0][y0] = g_CurColor;
			y0++;
			x0 = x0 + k * y0 + 0.5;
		}
	}
	else if (k < -1)
	{
		k = 1 / k;
		int x0 = 7, y0 = 0;
		while (x0 > 0 && y0 < 8)
		{
			HatchBits[x0][y0] = g_CurColor;
			y0++;
			x0 = x0 + k * y0 + 0.5;
		}
		x0 = 3; y0 = 0;
		while (x0 < 8 && y0 < 8)
		{
			HatchBits[x0][y0] = g_CurColor;
			y0++;
			x0 = x0 + k * y0 + 0.5;
		}
	}

	// Use the bit pattern to create a bitmap.
	CBitmap bm;
	bm.CreateBitmap(8, 8, 1, 32, HatchBits);

	// Create a pattern brush from the bitmap.
	CBrush brush; 
	brush.CreatePatternBrush(&bm);

	// Select the brush into a device context, and draw.
	CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brush);
	pDC->FloodFill(pt_seed.x, pt_seed.y, b_color);

	// Restore the original brush.
	pDC->SelectObject(pOldBrush);
	pDC->SetROP2(n);

	g_pView->ReleaseDC(pDC);

	return 0;
}

void RotatePattern(const Position& m_seed, const Position& m_dir, COLORREF pattern[8][8], COLORREF blankColor)
{
	COLORREF copy[8][8];
	for (int pi = 0; pi < 8; pi++)
	{
		for (int pj = 0; pj < 8; pj++)
		{
			copy[pi][pj] = pattern[pi][pj];
			pattern[pi][pj] = blankColor;
		}
	}

	double dx = (double)m_dir.x - m_seed.x;
	dx = dx > 0 ? dx : -dx;
	double dy = (double)m_dir.y - m_seed.y;
	double cosv = dx / (dx * dx + dy * dy);
	double sinv = sqrt(1.0 - cosv * cosv);
	for (int pi = 0; pi < 8; pi++)
	{
		for (int pj = 0; pj < 8; pj++)
		{
			int ri = pi * cosv - pj * sinv + (1.0 - cosv) * 3.5 + 3.5 * sinv;
			int rj = pi * sinv + pj * cosv + (1.0 - cosv) * 3.5 - 3.5 * sinv;
			ri = ri > 7 ? ri % 7 : ri;
			ri = ri < 0 ? (ri + 7) % 7 : ri;
			rj = rj > 7 ? ri % 7 : rj;
			rj = rj < 0 ? (ri + 7) % 7 : rj;
			pattern[ri][rj] = copy[pi][pj];
		}
	}
}

