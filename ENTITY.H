  #ifndef _ENTITY_H_
#define _ENTITY_H_

#include "base.h"
#include <vector>

#define PICK_RADIUS 0.01 // ѡȡ�뾶

#define ENTITY_NUM	12 // ͼԪ��������
enum EEntityType // ͼԪ������
{
	etUnknow	= 0,
	etPoint		= 1,
	etLine		= 2,
	etRectangle = 3,
	etCircle	= 4,
	etArc		= 5,
	etBspline	= 6,
	etEllipse	= 7,
	etText		= 8,
	etPolygon   = 9,
	etBlock     = 10,
	etConnect   = 11
};

enum EDrawMode // ��ͼģʽ
{
	dmNormal = 0, // ����״̬
	dmSelect = 1, // ѡ��״̬	
	dmPrompt = 2, // ��ʾ״̬
	dmDrag   = 3, // �϶�״̬
	dmInvalid = 4 // ����״̬
};

//////////////////////////
// define API function
//
void	SetDrawEnvir(CDC*	pDC, int drawMode, CPen* pPen);


class MEntity : public CObject
{
	DECLARE_SERIAL(MEntity) 
protected:
	// ��Ա����
	int			m_type;		// ͼԪ���ͣ�EEntityType��
	COLORREF	m_color ;   // ͼԪ��ɫ
	UINT		m_lineStyle ; // ͼԪ������
	int			m_lineWidth ; // ͼԪ���߿�	
public:
	int			m_nOperationNum;  //�ڼ��β������Ƶ�
	//BOOL		m_bErased;        //ʵ����ʱɾ����־
	MEntity *   next;
	MEntity *   GetShowEnt();      //�����ʾ��ʵ��
	MEntity *   GetCurrentEnt();
	MEntity*	Erase();
	void		Remove();
public:
	// ���캯������������
	MEntity() ;
	MEntity(const MEntity& entity);
	~MEntity();
	
	MEntity operator = (const MEntity& entity); // ���صȺ�
	virtual	MEntity*	Copy() { return NULL; } // ָ�뿽��

	virtual	void	Init(); // ��ʼ����Ա����ֵ
	virtual	int		GetType() { return m_type; }  // ����ͼԪ�����ͣ�EEntityType��
	COLORREF		GetColor() { return m_color; } // ����ͼԪ��ɫ
	void			SetColor(COLORREF color) { m_color = color; } // ����ͼԪ��ɫ
	void            SetWidth(int nWidth) { m_lineWidth = nWidth; }
	void			Append(MEntity * pEnt);
		
	virtual void Draw(CDC * pDC, int drawMode = dmNormal ) {}; // ����ͼԪ����

	// ����һ�㼰ʰȡ�뾶���ж�ͼԪ�Ƿ�ѡ��
	virtual BOOL Pick(const Position& pos, const double pick_radius) { return FALSE; } 
	// �õ��������С��Χ�У��ð�Χ�н�������ͼԪ��ѡȡ��ȫ����ʾ
	virtual void GetBox(BOX2D* pBox){}
	// ����һ�������һ��Ŀ���ƽ��ͼԪ
	virtual void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE) {}
	// ����һ�������һ���Ƕ�ֵ��תͼԪ
	virtual void Rotate(const Position& basePos, const double angle) {}
	// �������㾵��ͼԪ����������bCopy����ȷ���Ƿ�ɾ��ԭ��λ�õ�ͼԪ
	virtual void Mirror(const Position& pos1, const Position& pos2){}
	// �ı���
	virtual	void LoadPmtCursor() {}
	// Get a feature position of a entity near the giving position.
	// Input : giving a pos
	// output : TRUE or FALSE
	// Note : if true , the giving pos is reset to the feature position
	virtual BOOL GetSnapPos(Position& pos) { return FALSE; }

	virtual void Serialize(CArchive& ar) ; // ͼԪ�����л�
};

////////////////////////////
// CLASS MLines
//
class MLines : public MEntity
{
	DECLARE_SERIAL(MLines)
protected:
	Position	m_begin , m_end ; // �����յ�
	//Position *  m_pPositions;
	std::vector<Position> m_pPositions;
	//int			m_numpts;

public:
	MLines() ;
	MLines(const MLines& line);
	MLines(const Position& begin,const Position& end);
	MLines(std::vector<Position>& poses);
	MLines(int numpts, Position* poses);
	~MLines() ;

