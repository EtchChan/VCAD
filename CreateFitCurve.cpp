#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Entity.h"
#include "Command.h"
#include "CreateCmd.h"
#include "sysmath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCreateFitCurve::CCreateFitCurve(UINT nCmd)
	: m_begin(0,0), m_end(0,0)
{
	m_nStep = 0; // ��ʼ��������Ϊ 0
	m_pPositions = NULL;
	m_nCmd = nCmd;
}

CCreateFitCurve::~CCreateFitCurve()
{
	if(m_pPositions) {
		delete [] m_pPositions;
		m_pPositions = NULL;
	}
}

int CCreateFitCurve::GetType()
{
	return ctCreateLine; 
}

int	CCreateFitCurve::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++; // ÿ�ε���������ʱ�������� 1 

	int  i;
	//����㼯
	Position * pNew = new Position[m_nStep];
	for( i=0; i<m_nStep-1; i++)
		pNew[i] = m_pPositions[i];
	pNew[i] = pos;
	if(m_nStep > 1 && m_pPositions)
		delete [] m_pPositions;
	m_pPositions = pNew;

	switch(m_nStep) // ���ݲ�����ִ����Ӧ�Ĳ���
	{
		case 1: 
		{
			m_begin = m_end = pos;

			::Prompt("������ֱ�ߵ���һ�㣺");
			break;
		}
		default:
		{
			CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ�� 

			// �������϶�״̬ʱ��ʾ�����һ����
			//MLines*	pTempLine = new MLines(m_begin,m_end); 
			//pTempLine->Draw(pDC, dmDrag);
			//delete pTempLine;

			// ����ڰ��������Ĺ�����ͬʱ������Shift����
			//	��ô������굥��λ�û���ˮƽ�߻���ֱ��
			if( nFlags & MK_SHIFT ){ 
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx) <= fabs(dy)) // �����굥��λ����X���򿿽����
					m_end.Set(m_begin.x, pos.y); // ��ô�յ��x������������ͬ
				else
					m_end.Set(pos.x,m_begin.y);
			}
			else
				m_end = pos; // ���δ����Shift��, ���յ�Ϊ��굥��λ��			
			
			m_begin = pos;
			break;
		}		
	}

	return 0;
}


//����������ɵ㼯�ϵõ� ���κ�����ϵ��
void GetABC(int num, Position * pts,  double &a, double &b, double & c)
{

	double A11, A12, A13, B1;
	double A21, A22, A23, B2;
	double A31, A32, A33, B3;

	int i =0;
	//�������ϵ��
	A11 = 0;
	for (i=0; i<num; i++)
	{
		A11 += 1;//pts[i].x ^ 0;
	}

	A12 = 0;
	for (i=0; i<num; i++)
	{
		A12 += pts[i].x;
	}

	A13 = 0;
	for (i=0; i<num; i++)
	{
		A13 += pts[i].x*pts[i].x;
	}

	A21 = A12;
	A22 = A13;
	
	A23 = 0;
	for (i=0; i<num; i++)
	{
		A23 += pts[i].x*pts[i].x*pts[i].x;
	}

	A31 = A22;
	A32 = A23;
	
	A33 = 0;
	for (i=0; i<num; i++)
	{
		A33 += pts[i].x*pts[i].x*pts[i].x*pts[i].x;
	}

	B1 = 0;
	for (i=0; i<num; i++)
	{
		B1 += 1*pts[i].y;
	}

	B2 = 0;
	for (i=0; i<num; i++)
	{
		B2 += pts[i].x*pts[i].y;
	}

	B3 = 0;
	for (i=0; i<num; i++)
	{
		B3 += pts[i].x*pts[i].x*pts[i].y;
	}


	//��ⷽ���飬��Ԫһ�η�����
	double a11 = A21 - A11*A23/A13;
	double a12 = A22 - A12*A23/A13;
	double b1 = B2 - A23*B1/A13;
	double a21 = A31 - A11*A33/A13;
	double a22 = A32 - A12*A33/A13;
	double b2 = B3 - A33*B1/A13;

    a = (b2-b1*a22/a12) / (a21 - a11*a22/a12);
	b = (b1 - a11*a) / a12;

	double x0 = 0;
	x0 = a11*a + a12*b - b1;
	x0 = a21*a + a22*b - b2;

	c = (B1 - A11*a - A12*b)/A13;

	x0 = A11*a + A12*b + A13*c - B1;
	x0 = A21 * a + A22* b + A23*c - B2;
	x0 = A31*a + A32 * b + A33*c - B3;

	return;
}


