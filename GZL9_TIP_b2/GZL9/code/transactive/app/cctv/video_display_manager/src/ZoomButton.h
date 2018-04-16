#pragma once

#include "PTZButton.h"

namespace TA_IRS_App
{
class CZoomButton : public CPTZButton
{
public:
	CZoomButton(unsigned short zoomDirection);
	virtual bool handleButtonUp ();
	virtual bool handleButtonDown(); 

public:
	~CZoomButton(void);
private:
	unsigned short m_zoomDirection;
};
}
