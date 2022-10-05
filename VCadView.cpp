// VCadView.cpp : implementation of the CVCadView class
//

#include "stdafx.h"
#include "VCad.h"
#include "math.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Base.h"
#include "Command.h"
#include "CreateCmd.h"
#include "ModifyCmd.h"
#include "ChgViewCmd.h"
#include "Entity.h"
#include "TextInputDlg.h"
#include "ECT2DModelDlg.h"
#include "GearParamDlg.h"
#include "InterDlg.h"
#include "sysmath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCadView

IMPLEMENT_DYNCREATE(CVCadView, CView)

BEGIN_MESSAGE_MAP(CVCadView, CView)
	//{{AFX_MSG_MAP(CVCadView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_PICK, OnPick)
	ON_UPDATE_COMMAND_UI(ID_PICK, OnUpdatePick)
	ON_COMMAND(ID_REDRAW, OnRedraw)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_ERASE, OnUpdateModifyErase)
	ON_COMMAND(ID_EDIT_MANHADUN, OnEditManhadun)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MANHADUN, OnUpdateEditManhadun)
	ON_COMMAND(ID_MENUITEM32807, OnMenuitem32807)
	ON_COMMAND(ID_MENUITEM32808, OnMenuitem32808)
	ON_COMMAND(ID_MENUITEM32809, OnMenuitem32809)
	ON_COMMAND(ID_MENUITEM32810, OnMenuitem32810)
	ON_COMMAND(ID_MENUITEM32811, OnMenuitem32811)
	ON_COMMAND(ID_MENUITEM32812, OnMenuitem32812)
	ON_COMMAND(ID_MENUITEM32813, OnMenuitem32813)
	ON_COMMAND(ID_MENUITEM32814, OnMenuitem32814)
	ON_COMMAND(ID_MENUITEM32815, OnMenuitem32815)
	ON_COMMAND(ID_INNERPT, OnInnerpt)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

	ON_COMMAND_RANGE(ID_CREATE_LINE, 
					ID_CREATE_CONNECT, OnCreateEntity)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CREATE_LINE, 
					ID_CREATE_CONNECT, OnUpdateCreateCommand)
	ON_COMMAND_RANGE(ID_MODIFY_MOVE, 
					ID_MODIFY_ERASE, OnModifyEntity)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MODIFY_MOVE, 
					ID_MODIFY_ERASE, OnUpdateModifyCommand)
	//��ͼ�任����
	ON_COMMAND_RANGE(ID_VIEW_REGION,ID_VIEW_PAN, OnViewCommand)//

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCadView construction/destruction

CVCadView::CVCadView()
{
	// TODO: add construction code here
	g_pView = this;
	m_pCmd = NULL;
	// ��ʼ��������������Ļ����Ĺ�ϵ
	m_dOrgX = m_dOrgY = 0.;
	scale = 0.4;
	m_bIsPrinting = FALSE;

	m_bLBPressed = FALSE;
	m_bDragging = FALSE;
	m_bEditable = TRUE;

	m_nStartOperateNum = 0;
	m_nOperations = 0;
	m_nCurrentOperation = 0;
}

CVCadView::~CVCadView()
{
	if( m_pCmd )
		delete m_pCmd ;
}

void CVCadView::SetStartOperateNum(int num)
{
	m_nStartOperateNum = num;
	m_nOperations = m_nCurrentOperation = num;
}

BOOL CVCadView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVCadView drawing

void CVCadView::OnDraw(CDC* pDC)
{
	g_nRefresh ++; // ÿ���Ӵ������»���ʱ��ˢ�´����� 1
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPen	pen; 
	pen.CreatePen(PS_SOLID, 1, RGB(200,200,200)) ;
	CPen *	pOldPen = pDC->SelectObject(&pen); // �õ�ԭ���Ļ���

	int i;
	for( i=0; i<150*scale + 1; i++) {
		pDC->MoveTo(0, i*10/scale);
		pDC->LineTo(2000,i*10/scale);
	}
	for(i=0; i<250*scale +1; i++) {
		pDC->MoveTo(i*10/scale, 0);
		pDC->LineTo(i*10/scale, 1200);
	}
	pDC->SelectObject(pOldPen); // �ָ�ԭ���Ļ��� 
	pen.DeleteObject();
	DrawCoord(pDC) ;

	// TODO: add draw code for native data here
	pDoc->Draw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CVCadView diagnostics

#ifdef _DEBUG
void CVCadView::AssertValid() const
{
	CView::AssertValid();
}

void CVCadView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVCadDoc* CVCadView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVCadDoc)));
	return (CVCadDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVCadView message handlers

void CVCadView::OnCreateEntity(UINT m_nID)
{

	// �����ǰ״̬���������ô
	// 1. ȡ��������Ĳ���
	// 2. ɾ�����������
	// 3. ������ָ����Ϊ��
	if( m_pCmd ){ 
		m_pCmd->Cancel();
		delete m_pCmd ;
		m_pCmd = NULL; 
	}
	// ������ݲ�ͬ�Ĳ˵��������ͬ���������
	switch(m_nID)
	{
		case ID_CREATE_LINE: // ֱ��
		{
			m_pCmd = new CCreateLine();
			break;
		}
		case ID_CREATE_RECTANGLE: // ����
		{
			m_pCmd = new CCreateRect();
			break;
		}
		case ID_CREATE_CIRCLE: // Բ
		{
			m_pCmd = new CCreateCircle();
			break;
		}
		case ID_CREATE_ARC: // Բ��
		{
			m_pCmd = new CCreateArc();
			break;
		}
		case ID_CREATE_ELLIPSE: //��Բ
		{
			m_pCmd = new CCreateEllipse();
			break;
		}
		case ID_CREATE_TEXT: // �ı�
		{
			m_pCmd = new CCreateText();
			break;
		}
		case ID_CREATE_POLYGON: // �����
		{
			m_pCmd = new CCreatePolygon();
			break;
		}
		case ID_CREATE_BLOCK:
		{
			m_pCmd = new CCreateBlock();
			break;
		}
		case ID_CREATE_CONNECT:
		{
			m_pCmd = new CCreateConnect();
			break;
		}
		case 32810:
		{
			m_pCmd = new CCreateFitCurve(32810);//��С���˷�
			break;
		}
		case 32811:
		{
			m_pCmd = new CCreateFitCurve(32811);//Bezier
			break;
		}
		case 32812:
		{
			m_pCmd = new CCreateFitCurve(32812);//BSpline
			break;
		}
		case 32815:
		{
			m_pCmd = new CCreateFitCurve(32815);//BSpline
			break;
		}
		case 32816:
		{
			m_pCmd = new CCreateFitCurve(32816);//ʶ��������
			break;
		}
	}
}

void CVCadView::OnUpdateCreateCommand(CCmdUI* pCmdUI)
{
	int flag = 0 ;
	switch(pCmdUI->m_nID)
	{
		case ID_CREATE_LINE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateLine) )
				flag = 1;
			break;
		}
		case ID_CREATE_RECTANGLE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateRectangle) )
				flag = 1;
			break;
		}
		case ID_CREATE_CIRCLE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateCircle) )
				flag = 1;
			break;
		}
		case ID_CREATE_ARC:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateArc) )
				flag = 1;
			break;
		}
		case ID_CREATE_ELLIPSE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateEllipse) )
				flag = 1;
			break;
		}
		case ID_CREATE_TEXT:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateText) )
				flag = 1;
			break;
		}
		case ID_CREATE_POLYGON:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreatePolygon) )
				flag = 1;
			break;
		}
		case ID_CREATE_BLOCK:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateBlock) )
				flag = 1;
			break;
		}
		case ID_CREATE_CONNECT:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateConnect) )
				flag = 1;
			break;
		}
		default:
			break;
	}
	pCmdUI->SetCheck(flag);
}

