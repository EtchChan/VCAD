//	���ļ��еĺ�����Ҫ���ڱ������ʾԤ��λͼ	//		00-10-20

#include "stdafx.h"
#include "WinUser.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"

//	Ԥ��ͼ�εĴ�С
#define BMP_PREVIEW_WIDTH	200 
#define BMP_PREVIEW_HEIGHT	128

unsigned char *g_pBmpBuf = NULL;	//λͼ������

//	���������������ⲿʹ��
//������Ԥ�Ե�λͼд���ļ�
void BitmapSerialize(CArchive& ar);
//���ͻ������ڵ�ͼ�δ�Ϊλͼ
void GenBitmap();
//	�õ�VCAD�ļ���
BOOL GetCADName(BOOL bIsOpen,CString& sFileName);

//ѹ�����ѹ��λͼ����ϸԭ�����ͼ���ʽ���ָ�ϡ����廪��ѧ�����磩GIFͼ��һ��
//ѹ������Ԥ�Ե�λͼ 
DWORD Compressed(short *pComBuf,unsigned char *lpBuf,UINT nCount);
void pack_init(int clear,short *reqcnt,short first_ch,short *oldcode,
			   short *nextcode,short *nextlim,short *CTlink,short *CTnext,
			   unsigned char *CTfirst);
short lookup_ct(short code,unsigned char thischar,short *nextcode,
				short *CTlink,short *CTnext,unsigned char *CTfirst);
//��ѹ������Ԥ�Ե�λͼ 
void DeCompressed(unsigned char *entBuffer,short *filebuf);
void unpack_init(short clear,short *nextcode,short *oldcode,
				short *reqcnt,short *nextlim,short first_ch,
				 unsigned char *CTfirst,unsigned char *CTlast,
				 short *CTlink);
void putx(short code,unsigned char *OStack,unsigned char *CTlast,
		  short *CTlink,unsigned char *entBuffer,DWORD *size);
void insert_ct(short code,short oldcode,short *nextcode,short *nextlim,
			   short *reqcnt,short *CTlink,unsigned char *CTfirst,
			   unsigned char *CTlast);

//	����Ԥ��λͼ
void GenBitmap()
{
	CRect	clientRect;
	CBitmap	bitmap;
	CClientDC	dc(g_pView);
	CDC		memDC;
	DWORD	i=0,dwCount=0;
	int		x=0,y=0;
	COLORREF	color;
	
	CRect prevrect(0,0,BMP_PREVIEW_WIDTH,BMP_PREVIEW_HEIGHT);
	g_pView->GetClientRect(&clientRect);//�õ��ͻ����ڵľ���
	bitmap.CreateCompatibleBitmap(&dc,
			BMP_PREVIEW_WIDTH,BMP_PREVIEW_HEIGHT);//��ʼ���հ�λͼ
	memDC.CreateCompatibleDC(&dc);//�����ڴ��豸�龰����
	memDC.SelectObject(&bitmap);//��λͼѡ���ڴ��豸�龰����

	memDC.SetMapMode(MM_TEXT); 

	memDC.Rectangle(&prevrect);	//	���Ʊ߽粢�����������

	//	���ݵ�ǰ��ͼ����С��Ԥ��ͼ�δ�С���������ű���
	double dScale=1.0*clientRect.Width()/BMP_PREVIEW_WIDTH;
	//	��С��������Ԫ��
	  // ���ȵõ���ǰ״̬ϵͳ������
	double dOldScale = g_pView->GetScale(); // ���ű���
	dScale *= dOldScale;
	double dOldOrgX = g_pView->m_dOrgX; // ��Ļ���ĵ����������
	double dOldOrgY = g_pView->m_dOrgY;
      // Ȼ�����ø�״̬�������Ա�ʹ�����豸�������Ϸ�����ͼ��
	g_pView->m_dOrgX = (clientRect.Width() - BMP_PREVIEW_WIDTH)/2 * dScale;
	g_pView->m_dOrgY = -(clientRect.Height() - BMP_PREVIEW_HEIGHT)/2 * dScale ;
	g_pView->SetScale(dScale);
	g_pView->OnDraw(&memDC);
	  // ���ָ���ϵͳ״̬����
	g_pView->SetScale(dOldScale);
	g_pView->m_dOrgX = dOldOrgX;
	g_pView->m_dOrgY = dOldOrgY;
		
	dwCount=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;//�õ�λͼ���ݵ��ֽ���
	//	����λͼ���ݻ�����
	if(g_pBmpBuf!=NULL)
		delete g_pBmpBuf;
	g_pBmpBuf=new BYTE[dwCount];			
	//	�õ�λͼ����
	i=0;
	for(y=BMP_PREVIEW_HEIGHT-1;y>=0;y--)		
	{
		for(x=0;x<BMP_PREVIEW_WIDTH;x++)
		{
			i=(BMP_PREVIEW_HEIGHT-1-y)*BMP_PREVIEW_WIDTH+x;
			color=memDC.GetPixel(x,y);
			g_pBmpBuf[i*3]=(BYTE)((color&0x00ff0000)>>16);//B
			g_pBmpBuf[i*3+1]=(BYTE)((color&0x0000ff00)>>8);//G
			g_pBmpBuf[i*3+2]=(BYTE)(color&0x000000ff);//R
		}
	}

	memDC.SetMapMode(MM_LOENGLISH); 
}

