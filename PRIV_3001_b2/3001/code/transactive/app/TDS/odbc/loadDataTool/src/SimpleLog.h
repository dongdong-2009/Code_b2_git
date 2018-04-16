#pragma once

#ifndef _SIMPLELOG_H_6C1563EB_1DFB_4B2F_A1CD_A1C69B8B7023
#define _SIMPLELOG_H_6C1563EB_1DFB_4B2F_A1CD_A1C69B8B7023

#include <iostream>

inline void simpleLog__(const char* fileName, long lineNum, const char* msg)
{
    std::cout << "[" << fileName << "]" << "[" << lineNum << "]" << " " << msg << std::endl;
}

#endif // !_SIMPLELOG_H_6C1563EB_1DFB_4B2F_A1CD_A1C69B8B7023