	MLines&	operator = (const MLines& line);
	MEntity*	Copy();

	int			GetType();
	void		Init(); 
	Position	GetBeginPos(); // �������ֵ
	Position	GetEndPos();   // �����յ�ֵ
	
	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void GetBox(BOX2D* pBox);
	
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);
	
	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar) ;

	//���ӽӿ�
	int GetNumOfPoints() const {
		//return m_numpts;
		return m_pPositions.size();
	}
	std::vector<Position> GetPoints() const {
		//return m_pPositions;
		return m_pPositions;
	}
};

////////////////////////////
// CLASS MConnect
//
class MConnect : public MEntity
{
	DECLARE_SERIAL(MConnect)
protected:
	Position	m_begin , m_end ; // �����յ�
	Position *  m_pPositions;
	int			m_numpts;
public:
	MConnect() ;
	MConnect(const MConnect& line);
	MConnect(const Position& begin,const Position& end);
	MConnect(int num, Position * pPos);
	~MConnect() ;

	MConnect&	operator = (const MConnect& line);
	MEntity*	Copy();
	void	    Manhadunize();

	int			GetType();
	void		Init(); 
	Position	GetBeginPos(); // �������ֵ
	Position	GetEndPos();   // �����յ�ֵ
	void MConnect::SetBeginPos(const Position & pos);
	void MConnect::SetEndPos(const Position & pos);
	
	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void GetBox(BOX2D* pBox);
	
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);
	
	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar) ;
};

////////////////////////////
// CLASS MLines
//
class MPolygon : public MEntity
{
	DECLARE_SERIAL(MPolygon)
protected:
	Position	m_begin , m_end ; // �����յ�
	Position *  m_pPositions;
	int			m_numpts;
public:
	MPolygon() ;
	MPolygon(const MPolygon& line);
	MPolygon(const Position& begin,const Position& end);
	MPolygon(int num, Position * pPos);
	~MPolygon() ;

	MPolygon&	operator = (const MPolygon& line);
	MEntity*	Copy();

	int			GetType();
	void		Init(); 
	Position	GetBeginPos(); // �������ֵ
	Position	GetEndPos();   // �����յ�ֵ
	
	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void GetBox(BOX2D* pBox);
	
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);
	
	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar) ;
};

////////////////////////////
// CLASS MRectangle
//
class MRectangle : public MEntity
{
	DECLARE_SERIAL(MRectangle)
protected:
	Position	m_LeftTop , m_RightBottom ; // �����յ�
public:
	MRectangle() ;
	MRectangle(const MRectangle& rect);
	MRectangle(const Position& LeftTop,const Position& RightBottom);
	~MRectangle() ;

	MRectangle&	operator = (const MRectangle& rect);
	MEntity*	Copy();

	int			GetType();
	void		Init(); 
	Position	GetLeftTopPos();		// �������Ͻǵ�ֵ
	Position	GetRightBottomPos();	// �������½ǵ�ֵ
	
	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void GetBox(BOX2D* pBox);
	
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);
	
	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor() ;

	void Serialize(CArchive& ar) ;
};

////////////////////////////
// CLASS MBlock
//
class MBlock : public MEntity
{
	DECLARE_SERIAL(MBlock)
protected:
	Position	m_Import , m_Outport ; // ��������
	Position	m_LeftTop , m_RightBottom ; // �����յ�

	//CString		m_EntText;             //ʵ��
	//CObList		m_EntList;             //ʵ������
	CBitmap  *  m_Bitmap;

public:
	MBlock() ;
	MBlock(const MBlock & blk) ;
	MBlock(const Position & in, const Position & out, CBitmap * bmp);
	MBlock(const Position & in, const Position & out, const char * text);
	MBlock(const Position & in, const Position & out, CObList & ents);
	~MBlock() ;

	MBlock&	operator = (const MBlock& rect);
	MEntity*	Copy();

