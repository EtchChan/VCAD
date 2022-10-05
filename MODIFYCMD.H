/*	
 *	Header File of Class CModifyCmd
 */
#ifndef _ModifyCmd_h_
#define _ModifyCmd_h_

#include "base.h"

#ifdef __cplusplus

class CVPlotDoc ;
class CVPlotView ;
///////////////////////////////////////////////////////////////
/*	
 *	CMove
 */
class CMove : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CMove() ;
	~CMove() ;
	
	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos) ;
	int		OnMouseMove(UINT nFlags, const Position& pos) ;
	int		OnRButtonDown(UINT nFlags, const Position& pos) ;

	int		Cancel() ;
} ;
///////////////////////////////////////////////////////////////
/*	
 *	CRotate
 */
class CRotate : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CRotate() ;
	~CRotate() ;
	
	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos) ;
	int		OnMouseMove(UINT nFlags, const Position& pos) ;
	int		OnRButtonDown(UINT nFlags, const Position& pos) ;

	int		Cancel() ;
} ;
///////////////////////////////////////////////////////////////
/*	
 *	CMirror
 */

class CMirror : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CMirror();
	~CMirror();

	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos);
	int		OnMouseMove(UINT nFlags, const Position& pos);
	int		OnRButtonDown(UINT nFlags, const Position& pos);

	int		Cancel();
};

///////////////////////////////////////////////////////////////
/**
 * \modu CFillColor
 * \ingroup ModifyCmd->CFill
 * 
 * \brief Fill the region with invarient single color
 *
 * Architect: Yixuan.Chen
 * Owner: TODO DEV
 * Last Reviewer: Yixuan.Chen || 2022-Oct-05 14:24
 */
class CFillColor : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CFillColor() ;
	~CFillColor() ;
	
	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos) ;
	int		OnMouseMove(UINT nFlags, const Position& pos) ;
	int		OnRButtonDown(UINT nFlags, const Position& pos) ;

	int		Cancel() ;
} ;

/*
* \brief prototype of CFillPattern
*/
class CFillPattern : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CFillPattern();
	~CFillPattern();

	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos);
	int		OnMouseMove(UINT nFlags, const Position& pos);
	int		OnRButtonDown(UINT nFlags, const Position& pos);

	int		Cancel();
};

///////////////////////////////////////////////////////////////
#endif // __cplusplus

#endif // _ModifyCmd_h_