void CVCadView::OnModifyEntity(UINT m_nID)
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT(pDoc) ;

	// �����ǰ״̬���������ô
	// 1. ȡ��������Ĳ���
	// 2. ɾ�����������
	// 3. ������ָ����Ϊ��
	if( m_pCmd ){ 
		m_pCmd->Cancel();
		delete m_pCmd ;
		m_pCmd = NULL; 
	}

	// �ж��Ƿ���ʵ��ͼԪ��ѡ��
	if(pDoc->m_selectArray.GetSize() == 0)
	{
		CString	strError = _T("������ѡȡͼԪ");	
		AfxMessageBox(strError);
		return;
	}
	// ������ݲ�ͬ�Ĳ˵��������ͬ���������
	switch(m_nID)
	{
		case ID_MODIFY_MOVE: 
		{// ƽ��
			m_pCmd = new CMove();
			break;
		}
		case ID_MODIFY_ROTATE: 
		{// ��ת
			m_pCmd = new CRotate();
			break;
		}
		case ID_MODIFY_MIRROR: 
		{// ����
			m_pCmd = new CMirror();
			break;
		}
		default:
			//ɾ��
			Erase() ;
			break;
	}
}
void CVCadView::OnUpdateModifyCommand(CCmdUI* pCmdUI)
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT(pDoc) ;

	// �ж��Ƿ���ʵ��ͼԪ��ѡ��
	if(pDoc->m_selectArray.GetSize() == 0)
	{
		pCmdUI->Enable(FALSE);		
		return;
	}

	int flag = 0 ;
	switch(pCmdUI->m_nID)
	{
		case ID_MODIFY_MOVE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctMove) )
				flag = 1;
			break;
		}
		case ID_MODIFY_ROTATE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctRotate) )
				flag = 1;
			break;
		}
		case ID_MODIFY_MIRROR:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctMirror) )
				flag = 1;
			break;
		}
		default:
			break;
	}
	pCmdUI->SetCheck(flag);
}

void CVCadView::WorldtoScreen(const Position& pos, CPoint& screenPt)
{
	// ��ȡ��ǰ�ͻ����Ĵ�С
	CRect rect ;
	GetClientRect(&rect) ;
	// ����Ļԭ������Ϊ�ͻ���������
	int	nSOrgX = int((rect.left + rect.right) / 2 );///scale);
	int nSOrgY = int((rect.top + rect.bottom) / 2 );///scale);
	// ������Ļ����ֵ
	screenPt.x = (int)((pos.x - m_dOrgX) / scale + nSOrgX) ;
	screenPt.y = (int)(nSOrgY - (pos.y - m_dOrgY) / scale ) ;
	CDC* pDC = GetDC();
	
	if(m_bIsPrinting)
		pDC->SetMapMode(MM_LOENGLISH); 
	else
		pDC->SetMapMode(MM_TEXT); 
	pDC->LPtoDP(&screenPt);
	ReleaseDC(pDC);	
}

void CVCadView::ScreentoWorld(const CPoint& pt, Position& pos)
{
	CPoint screenPt = pt;
	CDC* pDC = GetDC();
	pDC->SetMapMode(MM_TEXT);

	pDC->DPtoLP(&screenPt);
	ReleaseDC(pDC);	
	// ��ȡ��ǰ�ͻ����Ĵ�С
	CRect rect ;
	GetClientRect(&rect) ;
	// ����Ļԭ������Ϊ�ͻ���������
	int	nSOrgX = (rect.left + rect.right) / 2 ;
	int nSOrgY = (rect.top + rect.bottom) / 2 ;
	// ������������ֵ
	pos.x = (screenPt.x - nSOrgX) * scale + m_dOrgX;
	pos.y = (nSOrgY - screenPt.y ) * scale + m_dOrgY;
}

double CVCadView::GetScale()
{
	return scale;
}

void CVCadView::SetScale(double dScale)
{
	scale = dScale;
}

void CVCadView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!m_bEditable)
		return;

	CDC * pDC = GetDC();
	m_basePos = m_desPos = point;

	// TODO: Add your message handler code here and/or call default
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	Position pos;
	ScreentoWorld(point, pos); // ���豸����ת��Ϊ��������

	if(m_pCmd) {
		m_pCmd->OnLButtonDown(nFlags, pos);
	}
	else
	{	
		//ȱʡ�ƶ�����
		if( (! (nFlags & MK_CONTROL)) && pDoc->m_selectArray.GetSize() >= 1)
			m_bLBPressed = TRUE;
		else //ѡ��
			pDoc->OnLButtonDown(nFlags, pos);		
	}

	ReleaseDC(pDC);
	CView::OnLButtonDown(nFlags, point);
}

