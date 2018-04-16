// IDataFrame.h: interface for the IDataFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDATAFRAME_H__2509719C_5284_4C67_8C1D_E642565EA5CE__INCLUDED_)
#define AFX_IDATAFRAME_H__2509719C_5284_4C67_8C1D_E642565EA5CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IDataFrame  
{
public:
		virtual void setDataFrame (CString * strData) = 0;
};

#endif // !defined(AFX_IDATAFRAME_H__2509719C_5284_4C67_8C1D_E642565EA5CE__INCLUDED_)
