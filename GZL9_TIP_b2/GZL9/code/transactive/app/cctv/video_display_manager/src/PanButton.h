#pragma once
#include "PTZButton.h"

namespace TA_IRS_App
{
class CPanButton :public CPTZButton 
{
public:
	CPanButton(unsigned short panDirection); 
	virtual bool handleButtonUp ();
	virtual bool handleButtonDown(); 
public:
	~CPanButton();
private:
	unsigned short m_panDirection;
};
}