//�ɶ��κ������̵õ�һϵ�еĵ�
void GeneratePoints(double a, double b,double c,int num, Position *pts)
{
	double xmin = pts[0].x;
	double xmax = pts[0].y;

	double delta = (xmax - xmin) / (num-1);
	for (int i=0; i<num; i++)
	{
		double x = xmin + delta * i;
		double y = c*x*x + b*x + a;

		pts[i].x = x;
		pts[i].y = y;
	}
}

#define MAXDETA 0.00001

void GetBS3OpenControl(Position *ValuePoint, Position *ControlPoint,int ValuePointNum)
{
	if(ValuePointNum<2) return;
	int i;
	
	// Use Gauss-Seidel algorithm to get value of equation:
	//     1/6xQ(i-1)+2/3xQ(i)+1/6xQ(i+1) = P(i)
	
	// Step 1:Initialization
	for(i=1;i<=ValuePointNum;i++) ControlPoint[i]=ValuePoint[i-1];
	ControlPoint[0]=ControlPoint[1];
	ControlPoint[ValuePointNum+1]=ControlPoint[ValuePointNum];
	
	// Step 2:Caculate deta(i)
	Position deta(0.0,0.0),maxdeta(0.0,0.0);
	do
	{
		deta.x=0.0;  deta.y=0.0;
		maxdeta.x=0.0;  maxdeta.y=0.0;
		for(i=1;i<=ValuePointNum;i++)
		{
			deta=ValuePoint[i-1]-ControlPoint[i]+
				(ValuePoint[i-1]-(ControlPoint[i-1]+ControlPoint[i+1])*0.5)*0.5;
			ControlPoint[i]=ControlPoint[i]+deta;
			if((~deta)>(~maxdeta)) maxdeta=deta;
		}
		ControlPoint[0]=ControlPoint[1];
		ControlPoint[ValuePointNum+1]=ControlPoint[ValuePointNum];
	}while((~maxdeta)>MAXDETA);
}


extern void bezier_n(int n, Position * pts,    int m, Position* pt);
extern void Draw_BSpline3(int n, Position *pts, int num, Position * ptOut);



//ɾ�����ɵ���ʱֱ�߶�
void DeletemLines(CObList &newEnts)
{
    POSITION pos = newEnts.GetHeadPosition();
	while (pos)
	{
		MEntity* pEnt = (MEntity*)newEnts.GetNext(pos);

		delete pEnt;
	}

	newEnts.RemoveAll();
}

//��ͼ��ʵ���еĶ���߷ָ��ֱ�߶�
void DivideMLines(CObList &oldEnts, CObList &newEnts)
{
    POSITION pos = oldEnts.GetHeadPosition();
	while (pos)
	{
		MEntity* pEnt = (MEntity*) oldEnts.GetNext(pos);

		if (pEnt->GetType() == etLine)
		{
			int num = ((MLines*)pEnt)->GetNumOfPoints();
			//Position * pts = ((MLines*)pEnt)->GetPoints();
			std::vector<Position> pts = ((MLines*)pEnt)->GetPoints();

			for (int i=0; i<num-1; i++)
			{
				std::vector<Position> p2ts;// = new Position[2];
				p2ts.push_back(pts[i]);
				p2ts.push_back(pts[i+1]);

				MLines * pLine = new MLines(p2ts);
				newEnts.AddTail(pLine);
			}
		}
	}	
}

//�Ƿ�Ϊͬһ��
BOOL IsSamePoint(Position pt1, Position pt2)
{
	if (fabs(pt1.x - pt2.x) < 1E-8 && fabs(pt1.y - pt2.y) < 1E-8 )
	{
		return TRUE;
	}

	return FALSE;
}

//��pt�Ƿ�Ϊʵ��pEnt�Ķϵ�
BOOL IsEndPoint(Position pt, MEntity * pEnt)
{
	ASSERT(pEnt->GetType() == etLine);

	MLines * pLine = (MLines*)pEnt;

	if (IsSamePoint(pt, pLine->GetBeginPos()) || IsSamePoint(pt, pLine->GetEndPos()))
	{
		return TRUE;
	}

	return FALSE;
}

//���Ƿ��ڻ���
BOOL PointInLoop(Position pt, CList<Position, Position>& ptLoop)
{
	POSITION pos = ptLoop.GetHeadPosition();
	while (pos)
	{
		Position pt1 = ptLoop.GetNext(pos);
		if (IsSamePoint(pt, pt1))
		{
			return TRUE;
		}
	}

	return FALSE;
}

