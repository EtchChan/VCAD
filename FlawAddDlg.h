#pragma once


// CFlawAddDlg �Ի���

class CFlawAddDlg : public CDialog
{
	DECLARE_DYNAMIC(CFlawAddDlg)

public:
	CFlawAddDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFlawAddDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_FLAW_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_pt1_x;
	double m_pt1_y;
	double m_pt2_x;
	double m_pt2_y;
	double m_pt3_x;
	double m_pt3_y;
	double m_pt4_x;
	double m_pt4_y;
	virtual BOOL OnInitDialog();
	double m_sigma;
	double m_miu;
};