void CVCadView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(!m_bEditable)
		return;

	// TODO: Add your message handler code here and/or call default
	Position pos;
	// ���豸����ת��Ϊ��������
	ScreentoWorld(point, pos);

	if(m_pCmd)
		m_pCmd->OnLButtonDblClk(nFlags, pos);
	else {
		//����ĵ����ָ��
		CVCadDoc * pDoc = GetDocument() ;
		ASSERT_VALID(pDoc) ;
		
		pDoc->OnLButtonDown(nFlags, pos);
		
		MEntity* pSelEntity = NULL ;
		POSITION pos = NULL ;
		if(pDoc->m_selectArray.GetSize() == 1) {
			pSelEntity = (MEntity*)pDoc->m_selectArray[0] ;
			if(pSelEntity->GetType() == etText) {
				CTextInputDlg dlg;
				CString oldText = ((MText*)pSelEntity)->GetText();
				dlg.m_text = oldText;
				dlg.DoModal();
				
				((MText*)pSelEntity)->SetText(dlg.m_text);
			}
		}
		//���ѡ��
		pDoc->m_selectArray.RemoveAll() ;
		//�����ĵ������»���
		pDoc->UpdateAllViews(NULL) ;

	}
	
	CView::OnLButtonDblClk(nFlags, point);
}

void CVCadView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!m_bEditable)
		return;

	// TODO: Add your message handler code here and/or call default
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	Position pos;
	ScreentoWorld(point, pos);// ���豸����ת��Ϊ��������

	//���״̬����ָ��
	CStatusBar* pStatus=(CStatusBar*)
		AfxGetApp()->m_pMainWnd->GetDescendantWindow(ID_VIEW_STATUS_BAR);
	if(pStatus)
	{
		char tbuf[40];
		sprintf(tbuf,"(%8.3f,%8.3f)",pos.x, pos.y);
		//��״̬���ĵڶ��������������ǰ����λ��
		pStatus->SetPaneText(1,tbuf);
	}
	if(m_pCmd)
		m_pCmd->OnMouseMove(nFlags, pos);
	else {
		if(!m_bDragging)
			pDoc->OnMouseMove(nFlags, pos);
	}

	if(m_bLBPressed) {//�϶�ʵ��
			Position	prePos, curPos, basePos;
			ScreentoWorld(m_basePos, basePos);
			ScreentoWorld(m_desPos, prePos); // �����һ��Ŀ��λ��
			curPos = pos; // �õ���ǰλ��
			

			CDC*	pDC = g_pView->GetDC(); 
			// ���ݵ�ǰλ�ø���ѡ��ͼԪ��ʵʱλ��
			int i, n;
			for(n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
			{
				MEntity*	pEntity = (MEntity*)g_pDoc->m_selectArray[i];
				pEntity->Draw(pDC,dmSelect);
				// ����ڲ��������д���û�б�ˢ�£���Ҫ�����һ��λ���ϻ��Ƶ�ͼԪ
				if(1) 
				{
					MEntity*	pCopyEntity1 = pEntity->Copy(); // �õ�ͼԪ�Ŀ���
					pCopyEntity1->Move(basePos, prePos, TRUE); // �������ƶ�����һ��λ��
					pCopyEntity1->Draw(pDC,dmDrag); // ������һ��λ�öԿ��������ػ�
					delete pCopyEntity1; // ɾ����ʱ����
				}
				// �ڵ�ǰλ���ϻ���ͼԪ
				MEntity*	pCopyEntity2 = pEntity->Copy();// �õ�ͼԪ�Ŀ���
				pCopyEntity2->Move(basePos, curPos, TRUE);// �������ƶ�����ǰλ��
				pCopyEntity2->Draw(pDC,dmDrag);	// �Ե�ǰλ�û��ƿ���
				delete pCopyEntity2; // ɾ����ʱ���� 
			}
			g_pView->ReleaseDC(pDC);
			m_desPos = point; // ��Ŀ������Ϊ��ǰλ��

			if(pos.Distance(prePos) > 0.5)
				m_bDragging = TRUE;
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CVCadView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(!m_bEditable)
		return;

	// TODO: Add your message handler code here and/or call default
	Position pos;
	// ���豸����ת��Ϊ��������
	ScreentoWorld(point, pos);

	if(m_pCmd)
		m_pCmd->OnRButtonDown(nFlags, pos);

	CView::OnRButtonDown(nFlags, point);
}

void CVCadView::OnPick() 
{
	// TODO: Add your command handler code here
	if(m_pCmd){

		delete m_pCmd;
		m_pCmd = NULL;
	}
}

void CVCadView::OnUpdatePick(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pCmd == NULL ? 1 : 0) ;
}
/////////////////////////////////////////////////////////////////////////////
// CVCadView printing

BOOL CVCadView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// ����ͼ������Ϊ2����׼ҳ��С
	pInfo->SetMaxPage(2);
	return DoPreparePrinting(pInfo);
}

void CVCadView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
	// ��ÿɴ�ӡ���򣨺���ֵ��
	int nHorzSize = pDC->GetDeviceCaps(HORZSIZE);
	int nVertSize = pDC->GetDeviceCaps(VERTSIZE);
	// ����ҳ��ĸ߶ȺͿ�ȣ��߼���λ��  
	m_nPageWidth = (int)((double)nHorzSize / 25.4 * 100.0);
	m_nPageHeight = (int)((double)nVertSize / 25.4 * 100.0);
}

void CVCadView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CVCadView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	pDC->SetMapMode(MM_TEXT); 
	if(pDC->IsPrinting())
	{
		pDC->SetMapMode(MM_LOENGLISH); 
		int nPages = pInfo->m_nCurPage - 1;
		int x = (nPages & 1) * m_nPageWidth;
		int y = (nPages / 2) * m_nPageHeight;
		pDC->SetWindowOrg(x,y);
	}	
	CView::OnPrepareDC(pDC, pInfo);

}
void CVCadView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	ASSERT_VALID(pDC);
	pDC->SetWindowOrg(pInfo->m_rectDraw.left,
		-pInfo->m_rectDraw.top);
	
	m_bIsPrinting=TRUE;	
	// ��ӡҳü
	PrintPageHeader(pDC,pInfo); 
	// ��ӡͼ��
	CView::OnPrint(pDC, pInfo); 
	// ��ӡҳ��
	PrintPageFooter(pDC,pInfo);

	m_bIsPrinting=FALSE;
}


