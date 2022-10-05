#include "stdafx.h"
#include "math.h"

#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"

#include "Entity.h"

#include "Command.h"
#include "CreateCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/*	
 *	CLASS	CCreateArc
 */
CCreateArc::CCreateArc()
	: m_center(0,0), m_begin(0,0), m_end(0,0)
{
	m_nStep = 0; // ��ʼ��������Ϊ 0	
}

CCreateArc::~CCreateArc()
{
}

int CCreateArc::GetType()
{
	return ctCreateArc;
}

int	CCreateArc::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++; // ÿ�ε���������ʱ�������� 1 
	switch(m_nStep)
	{
		case 1:
		{
			m_center = pos;
			::Prompt("������Բ������ʼ�㣺") ;
			break;
		}
		case 2:
		{
			m_begin = m_end = pos ;
			::Prompt("������Բ�����յ㣺") ;
			break;
		}
		case 3:
		{
 			CDC*	pDC = g_pView->GetDC(); // a pointer to internal dc 
			
			// �������϶�״̬ʱ��ʾ����Ƥ��
			MLines*	pTempLine1 = new MLines(m_center, m_begin);
			MLines*	pTempLine2 = new MLines(m_center, m_end);
			CArc*	pTempArc = new CArc(m_center,m_begin, m_end);
			pTempLine1->Draw(pDC, dmDrag);
			pTempLine2->Draw(pDC, dmDrag);
			pTempArc->Draw(pDC, dmDrag);
			delete pTempLine1;
			delete pTempLine2;
			delete pTempArc;

			m_end = pos ;

			CArc*	pNewArc = new CArc(m_center,m_begin, m_end);			
			pNewArc->Draw(pDC,dmNormal);
			g_pDoc->m_EntityList.AddTail(pNewArc);
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;			
			pNewArc->m_nOperationNum = g_pView->m_nCurrentOperation;
			
			g_pView->ReleaseDC(pDC); // don't forget this
			
			m_nStep = 0; 
			::Prompt("������Բ�������ĵ㣺") ;
			break;
		}
		
	}
	return 0;
}

int	CCreateArc::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_ARC));
	
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
			::Prompt("������Բ�����ĵ㣺") ;
			break;
		case 1:
			break;
		case 2:
		{
			Position	prePos, curPos;
			prePos = m_end ;
			curPos = pos;
			
			CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
			
			// ������ʱ���������һ�ε���Ƥ��
			MLines*	pTempLine1 = new MLines(m_center, prePos);
			CArc*	pTempArc1 = new CArc(m_center,m_begin, prePos);
			if(!bRefresh){ // ���Ӵ�û�б�ˢ��ʱ���ػ�ԭ������Ƥ��ʹ�䱻����
				pTempLine1->Draw(pDC, dmDrag);
				pTempArc1->Draw(pDC, dmDrag);
			}
			delete pTempLine1;
			delete pTempArc1;

			// ������ʱ���󣬸��ݵ�ǰλ�û�����Ƥ��
			MLines*	pTempLine2 = new MLines(m_center, curPos);
			CArc*	pTempArc2 = new CArc(m_center,m_begin, curPos);
			if(!bRefresh){ // ���Ӵ�û�б�ˢ��ʱ���ػ�ԭ������Ƥ��ʹ�䱻����
				pTempLine2->Draw(pDC, dmDrag);
				pTempArc2->Draw(pDC, dmDrag);
			}
			delete pTempLine2;
			delete pTempArc2;

			g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��			

			m_end = curPos; // ����ǰλ������Ϊֱ���յ㣬�Ա���һ������ƶ�ʱ��			

			break;
		}
	}
	return 0;
}

int	CCreateArc::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	// �����ǰ�Ĳ�����Ϊ 2 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if(m_nStep == 2){ 
		CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
	
		Position	prePos = m_end; // ���������ڵ�ǰһ��λ��
		MLines*		pTempLine1 = new MLines(m_center, m_begin);
		MLines*		pTempLine2 = new MLines(m_center, prePos);
		CArc*		pTempArc = new CArc(m_center, m_begin, prePos);
		pTempLine1->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		pTempLine2->Draw(pDC, dmDrag); 
		pTempArc->Draw(pDC, dmDrag); 
		delete		pTempLine1;
		delete		pTempLine2;
		delete		pTempArc;
		
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("������Բ�������ĵ㣺") ;

	return 0;
}

// ����Cancel ����ȡ�����β���
int CCreateArc::Cancel()
{
	// �����ǰ�Ĳ�����Ϊ 2 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if(m_nStep == 2){ 
		CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
		
		Position	prePos = m_end; // ���������ڵ�ǰһ��λ��
		MLines*		pTempLine1 = new MLines(m_center, m_begin);
		MLines*		pTempLine2 = new MLines(m_center, prePos);
		CArc*		pTempArc = new CArc(m_center, m_begin, prePos);
		pTempLine1->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		pTempLine2->Draw(pDC, dmDrag); 
		pTempArc->Draw(pDC, dmDrag); 
		delete		pTempLine1;
		delete		pTempLine2;
		delete		pTempArc;

		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("����"); // �ȴ���ʾ�����͵��������
	
	return 0 ;
}