//	������Ԥ�Ե�λͼ����д���ļ� 
void BitmapSerialize(CArchive& ar)
{
	DWORD	i=0,dwCount=0,len=0;
	dwCount=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;//�õ�λͼ���ݵ��ֽ���

	if(ar.IsStoring())
	{
		len=dwCount;
		ar<<len;			//ѹ����ĳ���д���ļ�
		for(i=0;i<len;i++)
			ar<<g_pBmpBuf[i];	//��λͼ����д���ļ�
		delete g_pBmpBuf;		//���λͼ������
		g_pBmpBuf=NULL;
	}
	else
	{
		ar>>len;	//����ѹ��λͼ�ĳ���
		unsigned char buf;	//λͼ���ݻ�����
		for(i=0;i<len;i++)		//���ļ��ж���λͼ����
			ar>>buf;
	}
}

//	��ѡ����ļ��еõ�Ԥ��λͼ����
BOOL GetFileSelect(unsigned char *entBuffer,char *filenamestring)
{
	CFile	 file;
	//���ļ�
	if(!file.Open(filenamestring,CFile::modeRead)) 
		return FALSE;
	CArchive ar(&file,CArchive::load);

	DWORD dwCount=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;//�õ�λͼ���ݵ��ֽ���
	DWORD i,len;
	ar>>len;//�ļ���ѹ�����λͼ���ݳ���
	for(i=0;i<len;i++)
		ar>>entBuffer[i];		//���ļ��ж���λͼ����

	ar.Close();
	file.Close();
    return TRUE;
}

//	����Ԥ��λͼ������ָ��λ����ʾλͼ
void DisplayBitmap(HDC hDC,int x,int y,unsigned char *entBuffer)
{
	BITMAPINFO bmInfo;
	bmInfo.bmiHeader.biSize=sizeof(bmInfo.bmiHeader);
	bmInfo.bmiHeader.biWidth=BMP_PREVIEW_WIDTH;
	bmInfo.bmiHeader.biHeight=BMP_PREVIEW_HEIGHT;
	bmInfo.bmiHeader.biPlanes=1;
	bmInfo.bmiHeader.biBitCount=24;
	bmInfo.bmiHeader.biCompression=BI_RGB;
	bmInfo.bmiHeader.biSizeImage=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;
	bmInfo.bmiHeader.biClrUsed=1;
	bmInfo.bmiHeader.biClrImportant=0;
	SetDIBitsToDevice(hDC,x,y,BMP_PREVIEW_WIDTH,
			BMP_PREVIEW_HEIGHT,0,0,0,BMP_PREVIEW_HEIGHT,entBuffer,
			&bmInfo,DIB_RGB_COLORS);
}