void CVCadView::PrintPageHeader(CDC *pDC, CPrintInfo *pInfo)
{
	// ��ȡ�豸��������ȱʡ������
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	// ���������岢����ѡ���豸������
	CFont newFont;
	newFont.CreateFont(-tm.tmHeight,
		0,0,0,
		FW_NORMAL,
		FALSE, FALSE,
		0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN,
		"Courier New");
	CFont* pOldFont = (CFont*)pDC->SelectObject(&newFont);

	// ҳü�е��ı��Ҷ���
	pDC->SetTextAlign(TA_RIGHT);
	// ��ӡҳü�ı�
	pDC->TextOut(pInfo->m_rectDraw.right, -25, "VCad ��ͼϵͳ");

	// ��ӡҳü�е�ˮƽֱ��
	int y = - 35 - tm.tmHeight;
	pDC->MoveTo(0,y);
	pDC->LineTo(pInfo->m_rectDraw.right,y);

	// ����ҳüռ�õĿռ��������ɴ�ӡͼ������
	y -= 25;
	pInfo->m_rectDraw.top += y;

	// �ָ���ǰ������
	pDC->SelectObject(pOldFont);
}
// ��ӡҳ��
void CVCadView::PrintPageFooter(CDC *pDC, CPrintInfo *pInfo)
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	// ���ĵ������λ�û��߱���͵�ǰ��ҳ�빹��ҳ�ŵ��ı�
	// ����ĵ�û�б����棬���ӡ�ĵ��ı������ƣ���֮�����ӡ�ĵ������λ��
	CString strFooter = pDoc->GetPathName();
	CString strTitle = pDoc->GetTitle();
	if(strTitle == "Untitled" || strTitle == "�ޱ���")
		strFooter = strTitle;
	
	// �ı������
	pDC->SetTextAlign(TA_LEFT);
	// ��ӡ�ı�
	pDC->TextOut(pInfo->m_rectDraw.left,
		pInfo->m_rectDraw.bottom + 100, strFooter);
	
	// ��ҳ�������ҳ����Ϣ
	CString strPage;
	strPage.Format("%s%d%s", "��", pInfo->m_nCurPage, "ҳ");
	// ��ȡҳ��Ŀ��
	CSize size = pDC->GetTextExtent(strFooter);
	// ҳ���Ҷ���
	pDC->TextOut(pInfo->m_rectDraw.right - size.cx,
		pInfo->m_rectDraw.bottom + 100, strPage);

	// ����ҳ�ŵ�ˮƽ�ϻ���
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int y = pInfo->m_rectDraw.bottom + 90 + tm.tmHeight;
	pDC->MoveTo(0, y);
	pDC->LineTo(pInfo->m_rectDraw.right, y);
}


void CVCadView::OnRedraw() 
{
	// TODO: Add your command handler code here
	g_pDoc ->UpdateAllViews(NULL) ;
}
//
void CVCadView::OnViewCommand(UINT m_nID) 
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT(pDoc) ;

	CRect rc ;
	GetClientRect(&rc) ;

	if( m_pCmd ){
		delete m_pCmd ;
		// �мǣ���ʹ��һ������֮ǰ����Ҫ�����һ������
		m_pCmd = NULL; 
	}
	switch(m_nID)
	{
		case ID_VIEW_REGION:
		{
			if( !(m_pCmd = new CZoomRgnCmd()) )
				AfxMessageBox("Failed!");
//			pDoc->SetModifiedFlag(TRUE) ;
			::Prompt("�������ƶ�����ʼ�㣺") ;
			break;
		}
		case ID_VIEW_ZOOMIN:
		{
			scale *= 0.618 ;
			if(scale<1e-6) scale = 1. ;
//			pDoc->SetModifiedFlag(TRUE) ;
			break;
		}
		case ID_VIEW_ZOOMOUT:
		{
			scale/=0.618 ;
			if(scale>1e+6) scale = 1. ;
//			pDoc->SetModifiedFlag(TRUE) ;
			break;	
		}
		case ID_VIEW_ZOOMALL:
		{
			ZoomAll() ;
			break;
		}
		case ID_VIEW_PAN:
		{
			if(!(m_pCmd = new CViewPanCmd()))
				AfxMessageBox("Failed!") ;
//			pDoc->SetModifiedFlag(TRUE) ;
			break;
		}
		default:
			break;
	}
	InvalidateRect(rc) ;
}
//
void CVCadView::ZoomAll()
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CDC* pDC = GetDC() ;
	//ӳ��ģʽҪһ��
	pDC->SetMapMode(MM_TEXT) ;

	CRect rc ;
	GetClientRect(&rc) ;
	pDC->DPtoLP(&rc) ;
	
	double MinX, MaxX, MinY, MaxY ;
	MinX = 1.e+6 ;
	MinY = 1.e+6 ;
	MaxX = - 1.e+6 ;
	MaxY = - 1.e+6 ;

	BOX2D pBox ;
	POSITION pos;
	pos = pDoc->m_EntityList.GetHeadPosition();
	// ��������в������κ�ͼԪ�������κβ���ֱ�ӷ���
	if(pos == NULL)  
		return ;
	// ͨ����ͼԪ�İ�Χ�������������ͼԪ�İ�Χ��
	while(pos != NULL)
	{
		MEntity* pEntity = (MEntity*)(pDoc->m_EntityList.GetNext(pos)) ;
		pEntity->GetBox(&pBox) ;
		if(MinX>pBox.min[0]) MinX = pBox.min[0] ;
		if(MinY>pBox.min[1]) MinY = pBox.min[1] ;
		if(MaxX<pBox.max[0]) MaxX = pBox.max[0] ;
		if(MaxY<pBox.max[1]) MaxY = pBox.max[1] ;
	}
	//���¼���ԭ��
	m_dOrgX = (MaxX + MinX)/2. ;
	m_dOrgY = (MaxY + MinY)/2. ;
	//���¼����������
	double scalex = fabs((MaxX - MinX)/(rc.right-rc.left)) ;
	double scaley = fabs((MaxY - MinY)/(rc.bottom-rc.top )) ;
	if(scalex>scaley)
		scale = scalex ;
	else
		scale = scaley ;

	if(fabs(scale)<1e-6||fabs(scale)>1e+6)
		scale = 1. ;
	InvalidateRect(rc) ;
}

