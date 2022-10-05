// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VCad.h"

#include "MainFrm.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
static UINT PropertyButtons[] = 
{
//	ID_SETTING_GRID,
//	ID_SETTING_ORTH,
	ID_PROPERTY,	//0
	ID_SEPARATOR,	//1
	ID_SEPARATOR,	//2:LINESTYLE
	ID_SEPARATOR,	//3
	ID_SEPARATOR,	//4:LINECOLOR
	ID_SEPARATOR,	//5
	ID_SEPARATOR,	//6:LINEWIDTH
};

//--------------------------------------------------------//
//--------------------------------------------------------//
// ȫ�ֺ�����Prompt
// ���ܣ���״̬���ϸ����������̵���ʾ��Ϣ
void Prompt(char* pmt)
{
	CStatusBar* pStatus=(CStatusBar*)
		AfxGetApp()->m_pMainWnd->GetDescendantWindow(ID_VIEW_STATUS_BAR);
	
	ASSERT(pStatus) ;
	if(pStatus)
		pStatus->SetPaneText(0, pmt, TRUE);
}
//--------------------------------------------------------//
//--------------------------------------------------------//

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD | WS_VISIBLE | CBRS_TOP| CBRS_GRIPPER |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // ����ʧ��
	}
	//�����޸Ĺ�����
	if(!m_wndModifyBar.CreateEx(this, TBSTYLE_FLAT,
		WS_CHILD | WS_VISIBLE | CBRS_TOP| CBRS_GRIPPER |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndModifyBar.LoadToolBar(IDR_MODIFY))
	{
		TRACE0("Failed to create modify toolbar\n");
		return -1;      // ����ʧ��
	}
	//�����޸Ĺ������ı���
	m_wndModifyBar.SetWindowText(_T("�޸Ĺ�����"));
	//�������ƹ�����
	if(!m_wndCreateBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD | WS_VISIBLE | CBRS_LEFT| CBRS_GRIPPER |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndCreateBar.LoadToolBar(IDR_CREATE))
	{
		TRACE0("Failed to create modify toolbar\n");
		return -1;      // ����ʧ��
	}
	//���û��ƹ������ı���
	m_wndCreateBar.SetWindowText(_T("���ƹ�����"));
	//������ͼ������
	if(!m_wndViewBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD | WS_VISIBLE | CBRS_LEFT| CBRS_GRIPPER |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndViewBar.LoadToolBar(IDR_VIEW))
	{
		TRACE0("Failed to create modify toolbar\n");
		return -1;      // ����ʧ��
	}
	m_wndViewBar.SetWindowText(_T("��ͼ������"));

	if(!CreatePropertyBar())
		return -1;      // ����ʧ��
	m_wndPropBar.SetWindowText(_T("���Թ�����")) ;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // ����ʧ��
	}

	//  ͣ��������
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndModifyBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCreateBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndViewBar.EnableDocking(CBRS_ALIGN_ANY) ;
	m_wndPropBar.EnableDocking(CBRS_ALIGN_ANY) ;
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndModifyBar) ;
	//ͨ���Զ���ĺ����������������ͣ����ʽ
	DockControlBarLeftOf(&m_wndCreateBar, &m_wndToolBar);
	DockControlBarLeftOf(&m_wndPropBar, &m_wndModifyBar);
	DockControlBarLeftOf(&m_wndViewBar, &m_wndPropBar) ;

	// ��ʾ��������
	CSplashWnd::ShowSplashScreen(this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::DockControlBarLeftOf(CControlBar* Bar,
									  CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;

	// ���� MFC �������������б�ͣ���Ĺ������ĳߴ�
	// �Ա� GetWindowRect ����������ȷ����
	RecalcLayout();
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? 
						AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? 
						AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? 
						AFX_IDW_DOCKBAR_RIGHT : n;

	// ������ʹ��ȱʡ�Ĳ���ʱ��
	//ÿ����������ͣ���ڷָ�����
	//ͨ���������������ʵ����������ģ��
	//���������Ϸŵ�ָ����λ����
	DockControlBar(Bar,n,&rect);
}

BOOL CMainFrame::CreatePropertyBar()
{
	const int nDropHeight = 100 ;
	if(!m_wndPropBar.Create(this, WS_CHILD|WS_VISIBLE|
		CBRS_LEFT|CBRS_SIZE_DYNAMIC|CBRS_TOOLTIPS|
		CBRS_FLYBY,IDR_PROPERTYBAR)||
		!m_wndPropBar.LoadBitmap(IDR_PROPERTYBAR)||
		!m_wndPropBar.SetButtons(PropertyButtons,
		sizeof(PropertyButtons)/sizeof(UINT)))
	{
		TRACE0("Failed to create propertybar\n") ;
		return FALSE ;
	}
	// ���� SEPARATOR �Ŀ���
	m_wndPropBar.SetButtonInfo(1, ID_SEPARATOR, 
		TBBS_SEPARATOR, 12) ;
	// ����������Ͽ�Ŀ���
	m_wndPropBar.SetButtonInfo(2, IDW_LINESTYLE, 
		TBBS_SEPARATOR,150) ;
	// ���� SEPARATOR �Ŀ���
	m_wndPropBar.SetButtonInfo(3, ID_SEPARATOR, 
		TBBS_SEPARATOR,12) ;
	// ������ɫ��Ͽ�Ŀ���
	m_wndPropBar.SetButtonInfo(4, IDW_LINECOLOR, 
		TBBS_SEPARATOR,100) ;
	// ���� SEPARATOR �Ŀ���
	m_wndPropBar.SetButtonInfo(5, ID_SEPARATOR, 
		TBBS_SEPARATOR,12) ;
	// �����߿�����Ͽ�Ŀ���
	m_wndPropBar.SetButtonInfo(6, IDW_LINEWIDTH,
		TBBS_SEPARATOR,150);
	
	// �������Ͽ�ı߽����
	CRect rc ;
	// ����������Ͽ�
	m_wndPropBar.GetItemRect(2,&rc) ;
	rc.bottom = rc.top + nDropHeight ;
	// ����������Ͽ�
	if(!m_wndPropBar.m_wndLineStyleCmb.Create(
		CBS_DROPDOWNLIST | WS_VISIBLE|WS_TABSTOP|
		WS_VSCROLL|CBS_OWNERDRAWVARIABLE,
		rc, &m_wndPropBar, IDW_LINESTYLE))
	{
		TRACE0("Failed to create linestyle combo--box!") ;
		return FALSE ;
	}

	// ��������Ͽ�����ѡ��
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_SOLID,"ʵ��") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_DASH,"����") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_DOT, "����") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_DASHDOT, "�㻮��") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_DASHDOTDOT, "˫�㻮��") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_NULL, "����") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_INSIDEFRAME, "�ڿ���") ;
	// ����ȱʡ��
	m_wndPropBar.m_wndLineStyleCmb.SetCurSel(0) ;

	// �����ߵ���ɫ��Ͽ�
	m_wndPropBar.GetItemRect(4, &rc) ;
	rc.bottom = rc.top + nDropHeight ;
	// �����ߵ���ɫ��Ͽ�
	if(!m_wndPropBar.m_wndLineColorCmb.Create(
		CBS_DROPDOWNLIST |WS_VISIBLE| WS_TABSTOP|
		WS_VSCROLL|CBS_OWNERDRAWVARIABLE,
		rc, &m_wndPropBar, IDW_LINECOLOR))
	{
		TRACE0("Failed to create COLOR combo--box!\n") ;
		return FALSE ;
	}
	// ���ߵ���ɫ��Ͽ�����ѡ��
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(255,0,0),"��ɫ") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(0,255,0),"��ɫ") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(0,0,255),"��ɫ") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(255,255,0),"��ɫ") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(0,255,255),"��ɫ") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(255,0,255),"��ɫ") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(0,0,0),"��ɫ") ;
	// ���õ�ǰȱʡ��ѡ��
	m_wndPropBar.m_wndLineColorCmb.SetCurSel(6) ;
	// �����߿���Ͽ�
	m_wndPropBar.GetItemRect(6,&rc) ;
	rc.bottom = rc.top + nDropHeight ;
	// �����߿���Ͽ�
	if(!m_wndPropBar.m_wndLineWidthCmb.Create(
		CBS_DROPDOWNLIST| WS_VISIBLE|WS_TABSTOP|
		WS_VSCROLL|CBS_OWNERDRAWVARIABLE,
		rc, &m_wndPropBar, IDW_LINEWIDTH))
	{
		TRACE0("Failed to create WIDTH combo--box!\n") ;
		return FALSE ;
	}
	//���߿���Ͽ�����ѡ��
	m_wndPropBar.m_wndLineWidthCmb.AddItem(1, "����1") ;
	m_wndPropBar.m_wndLineWidthCmb.AddItem(2, "����2") ;
	m_wndPropBar.m_wndLineWidthCmb.AddItem(3, "����3") ;
	m_wndPropBar.m_wndLineWidthCmb.AddItem(4, "����4") ;
	//���õ�ǰȱʡ��ѡ��
	m_wndPropBar.m_wndLineWidthCmb.SetCurSel(0) ;

	return TRUE ;

}