//	��/�����ļ��Ĺ��Ӻ���
UINT APIENTRY OFNHookProc(HWND hdlg,UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	static char szFile[256]; //�ļ���
	static BOOL	bFolderChag;		//Ŀ¼�Ƿ�ı��־
	static BOOL bIsPreview = TRUE;	//�Ƿ�ѡ��Ԥ��ͼ��
	static CRect	rectPrev=CRect(0,0,0,0);//Ԥ�Ծ���
	static BOOL bIsCADFlag = FALSE;	//ѡ�е��Ƿ�ΪCAD�ļ���־
	UINT	code;

	switch (uiMsg)
	{
	case WM_INITDIALOG:	//	��ʼ���Ի���
	{
		SetWindowLong(hdlg, -21, lParam);
		HWND hWnd=GetDlgItem(hdlg,IDC_STATIC_FILEPREVIEW);
		GetWindowRect(hWnd,&rectPrev);	//�õ�Ԥ�Դ��ھ���
		POINT point[2];
		point[0]=rectPrev.TopLeft();
		point[1]=rectPrev.BottomRight();
		ScreenToClient(hdlg,&point[0]);
		ScreenToClient(hdlg,&point[1]);
		rectPrev.left=point[0].x;
		rectPrev.right=point[1].x;
		rectPrev.top=point[0].y;
		rectPrev.bottom=point[1].y;
		DWORD dwCount=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;//�õ�λͼ���ݵ��ֽ���
		g_pBmpBuf=new unsigned char[dwCount];		//����λͼ���ݻ�����
		if(g_pBmpBuf==NULL)
			return FALSE;
		bFolderChag=FALSE;
		bIsPreview=TRUE;
		HWND hCheck=GetDlgItem(hdlg,IDC_CHECK_PREVIEW);
		::SendMessage(hCheck,BM_SETCHECK,BST_CHECKED,0);

	}
		return FALSE;
	case WM_DESTROY:	//	�Ի�������ٴ���
		if(g_pBmpBuf!=NULL)
		{
			delete []g_pBmpBuf;	//ɾ��λͼ���ݻ�����
			g_pBmpBuf=NULL;
		}
		bIsCADFlag=FALSE;
		return FALSE;
	case WM_PAINT:	//	���ƶԻ����е�Ԥ��λͼ
		{
			try
			{
				CWnd *pWnd;
				pWnd=CWnd::FromHandle(hdlg);
				HDC hDC=GetDC(hdlg);
				CDC *pDC;
				pDC=CDC::FromHandle(hDC);
				CPoint midpoint=rectPrev.CenterPoint();
				int nXOffset=16,nYOffset=4;
				if(bIsCADFlag==TRUE&&bIsPreview==TRUE)
				{	//ѡ�е�Ϊ�Ϸ��ļ��Ҵ���Ԥ�Կ���
					//���Ԥ��λͼ
					DisplayBitmap(hDC,midpoint.x+nXOffset-BMP_PREVIEW_WIDTH/2,
							midpoint.y+nYOffset-BMP_PREVIEW_HEIGHT/2,g_pBmpBuf);
				}
				else	
				{	//��Ԥ�Բ���Ϳ�ɵ�ɫ
					CBrush brush(RGB(192,192,192));
					CRect rect(midpoint.x+nXOffset-BMP_PREVIEW_WIDTH/2,
							midpoint.y+nYOffset-BMP_PREVIEW_HEIGHT/2,
							midpoint.x+nXOffset+BMP_PREVIEW_WIDTH/2,
							midpoint.y+nYOffset+BMP_PREVIEW_HEIGHT/2);
					pDC->FillRect(&rect,&brush);
				}
				ReleaseDC(hdlg,hDC);
			}
			catch (...)
			{
				return FALSE;
			}
			return FALSE;
		}
	case WM_COMMAND:	//	�����Ƿ�Ԥ��ѡ��
		if(LOWORD(wParam)==IDC_CHECK_PREVIEW&&
				HIWORD(wParam)==BN_CLICKED)
		{
			bIsPreview=!bIsPreview;
			::SendMessage(hdlg,WM_PAINT,0,0);
		}
		return FALSE;
	case WM_NOTIFY:	//	����֪ͨ��Ϣ
		code=((LPOFNOTIFY)lParam)->hdr.code;
		switch(code)
		{
		case CDN_SELCHANGE:	//	��ѡ��ͬ���ļ�ʱ
			if (CommDlg_OpenSave_GetFilePath(GetParent(hdlg),
				szFile, sizeof(szFile)) <= sizeof(szFile))//�õ��ļ���
			{
				if(bFolderChag==TRUE)//Ŀ¼�ı�,�Ѿ���CDN_FOLDERCHANGE�н����˴���,��˷���
				{
					::SendMessage(hdlg,WM_PAINT,0,0);
					bFolderChag=FALSE;
					return FALSE;
				}
				//�ж��Ƿ�Ϊ�Ϸ��ļ�
				int length=strlen(szFile);
				if(length<7)
					return FALSE;
				char ext[4];
				HWND hWnd=GetDlgItem(hdlg,IDC_STATIC_FILEPREVIEW);
				ext[3]='\0';
				ext[2]=szFile[length-1];
				ext[1]=szFile[length-2];
				ext[0]=szFile[length-3];
				_strlwr(ext);
				if(strcmp(ext,"cad")==0)
				{
					bIsCADFlag=TRUE;
					if(!GetFileSelect(g_pBmpBuf,szFile)) //�õ�λͼ����
						bIsCADFlag=FALSE;
				}
				else 
					bIsCADFlag=FALSE;
				::SendMessage(hdlg,WM_PAINT,0,0);
			}
			return FALSE;
		case CDN_FOLDERCHANGE:	//	��ѡ�е��ļ��иı�ʱ
			//Ŀ¼�ı�,�����Ŀ¼��û��CAD�ļ�,��ô��û��CDN_SELCHANGE��Ϣ����
			//����ִ�����ⲿ�ֺ�ͻ�ִ��CDN_DELCHANGE����Ϣ����
			if (CommDlg_OpenSave_GetFilePath(GetParent(hdlg),
				szFile, sizeof(szFile)) <= sizeof(szFile))
			{
				bFolderChag=TRUE;
				//�ж��Ƿ�Ϊ�Ϸ��ļ�
				int length=strlen(szFile);
				if(length<7)
					return FALSE;
				char ext[4];
				HWND hWnd=GetDlgItem(hdlg,IDC_STATIC_FILEPREVIEW);
				ext[3]='\0';
				ext[2]=szFile[length-1];
				ext[1]=szFile[length-2];
				ext[0]=szFile[length-3];
				_strlwr(ext);
				if(strcmp(ext,"CAD")==0)
				{
					bIsCADFlag=TRUE;
					if(!GetFileSelect(g_pBmpBuf,szFile))
						bIsCADFlag=FALSE;
				}
				else bIsCADFlag=FALSE;
				::SendMessage(hdlg,WM_PAINT,0,0);
			}
			return FALSE;
		case CDN_FILEOK:	//	������ȷ������ť
			return TRUE;
		}
	}
	return FALSE;
}