void CVCadView::DrawCoord(CDC *pDC)
{
	CVCadDoc * pDoc = GetDocument() ;
	ASSERT_VALID(pDoc) ;

	CPoint sOrgp, sXp, sYp;
	Position dOrgPnt(0,0) ;

	WorldtoScreen(dOrgPnt,sOrgp) ;
	pDC->TextOut(sOrgp.x-20,sOrgp.y-2,"O") ;

	Position dtemppx(50,0) ;
	WorldtoScreen(dtemppx,sXp) ;
	pDC->TextOut(sXp.x+2,sXp.y-2,"X") ;

	Position dtemppy(0,50) ;
	WorldtoScreen(dtemppy,sYp) ;
	pDC->TextOut(sYp.x-20,sYp.y-20,"Y") ;

	pDC->MoveTo(sOrgp) ;
	pDC->LineTo(sXp) ;
	pDC->MoveTo(sOrgp) ;
	pDC->LineTo(sYp) ;

	//����ͷ
	CBrush * pnew = new CBrush;
	pnew->CreateSysColorBrush(BLACK_BRUSH);
	CBrush* pbrsh = (CBrush*) pDC->SelectObject(pnew);
	POINT* pts = new POINT[3];
	pts[0] = sXp;
	pts[1] = sXp; pts[1].x -= 5 / scale; pts[1].y -= 3 / scale;
	pts[2] = sXp; pts[2].x -= 5 / scale; pts[2].y += 3 / scale;
	pDC->Polygon(pts, 3);

	pts[0] = sYp;
	pts[1] = sYp; pts[1].x -= 3 / scale; pts[1].y += 5 / scale;
	pts[2] = sYp; pts[2].x += 3 / scale; pts[2].y += 5 / scale;
	pDC->Polygon(pts, 3);
	pDC->SelectObject(pbrsh);

	delete pts;
	delete pnew;
}

void CVCadView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_NEXT:
		scale *= 0.618 ;
		if(scale<1e-6) scale = 1. ;
//		g_pDoc->SetModifiedFlag(TRUE) ;
		Invalidate(TRUE) ;
		break;
	case VK_PRIOR:
		scale /= 0.618 ;
		if(scale>1e+6) scale = 1. ;
//		g_pDoc->SetModifiedFlag(TRUE) ;
		Invalidate(TRUE) ;
		break;
	default:;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
//��ʱɾ������
void CVCadView::Erase()
{
	//����ĵ����ָ��
	CVCadDoc * pDoc = GetDocument() ;
	ASSERT_VALID(pDoc) ;
	
	MEntity* pSelEntity = NULL ;
	POSITION pos = NULL ;
	//�������в��ұ���ӵ�ѡ����ͼԪ
	//���������������Զɾ��
	if(pDoc->m_selectArray.GetSize() > 0)
	{
		g_pDoc->SetModifiedFlag(TRUE);
		m_nCurrentOperation++;
		m_nOperations = m_nCurrentOperation;
		g_pDoc->m_clpArray.RemoveAll();
	}
	
	for( int i = 0 ; i < pDoc->m_selectArray.GetSize() ; i++ )
	{
		pSelEntity = (MEntity*)pDoc->m_selectArray[i] ;
//		pos = pDoc->m_EntityList.Find(pSelEntity) ;
//		pDoc->m_EntityList.RemoveAt(pos) ;
//		delete pSelEntity ;
//		pSelEntity = NULL ;
		//pDoc->SetModifiedFlag(TRUE);
			MEntity *   pShowEntity = pSelEntity->GetCurrentEnt();
			if(pShowEntity != NULL) {
				MEntity * pNext = pShowEntity->next;
				while(pNext) {
					MEntity * pTmp = pNext;
					pNext = pNext->next;
					delete pTmp;
				}
				pShowEntity->next = NULL;
			}

		MEntity* pEraseEnt = pShowEntity->Erase();
//		pEraseEnt->m_bErased = TRUE;
		pShowEntity->next = pEraseEnt;
		pEraseEnt->m_nOperationNum = m_nCurrentOperation;
	}

	//���ѡ��
	pDoc->m_selectArray.RemoveAll() ;
	//�����ĵ������»���
	pDoc->UpdateAllViews(NULL) ;
}

void CVCadView::OnUpdateModifyErase(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	OnUpdateEditCopy(pCmdUI);
}

//	���ļ�	
void CVCadView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CVCadDoc* pDocument = GetDocument();

	BOOL	ret = FALSE;

	//	�����ǰ�ļ�Ϊ�������ȱ���
	int	status = 0;
	ret = pDocument->IsModified();
	if(ret == TRUE) 
	{
	    status = AfxMessageBox(" �ļ����޸�,�Ƿ񱣴��ļ�?",MB_YESNOCANCEL);
	    if(status == IDCANCEL) 
		{
			return;
	    }
	    if(status == IDYES) 
		{
			AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND,ID_FILE_SAVE);
	    }
	}

	extern BOOL GetCADName(BOOL bIsOpen,CString& sFileName);	//	�õ�CAD�ļ���
	CString sFileName;
    if (GetCADName(TRUE,sFileName))
	{
		pDocument->DeleteContents();
//		pDocument->SetModifiedFlag();

		CFileException fe;
		CFile* pFile = pDocument->GetFile(sFileName,
			CFile::modeRead|CFile::shareDenyWrite, &fe);
		if (pFile == NULL)
		{
			pDocument->ReportSaveLoadException(sFileName, &fe,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		}

		CArchive loadArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
		loadArchive.m_pDocument = pDocument;
		loadArchive.m_bForceFlat = TRUE;

		//	��ȡ�ļ�
		if (pFile->GetLength() != 0)
		{
			GetDocument()->BeginWaitCursor();
			pDocument->Serialize(loadArchive);
			GetDocument()->EndWaitCursor();
		}

		pDocument->SetTitle(sFileName);	//	���ô��ڱ���
		AfxGetApp()->AddToRecentFileList(sFileName);	//	��������ļ��б�

		loadArchive.Close();
		pDocument->ReleaseFile(pFile, FALSE);

		pDocument->SetModifiedFlag(FALSE);

		Invalidate();	//	ˢ�»�ͼ��
	}
}

