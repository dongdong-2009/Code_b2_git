#include "StringConverter.h"
#include "core/utilities/src/DebugUtil.h"
#include <sstream>
#include <vector>
#if defined(WIN32)
#include <Windows.h>
#endif

#if !defined(WIN32)
#include <iconv.h>
#endif

namespace TA_IRS_App
{
	using TA_Base_Core::DebugUtil;
    std::wstring StringConverter::convertUtf16LeToUtf32( const RawString& rawString )
    {
#if !defined(WIN32)
        char* oriSrc = rawString.getData();
		if(0 == oriSrc)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "The string to be converted is empty");
			return std::wstring(L"");
		}
		char* src = oriSrc;
        size_t oriSrcLen = (size_t)(rawString.getLength());
        size_t oriDstLen = oriSrcLen * 2;
        char* dstBuf = new char[oriDstLen];
		memset(dstBuf, 0, oriDstLen);
		char* dst = dstBuf;
		size_t dstLen = oriDstLen;
        iconv_t conv = (iconv_t)(-1);
        conv = iconv_open( "UTF-32LE", "UTF-16LE" );
		if ((iconv_t)(-1) == conv)
		{
			delete [] oriSrc;
			delete [] dstBuf;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "iconv_open return -1");
			return L"";
		}
        iconv( conv, ( const char** )( &src), &oriSrcLen, &dst, &dstLen );
        iconv_close( conv );
        std::wstring temp( (wchar_t*)(dstBuf), (oriDstLen - dstLen)/4 );
        delete [] dstBuf;
		delete [] oriSrc;
        return temp;
#else
		return std::wstring(L"");
#endif // Windows should not use this func
    }

	RawString StringConverter::convertUtf32ToUtf16Le(const std::wstring& ws)
	{
#if !defined(WIN32)
        if(0 == ws.size())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "The string to be converted is empty");
			return RawString();
		}
		char* oriSrc = RawString::dup((const char*)(ws.c_str()), ws.size()*4);
        size_t oriSrcLen = (size_t)(ws.size()*4);
		char* src = (char*)oriSrc;
        size_t oriDstLen = oriSrcLen;
        char* dstBuf = new char[oriDstLen];
		memset(dstBuf, 0, oriDstLen);
		char* dst = dstBuf;
		size_t dstLen = oriDstLen;
        iconv_t conv = (iconv_t)(-1);
        conv = iconv_open( "UTF-16LE", "UTF-32LE" );
		if ((iconv_t)(-1) == conv)
		{
			delete [] oriSrc;
			delete [] dstBuf;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "iconv_open return -1");
			return RawString();
		}
        iconv( conv, ( const char** )( &src), &oriSrcLen, &dst, &dstLen );
        iconv_close( conv );
		RawString rs(dstBuf, oriDstLen - dstLen);
        delete [] dstBuf;
		delete [] oriSrc;
        return rs;
#else // Windows should not use this func
		return RawString();
#endif
	}

	std::string StringConverter::convertUtf32ToUtf8( std::wstring& ws )
    {
        if(true == ws.empty())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "The string to be converted is empty");
			return std::string("");
		}
#if !defined(WIN32)
        const wchar_t* oriSrc = ws.c_str();
        size_t oriSrcLen = (size_t)(ws.size()*4);
		char* src = (char*)oriSrc;

        size_t oriDstLen = oriSrcLen;
        char* dstBuf = new char[oriDstLen];
		memset(dstBuf, 0, oriDstLen);
		char* dst = dstBuf;
		size_t dstLen = oriDstLen;
        iconv_t conv = (iconv_t)(-1);
        conv = iconv_open( "UTF-8", "UTF-32LE" );
		if ((iconv_t)(-1) == conv)
		{
			delete [] dstBuf;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "iconv_open return -1");
			return "";
		}
        iconv( conv, ( const char** )( &src), &oriSrcLen, &dst, &dstLen );
        iconv_close( conv );
        std::string temp( dstBuf, oriDstLen - dstLen );
        delete [] dstBuf;
        return temp;
#else
		return std::string("");