double GetAngle(double x0, double y0, double x1, double y1, double x2, double y2)
// ��˳ʱ�뷽�����POQ: P(x1,y1), O(x0,y0), Q(x2,y2)
// ����ֵ�� (0 -- 2 Pi) ֮�䣬��λΪ����
{	
	#define Pi 3.1415926535897932
	double x, y, r, alfa;
	x=(x1-x0)*(x2-x0)+(y1-y0)*(y2-y0);
	y=-(x1-x0)*(y2-y0)+(x2-x0)*(y1-y0);
	r=sqrt(x*x+y*y);
	alfa=acos(x/r);
	if(y<=0.0) alfa=2*Pi-alfa;

	return alfa;
}

double GetDistance(Position p1, Position p2)
{
	return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

//�󽻵�
int GetInterPoint(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, double &x, double &y )
{	// �н⣬����1���޽⣬����0��
	double delta;
	delta = -(x2-x3)*(y0-y1)+(x0-x1)*(y2-y3);

	if(fabs(delta) < 1E-8)	// �Ƚ��ֱ��ƽ��
	{
		if( fabs((y1-y0)*(x2-x0)-(y2-y0)*(x1-x0)) < 1E-8 )	// ����
		{	if ( (x2-x0)*(x1-x2) >= 0.0 && (y2-y0)*(y1-y2) >= 1E-10 ) 
			{	x=x2; y=y2; 
				//printf(" x, y, delta = %g, %g,     %g", x, y, delta);
				return 1;}
			else if ( (x3-x0)*(x1-x3) >= 0.0 && (y3-y0)*(y1-y3) >= 1E-10 ) 
			{	x=x3; y=y3; 
				//printf(" x, y, delta = %g, %g,     %g", x, y, delta);
				return 1;}
			else if ( (x0-x2)*(x3-x0) >= 0.0 && (y0-y2)*(y3-y0) >= 1E-10 ) 
			{	x=x3; y=y3; 
				//printf(" x, y, delta = %g, %g,     %g", x, y, delta);
				return 1;}
			else return 0;
		}
		else return 0;
	}
	else
	{
		x = ((x2-x3)*(x0*y1-x1*y0)+(x0-x1)*(x3*y2-x2*y3))/delta;
		y = ((y2-y3)*(x0*y1-x1*y0)+(y0-y1)*(x3*y2-x2*y3))/delta;
		//printf(" x, y, delta = %g, %g,     %g", x, y, delta);
		if ( (x-x2)*(x3-x) >= -1E-10 && (x-x0)*(x1-x) >= -1E-10 ) return 1;
		else return 0;
	}	
}

//��ǰ�����pStart, �յ�pEnd, ʵ��pEnt,��ʵ�弯mLines��Ѱ����һ��ʵ��pEnt�����������յ�
//�ҵ�����True������ʱ����Ϊfalse
BOOL GetNextPoints(Position &pStart, Position &pEnd, CObList &mLines, MEntity *& pEnt)
{
	BOOL bReturn = FALSE;
	//�õ���һ������
	Position pInter;
	//���㴦��ʵ�弯, ȥ���Լ�
	CObList interLines;
	double dis0 = 1E10; 
	POSITION pos = mLines.GetHeadPosition();
	while (pos)
	{
		MLines * pLine = (MLines*)mLines.GetNext(pos);
		if (pLine == pEnt)
		{
			continue;
		}
		
		Position pSt = pLine->GetBeginPos();
		Position pEd = pLine->GetEndPos();

		Position xy;
		if (GetInterPoint(pStart.x, pStart.y, pEnd.x, pEnd.y, pSt.x, pSt.y, pEd.x, pEd.y, xy.x, xy.y))
		{
			if (IsSamePoint(xy, pStart))
			{
				continue;
			}

			
			bReturn = TRUE;

			double dis = GetDistance(pStart, xy);

			if (fabs(dis - dis0) < 1E-8) //ͬ����
			{
				interLines.AddTail(pLine);
				pInter = xy;
			}
			else if (dis < dis0)
			{
				interLines.RemoveAll();
				interLines.AddTail(pLine);
				pInter = xy;

				dis0 = dis;
			}
		}
	}

	//�õ�interLines������㡢�յ㣬����н���С��
    pos = interLines.GetHeadPosition();

	double alfa = 1E10;
	while (pos)
	{
		MLines * pLine = (MLines*)interLines.GetNext(pos);

		Position ptStart = pLine->GetBeginPos();
		if (!IsSamePoint(ptStart, pInter))
		{
			double interAngle = GetAngle(pInter.x, pInter.y, pStart.x, pStart.y, ptStart.x, ptStart.y);

			if (interAngle < alfa)
			{
				pEnd = ptStart;
				pEnt = pLine;
				alfa = interAngle;
			}
		}

		Position ptEnd = pLine->GetEndPos();
		if (!IsSamePoint(ptEnd, pInter))
		{
			double interAngle = GetAngle(pInter.x, pInter.y, pStart.x, pStart.y, ptEnd.x, ptEnd.y);

			if (interAngle < alfa)
			{
				pEnd = ptEnd;
				pEnt = pLine;
				alfa = interAngle;
			}
		}
	}

	//����pInterΪ��һ�����
	pStart = pInter;

	return bReturn;
}


//������˫����������
int CCreateFitCurve::OnLButtonDblClk(UINT nFlags, const Position& pos)
{
	CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ�� 
	
	// �������϶�״̬ʱ��ʾ�����һ����
	MLines*	pTempLine = new MLines(m_begin,m_end); 
	pTempLine->Draw(pDC, dmDrag);
	delete pTempLine;
	
	// ����ڰ��������Ĺ�����ͬʱ������Shift����
	//	��ô������굥��λ�û���ˮƽ�߻���ֱ��
	if( nFlags & MK_SHIFT ){ 
		double dx = pos.x - m_begin.x;
		double dy = pos.y - m_begin.y;
		if(fabs(dx) <= fabs(dy)) // �����굥��λ����X���򿿽����
			m_end.Set(m_begin.x, pos.y); // ��ô�յ��x������������ͬ
		else
			m_end.Set(pos.x,m_begin.y);
	}
	else
		m_end = pos; // ���δ����Shift��, ���յ�Ϊ��굥��λ��			
	

//////////////////////////////////////////////////////////////////////////
	//����ʶ��, �µ���
	if (m_nCmd == 32816)
	{
		//���ʰȡ�ڵ�
		Position ptInner = m_end;

		//����������ˮƽ���ߣ��󵽵�һ������
		Position pEnd0 = ptInner;
		pEnd0.x = 1E3;

		//��ֱ�߶�̷ָ�ɼ򵥵�ʵ��
		CObList lines;
		DivideMLines(g_pDoc->m_EntityList, lines);

		//�ڵ�һ�����㴦����õ�һ��ʵ�壬��¼���
		Position ptFirst = ptInner;
		MEntity * pEnt = NULL;
		ASSERT( GetNextPoints(ptFirst, pEnd0, lines, pEnt) );

		CList<Position, Position> ptLoop;
		if (IsEndPoint(ptFirst, pEnt))
		{
			ptLoop.AddTail(ptFirst);
		}

		//������㵽ʵ����յ�Ϊ����
		//�ص����ʱ����
		while (1)
		{
			CCircle*	pTempCircle = new CCircle(ptFirst, 4); 
			pTempCircle->Draw(pDC,dmNormal); // ���ƶ����
			g_pDoc->m_EntityList.AddTail(pTempCircle); // �������ָ����ӵ�ͼԪ����
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
			pTempCircle->m_nOperationNum = g_pView->m_nCurrentOperation;	

			pTempCircle = new CCircle(pEnd0, 2); 
			pTempCircle->Draw(pDC,dmNormal); // ���ƶ����
			g_pDoc->m_EntityList.AddTail(pTempCircle); // �������ָ����ӵ�ͼԪ����
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
			pTempCircle->m_nOperationNum = g_pView->m_nCurrentOperation;	

			BOOL bLoop = GetNextPoints(ptFirst, pEnd0, lines, pEnt);
			if (PointInLoop(ptFirst, ptLoop))
			{
				//�ɻ��ˣ��˳�
				break;
			}

			if (!bLoop)
			{
				AfxMessageBox("������");
				DeletemLines(lines);
				
				return 0;
			}
			else {
				ptLoop.AddTail(ptFirst);
			}
		}

		//��ʾ�������
		int num = ptLoop.GetCount();
		Position * pPositions = new Position[num];
		POSITION pos = ptLoop.GetHeadPosition();
		int i = 0;
		while(pos)
		{
			Position pt = ptLoop.GetNext(pos);
			pPositions[i++] = pt;
		}
		MPolygon*	pNewLine = new MPolygon(num, pPositions);// ���������յ㴴�������
		pNewLine->Draw(pDC,dmNormal); // ���ƶ����
		g_pDoc->m_EntityList.AddTail(pNewLine); // �������ָ����ӵ�ͼԪ����
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;	

		DeletemLines(lines);
	}

//////////////////////////////////////////////////////////////////////////
//���ƿ��ƶ����;
	if(m_nStep >= 2) {
		MLines*	pNewLine = new MLines(m_nStep,m_pPositions);// ���������յ㴴��ֱ��
		pNewLine->SetColor(RGB(0,0,0));
		pNewLine->SetWidth(1);
		pNewLine->Draw(pDC,dmNormal); // ����ֱ��
		g_pDoc->m_EntityList.AddTail(pNewLine); // ��ֱ��ָ����ӵ�ͼԪ����
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;

//////////////////////////////////////////////////////////////////////////
//��С���˷�
		int num = 0;
		Position * pts = NULL;
	
		if (m_nCmd == 32810)
		{
			num = 100;
			pts = new Position[num];

			//����������� y = c*x^2 + b*x + a
			double a, b, c;
			GetABC(m_nStep, m_pPositions, a, b, c);

			//����㵽�յ���ɢ100����
			pts[0].x = m_pPositions[0].x;
			pts[0].y = m_pPositions[m_nStep-1].x;
			GeneratePoints(a, b, c, 100, pts);
		}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
		//Bezier
		else if (m_nCmd == 32811)
		{
			num = 100;
			pts = new Position[num];
			bezier_n(m_nStep-1, m_pPositions, num, pts);
		}
//////////////////////////////////////////////////////////////////////////
		//BSpline
		else if (m_nCmd == 32812 && m_nStep >= 4)
		{
			num = 100;
			num = num * (m_nStep-3);

			pts = new Position[num];

			Draw_BSpline3(m_nStep, m_pPositions, 100, pts);
		}
//////////////////////////////////////////////////////////////////////////
		//BSpline����
		else if (m_nCmd == 32815) //
		{
			//������ƶ���
			Position * vCtlPts = new Position[m_nStep + 2];
			GetBS3OpenControl(m_pPositions, vCtlPts, m_nStep);


			num = 100;
			num = num * (m_nStep-1);

			pts = new Position[num];

			Draw_BSpline3(m_nStep+2, vCtlPts, 100, pts);
		}
	

		//�����������
		if (num >= 2)
		{
			pNewLine = new MLines(num, pts);// ���������յ㴴��ֱ��
			pNewLine->Draw(pDC,dmNormal); // ����ֱ��
			g_pDoc->m_EntityList.AddTail(pNewLine); // ��ֱ��ָ����ӵ�ͼԪ����
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
			pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;
		}
	}
	
	g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	
	m_nStep = 0;  // ������������Ϊ 0
	::Prompt("������ֱ�ߵ���㣺");
	
	return 0;
}

int	CCreateFitCurve::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_LINE));

	// ��һ��̬����nPreRefresh��¼����OnMouseMove״̬ʱ��ˢ�´���
	static	int nPreRefresh = g_nRefresh; 
	// ��������bRefresh˵����OnMouseMove�������Ӵ��Ƿ�ˢ��
	BOOL	bRefresh = FALSE; 
	// nCurRefresh���ڼ�¼��ǰ��ˢ�´���
	int		nCurRefresh = g_nRefresh; 
	// ���nCurRefresh��nPreRefresh����ȣ�˵���Ӵ�����ˢ�¹�
	if(nCurRefresh != nPreRefresh){ 
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh; 
	}

	switch(m_nStep)
	{
		case 0:
			::Prompt("������ֱ�ߵ���㣺");
			break;
		default:
		{

			Position	prePos, curPos;
			prePos = m_end; // ���������ڵ�ǰһ��λ��
			
			// ����ڰ��������Ĺ�����ͬʱ������Shift����
			//	��ô������굥��λ�û���ˮƽ�߻���ֱ��
			if( nFlags & MK_SHIFT ){
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx)>=fabs(dy))
					curPos.Set(pos.x,m_begin.y);
				else
					curPos.Set(m_begin.x, pos.y);
			}
			else
				curPos = pos;

			CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
			
			// ������ʱ���������һ����Ƥ��
			MLines*	pTempLine1 = new MLines(m_begin, prePos);
			if(!bRefresh) // ���Ӵ�û�б�ˢ��ʱ���ػ�ԭ������Ƥ��ʹ�䱻����
				pTempLine1->Draw(pDC, dmDrag);
			delete pTempLine1;
			// ������ʱ���󣬸��ݵ�ǰλ�û���һ����Ƥ��
			MLines*	pTempLine2 = new MLines(m_begin, curPos);	
			pTempLine2->Draw(pDC, dmDrag);
			delete pTempLine2;
 

	if(m_nStep >= 2) {
//////////////////////////////////////////////////////////////////////////
//��С���˷�
		int num = 0;
		Position * ptsOld = NULL;
		Position * pts = NULL;

		Position * vPtsPre = new Position[m_nStep+1];
		Position * vPtsCur = new Position[m_nStep+1];
		for (int k=0; k<m_nStep; k++)
		{
			vPtsCur[k] = m_pPositions[k];
			vPtsPre[k] = m_pPositions[k];
		}
		vPtsPre[m_nStep] = prePos;
		vPtsCur[m_nStep] = curPos;
	
		if (m_nCmd == 32810)
		{
			num = 100;
			ptsOld = new Position[num];
			pts = new Position[num];

			//����������� y = c*x^2 + b*x + a
			double a, b, c, a2, b2, c2;
			GetABC(m_nStep+1, vPtsPre, a, b, c);
			GetABC(m_nStep+1, vPtsCur, a2, b2, c2);

			//����㵽�յ���ɢ100����
			ptsOld[0].x = vPtsPre[0].x;
			ptsOld[0].y = vPtsPre[m_nStep].x;
			pts[0].x = vPtsCur[0].x;
			pts[0].y = vPtsCur[m_nStep].x;
			GeneratePoints(a, b, c, 100, ptsOld);
			GeneratePoints(a2, b2, c2, 100, pts);
		}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
		//Bezier
/*		else if (m_nCmd == 32811)
		{
			num = 100;
			pts = new Position[num];
			bezier_n(m_nStep-1, m_pPositions, num, pts);
		}
//////////////////////////////////////////////////////////////////////////
		//BSpline
		else if (m_nCmd == 32812 && m_nStep >= 4)
		{
			num = 100;
			num = num * (m_nStep-3);

			pts = new Position[num];

			Draw_BSpline3(m_nStep, m_pPositions, 100, pts);
		}

//////////////////////////////////////////////////////////////////////////
		//BSpline����
		else if (m_nCmd == 32815) //
		{
			//������ƶ���
			Position * vCtlPts = new Position[m_nStep + 2];
			GetBS3OpenControl(m_pPositions, vCtlPts, m_nStep);


			num = 100;
			num = num * (m_nStep-1);

			pts = new Position[num];

			Draw_BSpline3(m_nStep+2, vCtlPts, 100, pts);
		}
	
*/
		//�����������
		if (num >= 2)
		{
			// ������ʱ���������һ����Ƥ��
			MLines*	pTempLine1 = new MLines(num, ptsOld);
			if(!bRefresh) // ���Ӵ�û�б�ˢ��ʱ���ػ�ԭ������Ƥ��ʹ�䱻����
				pTempLine1->Draw(pDC, dmDrag);
			delete pTempLine1;
			// ������ʱ���󣬸��ݵ�ǰλ�û���һ����Ƥ��
			MLines*	pTempLine2 = new MLines(num, pts);	
			pTempLine2->Draw(pDC, dmDrag);
			delete pTempLine2;
		}

		delete []vPtsCur;
		delete []vPtsPre;
	}


			g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��			

			m_end = curPos; // ����ǰλ������Ϊֱ���յ㣬�Ա���һ������ƶ�ʱ��
			break;
		}
	}
	return 0;
}

// ��������Ҽ�ȡ����ǰ�Ĳ���
int	CCreateFitCurve::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_end; // ���������ڵ�ǰһ��λ��
		MLines*	pTempLine = new MLines(m_begin, prePos); 
		pTempLine->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempLine;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("������ֱ�ߵ���㣺");
	return 0;
}
// ����Cancel ����ȡ�����β���
int CCreateFitCurve::Cancel()
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_end; // ���������ڵ�ǰһ��λ��
		MLines*	pTempLine = new MLines(m_begin, prePos); 
		pTempLine->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempLine;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("����"); // �ȴ���ʾ�����͵��������
	return 0 ;
}

