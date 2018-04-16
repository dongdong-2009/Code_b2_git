// FileLoader.h: interface for the FileLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELOADER_H__7DB186FC_AB21_472B_8AF8_27BD664D3070__INCLUDED_)
#define AFX_FILELOADER_H__7DB186FC_AB21_472B_8AF8_27BD664D3070__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class FileLoader  
{
public:
	void operator()( std::string& s, std::istream& is );
};

#endif // !defined(AFX_FILELOADER_H__7DB186FC_AB21_472B_8AF8_27BD664D3070__INCLUDED_)