#endif  // Windows should not use this func
    }

    std::wstring StringConverter::vectorCharToWstring(const std::vector<unsigned char>& charVector)
	{
		if(true == charVector.empty())
		{
			return std::wstring(L"");
		}
#if !defined(WIN32)
		unsigned long len = charVector.size();
	    char* temp = new char[len];
		memset(temp, 0, len);
		RawString s(temp, len);
		delete [] temp;
		for(unsigned long i = 0; i < len; ++i)
		{
			s[i] = charVector[i];
		}
		return convertUtf16LeToUtf32(s); 
#else
		wchar_t tmpWchar;
		std::wstring tmpWstring;
		if(charVector.size() % 2 != 0)
		{
			return tmpWstring;
		}
		unsigned long size = charVector.size()/2;
		tmpWstring.resize(size);
		for(std::vector<unsigned char>::size_type i = 0; i < size; ++i)
		{
			tmpWchar = 0;
			tmpWchar += charVector[i*2];
			tmpWchar += (((wchar_t)charVector[i*2+1]) << 8);
			tmpWstring[i] = tmpWchar;
		}
		return tmpWstring;
#endif
	}

	std::string StringConverter::vectorCharToString(const std::vector<unsigned char>& charVector)
	{
		std::string tmp(charVector.begin(), charVector.end());
		return tmp;
	}

	std::vector<unsigned char> StringConverter::stringToVectorChar(const std::wstring& ws)
	{
		if(true == ws.empty())
		{
			return std::vector<unsigned char>();
		}
		std::vector<unsigned char> tmpVec;
#if !defined(WIN32)
		// first convert utf32 string to utf16
		RawString rs = convertUtf32ToUtf16Le(ws);
		for(unsigned long i = 0; i < rs.getLength(); ++i)
		{
			tmpVec.push_back(rs[i]);
		}
		return tmpVec;
#else
		unsigned long size = ws.size();
		for(unsigned long i = 0; i != size; ++i)
		{
			wchar_t tmp = ws[i];
			tmpVec.push_back((tmp << 8) >> 8);
			tmpVec.push_back(tmp >> 8);
		}
		return tmpVec;
#endif
	}

	std::vector<unsigned char> StringConverter::stringToVectorChar(const std::string& s)
	{
		std::vector<unsigned char> tmpVec(s.begin(), s.end());
		return tmpVec;
	}

	std::string StringConverter::convertUtf16leToUtf8( const std::wstring& src )
    {
		if(true == src.empty())
		{
			return std::string("");
		}
#if !defined(WIN32) // Solaris should not use this func
		return std::string("");
#else
        const wchar_t* srcStr = src.c_str();
        size_t dstlen = WideCharToMultiByte( CP_UTF8, 0, srcStr, -1, NULL, 0, NULL, NULL );
        char *dst = new char[dstlen];
		ZeroMemory(dst, dstlen);
        WideCharToMultiByte( CP_UTF8, 0, srcStr, -1, dst, dstlen, NULL, NULL );
        std::string result = dst;
        delete [] dst;
        return result;
#endif
    }

	std::string StringConverter::wstringToString( const std::wstring& ws )
	{
		if(true == ws.empty())
		{
			return std::string("");
		}
        std::ostringstream os;
        for ( std::wstring::const_iterator iter = ws.begin(); iter != ws.end(); ++iter )
        {
            os << ( char )( *iter );
        }
        return os.str();
	}

    TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType StringConverter::vectorCharToCorbaCharSequence(const std::vector<unsigned char>& charVector)
	{
		if(true == charVector.empty())
		{
			return TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType();
		}
		TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType tmp;
		tmp.length(charVector.size());
		for(unsigned long i = 0; i < tmp.length(); ++i)
		{
			tmp[i] = charVector[i];
		}
		return tmp;
	}

	std::vector<unsigned char> StringConverter::corbaCharSequenceToVectorChar(const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& charSequence)
	{
		if(0 == charSequence.length())
		{
			return std::vector<unsigned char>();
		}
		std::vector<unsigned char> tmp;
		for(unsigned long i = 0; i < charSequence.length(); ++i)
		{
			tmp.push_back(charSequence[i]);
		}
		return tmp;
	}
}