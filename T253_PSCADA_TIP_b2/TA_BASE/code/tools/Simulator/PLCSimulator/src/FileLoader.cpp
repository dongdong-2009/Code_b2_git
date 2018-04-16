// FileLoader.cpp: implementation of the FileLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void FileLoader::operator()(std::string& s, std::istream& is)
{
	s.erase();
	if(is.bad()) return;
	s.reserve(is.rdbuf()->in_avail());
	char c;
	while(is.get(c))
	{
		if(s.capacity() == s.size())
			s.reserve(s.capacity() * 3);
		s.append(1, c);
	}
}