//	�����ļ�
void CVCadView::OnFileSave() 
{
	// TODO: Add your command handler code here
	CVCadDoc* pDoc = GetDocument();
	CFileException fe;
	
	//	����ļ�δ��������������Ϊ������	
	CString sFileName=pDoc->GetTitle();
	sFileName.MakeLower();
	if(sFileName=="untitled" || sFileName == "�ޱ���")
	{
		OnFileSaveAs();
		return;
	}

	//	�����ļ�
	CFile* pFile = new CFile(sFileName, CFile::modeCreate |
		CFile::modeWrite | CFile::shareExclusive);//, &fe);

	if (pFile == NULL)
		return;

	CArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
	saveArchive.m_pDocument = pDoc;
	saveArchive.m_bForceFlat = TRUE;

	//	�����ļ�
	pDoc->Serialize(saveArchive);	

	//�����ĵ����±��
	pDoc->SetModifiedFlag(FALSE);

	saveArchive.Close();
	pDoc->ReleaseFile(pFile, FALSE);

}

//	����ļ�
void CVCadView::OnFileSaveAs() 
{
	CVCadDoc* pDoc = GetDocument();

	// TODO: Add your command handler code here
	extern BOOL GetCADName(BOOL bIsOpen,CString& sFileName);	//	�õ�CAD�ļ���
	CString sFileName;
    if (GetCADName(FALSE,sFileName))
	{

		CFileException fe;
		CFile* pFile = new CFile(sFileName, CFile::modeCreate |
			CFile::modeWrite | CFile::shareExclusive);//, &fe);

		if (pFile == NULL)
			return;

		CArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
		saveArchive.m_pDocument = pDoc;
		saveArchive.m_bForceFlat = TRUE;
		TRY
		{
			pDoc->Serialize(saveArchive);    

			pDoc->SetTitle(sFileName);
			AfxGetApp()->AddToRecentFileList(sFileName);

			saveArchive.Close();
			pDoc->ReleaseFile(pFile, FALSE);
		}
		CATCH_ALL(e)
		{
			pDoc->ReleaseFile(pFile, TRUE);

			TRY
			{
				pDoc->ReportSaveLoadException(sFileName, e,
					TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
			}
			END_TRY
			e->Delete();
			return;
		}
		END_CATCH_ALL

		pDoc->SetModifiedFlag(FALSE);    

    }

}

void CVCadView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(!m_bEditable)
		return;

	// TODO: Add your message handler code here and/or call default
	m_desPos = point;
	Position basePos, desPos;
	ScreentoWorld(m_basePos, basePos);
	ScreentoWorld(m_desPos, desPos);
	CDC*	pDC = g_pView->GetDC(); // ���������豸����ָ��

	// ��ѡ���е�ͼԪ�ƶ���Ŀ��λ�ò����л���
	int i, n;
	if(m_bDragging) {
		for(n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity*	pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			pEntity->Draw(pDC,dmInvalid); // ���ԭ��λ���ϵ�ͼԪ
			pEntity = pEntity->GetShowEnt();
			BOX2D box;
			pEntity->GetBox(&box);
			CPoint p1, p2;
			WorldtoScreen(box.min, p1);
			WorldtoScreen(box.max, p2);
			CRect rect(p1, p2);
			InvalidateRect(rect, 1);
//			pEntity->Move(basePos, desPos); // ��ͼԪ�ƶ���Ŀ��λ��
//			pEntity->Draw(pDC,dmNormal); // ��Ŀ��λ���ϻ���ͼԪ

			MEntity * pNewEnt = pEntity->Copy();
			pNewEnt->Move(basePos, desPos); // ��ͼԪ�ƶ���Ŀ��λ��
			pNewEnt->Draw(pDC,dmNormal); // ��Ŀ��λ���ϻ���ͼԪ
			g_pDoc->SetModifiedFlag(TRUE); // ��־�ĵ������ѱ��޸�
			pNewEnt->m_nOperationNum = g_pView->m_nCurrentOperation;
			pEntity->Append(pNewEnt);
		}
		g_pDoc->m_selectArray.RemoveAll(); //  ���ѡ��
//		g_pDoc->SetModifiedFlag(TRUE); // ��־�ĵ������ѱ��޸�
		g_pView->ReleaseDC(pDC); // �ͷ�������豸����ָ��
	}

	m_bDragging  = FALSE;
	m_bLBPressed = FALSE;
	//Invalidate(1);
	CView::OnLButtonUp(nFlags, point);
}

void CVCadView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	for( int i = 0 ; i < g_pDoc->m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_selectArray[i] ;
		g_pDoc->m_clpArray.Add(pSelEntity);
	}
}

void CVCadView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_pDoc->m_selectArray.GetSize() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);		
}

void CVCadView::OnEditCut() 
{
	// TODO: Add your command handler code here
	for( int i = 0 ; i < g_pDoc->m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_selectArray[i] ;
		g_pDoc->m_clpArray.Add(pSelEntity);

			MEntity *   pShowEntity = pSelEntity->GetCurrentEnt();
			if(pShowEntity != NULL) {
				MEntity * pNext = pShowEntity->next;
				while(pNext) {
					MEntity * pTmp = pNext;
					pNext = pNext->next;
					delete pTmp;
				}
				pShowEntity->next = NULL;
			}

		MEntity* pEraseEnt = pShowEntity->Erase();
		pShowEntity->next = pEraseEnt;
		pEraseEnt->m_nOperationNum = m_nCurrentOperation + 1;
	}

	m_nCurrentOperation++;
	m_nOperations = m_nCurrentOperation;
	g_pDoc->m_selectArray.RemoveAll();
	Invalidate(TRUE);
}

