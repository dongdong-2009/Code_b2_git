#include "helperfun.h"
#include <Windows.h>

std::wstring convertUtf8ToUtf16le(const std::string& src)
{
    const char* srcStr = src.c_str();
    int dstLen = MultiByteToWideChar(CP_UTF8, 0, srcStr, -1, NULL, 0);
    wchar_t *dst = new wchar_t[dstLen];
    MultiByteToWideChar(CP_UTF8, 0, srcStr, -1, dst, dstLen);
    std::wstring result(dst);
    delete []dst;
    return result;
}

std::string convertUtf16leToUtf8(const std::wstring& src)
{
    const wchar_t* srcStr = src.c_str();
    int dstLen = WideCharToMultiByte(CP_UTF8, 0, srcStr, -1, NULL, 0, NULL, NULL);
    char* dst = new char[dstLen];
	ZeroMemory(dst, dstLen);
    WideCharToMultiByte(CP_UTF8, 0, srcStr, -1, dst, dstLen, NULL, NULL);
    std::string result(dst);
    delete []dst;
    return result;
}