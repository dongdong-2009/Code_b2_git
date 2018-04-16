#include "ZoomButton.h"

namespace TA_IRS_App
{
	CZoomButton::CZoomButton(unsigned short zoomDirection):m_zoomDirection(zoomDirection)
	{

	}

	CZoomButton::~CZoomButton()
	{
	}

	bool CZoomButton::handleButtonUp () 
	{
		return CPTZButton::m_commandListener->onZoomCommand( m_zoomDirection, true);
	}

	bool CZoomButton::handleButtonDown()
	{
	  return CPTZButton::m_commandListener->onZoomCommand ( m_zoomDirection, false);
	}
}