void CVCadView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_pDoc->m_selectArray.GetSize() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);	
}

void CVCadView::OnEditPaste() 
{
	CDC *pDC = GetDC();
	// TODO: Add your command handler code here
	for( int i = 0 ; i < g_pDoc->m_clpArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_clpArray[i] ;
		MEntity *   pCopyEntity = pSelEntity->GetCurrentEnt();
		ASSERT_VALID(pCopyEntity);
		MEntity * pNew = pCopyEntity->Copy();
		pNew->Move(Position(0,0), Position(10,10));
		pNew->Draw(pDC, dmNormal);
		g_pDoc->m_EntityList.AddTail(pNew); // ��ָ����ӵ�ͼԪ����
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pNew->m_nOperationNum = g_pView->m_nCurrentOperation;
	}

	ReleaseDC(pDC);
}

void CVCadView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_pDoc->m_clpArray.GetSize() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CVCadView::OnEditRedo() 
{
	// TODO: Add your command handler code here
	m_nCurrentOperation++;
	g_pDoc->m_selectArray.RemoveAll();
	g_pDoc->m_clpArray.RemoveAll();
	Invalidate(TRUE);
}

void CVCadView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nOperations > m_nCurrentOperation)
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);		
}

void CVCadView::OnEditUndo() 
{
	// TODO: Add your command handler code here
	m_nCurrentOperation--;
	g_pDoc->m_selectArray.RemoveAll();
	g_pDoc->m_clpArray.RemoveAll();
	Invalidate(TRUE);
}

void CVCadView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nCurrentOperation > m_nStartOperateNum)
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}


void CVCadView::OnEditManhadun() //�����ٻ�
{
	CDC * pDC = GetDC();
	//�㷨������㿪ʼ�����&x > &y������X�ߣ��������ͬ����ӣ��������ͬX��ͬY��ı���һ���Y��X;
	//      ����յ㣬�㷨������
	for( int i = 0 ; i < g_pDoc->m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_selectArray[i] ;
		if(pSelEntity->GetType() == etConnect)
		{
			pSelEntity->Draw(pDC, dmInvalid);
			MEntity * pManhadun = pSelEntity->Copy();
			((MConnect*)pManhadun)->Manhadunize();
			pManhadun->Draw(pDC,dmNormal); // ��Ŀ��λ���ϻ���ͼԪ
			g_pDoc->SetModifiedFlag(TRUE); // ��־�ĵ������ѱ��޸�
			pManhadun->m_nOperationNum = g_pView->m_nCurrentOperation;
			pSelEntity->Append(pManhadun);
		}
	}
	ReleaseDC(pDC);
	g_pDoc->m_selectArray.RemoveAll();
	Invalidate(FALSE);
}

void CVCadView::OnUpdateEditManhadun(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	//���ѡ�е�ʵ�弯�������ӣ�����Ч��������Ч
	BOOL bValid = FALSE;
	for( int i = 0 ; i < g_pDoc->m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_selectArray[i] ;
		if(pSelEntity->GetType() == etConnect)
		{
			bValid = TRUE;
			break;
		}
	}

	pCmdUI->Enable(bValid);
}


void CVCadView::AddEntity(MEntity* pEnt)
{
	g_pDoc->m_EntityList.AddTail(pEnt); 
	g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
	pEnt->m_nOperationNum = g_pView->m_nCurrentOperation;
}

void CVCadView::OnMenuitem32807() 
{
	// TODO: Add your command handler code here
	CECT2DModelDlg dlg(this);
	
	if(dlg.DoModal() == IDCANCEL)
	{ 
		return ;
	}
}

void CVCadView::OnMenuitem32808() 
{
	// TODO: Add your command handler code here
	//�ݿ�ϵ������ĳ���
	CGearParamDlg dlg;
	dlg.DoModal();
}




void CVCadView::OnMenuitem32809() 
{
	// ˫���Բ�ֵ
	CInterDlg dlg;

	dlg.DoModal();	
}

void CVCadView::OnMenuitem32810() 
{
	//��С����
	OnCreateEntity(32810);
}

void CVCadView::OnMenuitem32811() 
{
	//Bezier
	OnCreateEntity(32811);
}

void CVCadView::OnMenuitem32812() 
{
	//Bspline
	OnCreateEntity(32812);
}


extern void bezsurf(double b[], int n, int m, int p1, int p2, double q[]);
extern void rbspsurf(double b[], int k, int l, int npts, int mpts, int p1, int p2, double q[]);