	int			GetType();
	void		Init(); 
	Position	GetImportPos();		// ����Import��ֵ
	Position	GetOutportPos();	// ����Outport��ֵ
	Position	GetLeftTopPos();		// �������Ͻǵ�ֵ
	Position	GetRightBottomPos();	// �������½ǵ�ֵ
	CBitmap *	GetBitmap();
	void		SetBitmap(CBitmap * bmp);
	
	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void GetBox(BOX2D* pBox);
	
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);
	
	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor() ;

	void Serialize(CArchive& ar) ;
};

////////////////////////////
// CLASS MText
//
class MText : public MEntity
{
	DECLARE_SERIAL(MText)
protected:
	Position	m_LeftTop , m_RightBottom ; // �����յ�
	CString		m_Text;                     //�ı�����
public:
	MText() ;
	MText(const MText& rect);
	MText(const Position& LeftTop,const Position& RightBottom, const char * text);
	~MText() ;

	MText&	operator = (const MText& rect);
	MEntity*	Copy();

	int			GetType();
	void		Init(); 
	Position	GetLeftTopPos();		// �������Ͻǵ�ֵ
	Position	GetRightBottomPos();	// �������½ǵ�ֵ
	CString		GetText();
	void		SetText(const char * text);
	
	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void GetBox(BOX2D* pBox);
	
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);
	
	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor() ;

	void Serialize(CArchive& ar) ;
};

/////////////////
// Class CCircle
//
class CCircle : public MEntity
{
	DECLARE_SERIAL(CCircle)
protected:
	//member variables
	Position	m_center ;
	double		m_dRadius ;
public:
	// constructor and destructor
	CCircle() ;
	CCircle(const CCircle& circle);
	CCircle(const Position& center,const double& radius);
	CCircle(const Position& center,const Position& aroud);
	~CCircle() ;

	MEntity*	Copy();
	//-----------------------------------------------
	// member function
	// Attributes
	int			GetType();
	void		Init(); // initialize member variables
	Position	GetCenterPos();
	double		GetRadius();
	
	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void GetBox(BOX2D* pBox);
	
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);
	
	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor() ;

	void Serialize(CArchive& ar) ;
};

/////////////////
// Class MEllipse
//
class MEllipse : public MEntity
{
	DECLARE_SERIAL(MEllipse)
protected:
	//member variables
	Position	m_LeftTop ;
	Position	m_RightBottom ;
public:
	// constructor and destructor
	MEllipse() ;
	MEllipse(const MEllipse& elps);
	MEllipse(const Position& lt,const Position& rb);
	~MEllipse() ;

	MEntity*	Copy();
	//-----------------------------------------------
	// member function
	// Attributes
	int			GetType();
	void		Init(); // initialize member variables
	Position	GetLTPos();
	Position		GetRBPos();
	
	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void GetBox(BOX2D* pBox);
	
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);
	
	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor() ;

	void Serialize(CArchive& ar) ;
};

////////////////////////////
// CLASS CArc
//
class CArc : public MEntity
{
	DECLARE_SERIAL(CArc)
protected:
	//member variables
	Position	m_center ;
	Position	m_begin  ;
	Position    m_end    ;

public:
	// constructor and destructor
	CArc() ;
	CArc(const CArc& arc);
	CArc(const Position& center,const Position& startPos, const Position& endPos);
	~CArc() ;

	MEntity*	Copy();
	//-----------------------------------------------
	// member function
	// Attributes
	int			GetType();
	void		Init(); // initialize member variables
	Position	GetStartPos();
	Position	GetEndPos();
	Position	GetCenterPos() ;
	BOOL		GetSnapPos(Position& pos) ;

	void Draw(CDC * pDC, int drawMode = dmNormal ) ;

	//��ֱ�ߵı༭������ʰȡ��ƽ�ƣ���ת������ͻ����С��Χ��
	BOOL Pick(const Position& pos, const double pick_radius) ;
	void Move(const Position& basePos,const Position& desPos, BOOL bTemp = FALSE) ;
	void Rotate(const Position& basePos, const double angle) ;
	void Mirror(const Position& FstPos, const Position& SecPos);
	void GetBox(BOX2D* pBox);

	void LoadPmtCursor();
	void Serialize(CArchive& ar) ;
};
#endif