//	�õ�CAD�ļ���
BOOL GetCADName(BOOL bIsOpen,CString& sFileName)
{
	CString sFilter="ͼ���ļ�";
	sFilter += "(*.CAD)|*.CAD|";	
	sFilter.Replace('|','\0');
	char	strFileName[256]="\0";
	CString sTitle;
	BOOL bRet=TRUE;

	if(bIsOpen)
		sTitle="���ļ�";
	else
		sTitle="����ļ�";

	OPENFILENAME ofn;

	ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = AfxGetMainWnd()->m_hWnd;
    ofn.hInstance         = AfxGetInstanceHandle();
    ofn.lpstrFilter       = sFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = strFileName;
    ofn.nMaxFile          = sizeof(strFileName);
    ofn.lpstrFileTitle    = NULL;
    ofn.nMaxFileTitle     = 0;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = sTitle;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "CAD";
	ofn.lpfnHook 		  = (LPOFNHOOKPROC)OFNHookProc;
	ofn.lpTemplateName    = MAKEINTRESOURCE(IDD_DIALOG_FILEPREVIEW);
    ofn.Flags             = OFN_ENABLETEMPLATE|//OFN_NOVALIDATE|
							 OFN_EXPLORER |OFN_ENABLEHOOK|OFN_HIDEREADONLY|
							 OFN_OVERWRITEPROMPT;

    if (bIsOpen)
		bRet=GetOpenFileName(&ofn);
	else
		bRet=GetSaveFileName(&ofn);
	
	if(bRet)
		sFileName=ofn.lpstrFile;

	return bRet;

}