//draw bezier surface
void CVCadView::OnMenuitem32813() 
{
	// TODO: Add your command handler code here
	int i,j;
    int n = 3;    //the order of u-direction
	int m = 4;    //the order of w-direction
    int p1 = 17;  //the subdivision number of u-direction
	int p2 = 21;  //the subdivision number of w-direction
	double q[1072];   //21*17*3+1
	//b[(4+1)*(3+1)+1]
/*	double b[61]={0, -300,200,0,    -150,200,200,   0,200,-200,  150,200,200,   300,200,0,        //row-1: 5 points
	               	 -300,80,200,   -150,80,-200,   0,80,200,    150,80,-200,   300,80,200,       //row-2: 5 points
					 -300,-80,-200, -150,-80,200,   0,-80,-200,  150,-80,200,   300,-80,-200,     //row-3: 5 points
					 -300,-200,0,   -150,-200,-200, 0,-200,200,  150,-200,-200, 300,-200,0};      //row-4: 5 points
*/
	double b[61]={0, -300,200,0,    -150,200,200,   0,200,300,   150,200,200,   300,200,0,        //row-1: 5 points
	               	 -300,80,200,   -150,80,300,    0,80,400,    150,80,300,    300,80,200,       //row-2: 5 points
					 -300,-80,200,  -150,-80,300,   0,-80,400,   150,-80,300,   300,-80,200,     //row-3: 5 points
					 -300,-200,0,   -150,-200,200,  0,-200,300,  150,-200,200,  300,-200,0};      //row-4: 5 points

	//transform around z-axis, y-axis and transfer [x,y,z]=[450,350,0]
	Vector vec(0,0,0);
	for(i=0; i<20; i++)
	{
		vec.xyz(b[i*3+1],b[i*3+2],b[i*3+3]);
		vec.rotate(70,0,40);    //rotate 20 around x-axis,   rotate 30 around z-axis
		b[i*3+1] = vec.x()+450;
		b[i*3+2] = vec.y()+150;
	}	
	
	//draw control polygon
	CDC *pDC = GetDC();
	for(j = 0; j < 4; j++)
	{
		pDC->MoveTo(b[j*15+1],b[j*15+2]);
		for(i = 1; i < 5; i++)
		   pDC->LineTo(b[j*15+i*3+1],b[j*15+i*3+2]);
	}
	for(j = 0; j < 5; j++)
	{
		pDC->MoveTo(b[j*3+1],b[j*3+2]);
		for(i = 1; i < 4; i++)
		   pDC->LineTo(b[i*15+j*3+1],b[i*15+j*3+2]);
	}

	//draw bezier surface  
    bezsurf(b, n, m, p1, p2, q);
	for(j = 0; j < p1; j++)
	{
		pDC->MoveTo(q[j*p2*3+1],q[j*p2*3+2]);
		for(i = 1; i < p2; i++)
		   pDC->LineTo(q[j*p2*3+i*3+1],q[j*p2*3+i*3+2]);
	}
	for(j = 0; j < p2; j++)
	{
		pDC->MoveTo(q[j*3+1],q[j*3+2]);
		for(i = 1; i < p1; i++)
		   pDC->LineTo(q[i*p2*3+j*3+1],q[i*p2*3+j*3+2]);
	}

	ReleaseDC(pDC);
}

//Bspline surface
void CVCadView::OnMenuitem32814() 
{
	int i,j;
	int npts,mpts;
	int k,l;
	int p1,p2;

	double b[65];
	double q[1072];   //21*17*3+1

	npts = 4;
	mpts = 4;
	k = 3;
	l = 3;
	p1 = 17;
	p2 = 21;

	for (i = 1; i <= mpts*npts; i++)
		b[i] = 0.;
	for (i = 1; i <= 3*p1*p2; i++)
		q[i] = 0.;

	//	This is a standard test control polygon from Ex. 6.1, p. 184
	//	Comment out to use file input <filename>.rnp. Data is in the
	//	form x=b[1], y=b[2], z=b[3], h=b[4], etc. All h are 1.0.
	//	Thus, this is a nonrational B-spline surface and the results
	//	should be the same as for tbsurf.c
	b[1] = -150.;	b[2] = 0.;	   b[3] = 150.;	   b[4] = 1;  	
	b[5] = -15.;	b[6] = 50.;    b[7] = 50.;     b[8] = 1; 
    b[9] = -150.;   b[10] = 50.;   b[11] = -50.;   b[12] = 1;
	b[13] = -150.;	b[14] = 0.;    b[15] = -150.;  b[16] = 1; 
	b[17] = -50.;	b[18] = 50.;   b[19] = 150.;   b[20] = 1; 
	b[21] = -50.;   b[22] = 100.;  b[23] = 50.;    b[24] = 1;
	b[25] = -50.;	b[26] = 100.;  b[27] = -50.;   b[28] = 1;
	b[29] = -50.;	b[30] = 50.;   b[31] = -150.;  b[32] = 1;
    b[33] = 50.;    b[34] = 50.;   b[35] = 150.;   b[36] = 1;
	b[37] = 50.; 	b[38] = 100.;  b[39] = 50.;    b[40] = 1;
	b[41] = 50.; 	b[42] = 100.;  b[43] = -50.;   b[44] = 1;
    b[45] = 50.;    b[46] = 0.;	   b[47] = -150.;  b[48] = 1;
	b[49] = 150.;	b[50] = 0.;    b[51] = 150.;   b[52] = 1; 
	b[53] = 150.;	b[54] = 50.;   b[55] = 50.;    b[56] = 1;
 	b[57] = 150.;	b[58] = 50.;   b[59] = -50.;   b[60] = 1;
	b[61] = 150.;	b[62] = 0.;	   b[63] = -150.;  b[64] = 1;

		//transform around z-axis, y-axis and transfer [x,y,z]=[300,200,0]
	Vector vec(0,0,0);
	for(i=0; i<16; i++)
	{
		vec.xyz(b[i*4+1],b[i*4+2],b[i*4+3]);
		vec.rotate(0,60,30);    //rotate 20 around x-axis,   rotate 30 around z-axis
		b[i*4+1] = vec.x()+300;
		b[i*4+2] = vec.y()+200;
	}	

	//draw control polygon
	CDC * pDC = GetDC();
	for(j = 0; j < 4; j++)
	{
		pDC->MoveTo(b[j*16+1],b[j*16+2]);
		for(i = 1; i < 4; i++)
		   pDC->LineTo(b[j*16+i*4+1],b[j*16+i*4+2]);
	}
	for(j = 0; j < 4; j++)
	{
		pDC->MoveTo(b[j*4+1],b[j*4+2]);
		for(i = 1; i < 4; i++)
		   pDC->LineTo(b[i*16+j*4+1],b[i*16+j*4+2]);
	}
	//draw RBspline surface  
	rbspsurf(b,k,l,npts,mpts,p1,p2,q);
	for(j = 0; j < p1; j++)
	{
		pDC->MoveTo(q[j*p2*3+1],q[j*p2*3+2]);
		for(i = 1; i < p2; i++)
		   pDC->LineTo(q[j*p2*3+i*3+1],q[j*p2*3+i*3+2]);
	}
	for(j = 0; j < p2; j++)
	{
		pDC->MoveTo(q[j*3+1],q[j*3+2]);
		for(i = 1; i < p1; i++)
		   pDC->LineTo(q[i*p2*3+j*3+1],q[i*p2*3+j*3+2]);
	}

	ReleaseDC(pDC);
}

//B��������
void CVCadView::OnMenuitem32815() 
{
	// TODO: Add your command handler code here
	OnCreateEntity(32815);
}


void CVCadView::OnInnerpt() 
{
	OnCreateEntity(32816);
	//����ʶ��
}