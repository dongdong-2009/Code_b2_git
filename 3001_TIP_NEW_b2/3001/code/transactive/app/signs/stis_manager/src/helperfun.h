#pragma once
#include <string>

std::wstring convertUtf8ToUtf16le(const std::string& src);
std::string convertUtf16leToUtf8(const std::wstring& src);