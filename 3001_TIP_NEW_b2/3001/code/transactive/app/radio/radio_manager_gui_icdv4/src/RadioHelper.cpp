/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioHelper.cpp $
  * @author:  Jiang Haipeng
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  *
  */

// RadioHelper.cpp: implementation of the CRadioHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "ace/OS.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioHelper.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"

#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#include <sstream>
//#include <iomanip> 

CString CRadioHelper::getTypeText(RadioResourceType type)
{
	CString sType;
	switch( type )
	{
	case RR_NIL:
		sType.LoadString(IDS_RR_NIL);
		break;
	case RR_RADIO: 
		sType.LoadString(IDS_RR_RADIO);
		break;
	case RR_TALKGROUP:
		sType.LoadString(IDS_RR_TALKGROUP);
		break;
	case RR_ANNGROUP: 
		sType.LoadString(IDS_RR_ANNGROUP);
		break;
	case RR_PATCHGROUP: 
		sType.LoadString(IDS_RR_PATCHGROUP);
		break;
	case RR_DYNGROUP: 
		sType.LoadString(IDS_RR_DYNGROUP);
		break;
	case RR_MSELECT: 
		sType.LoadString(IDS_RR_MSELECT);
		break;
	case RR_SITE: 
		sType.LoadString(IDS_RR_SITE);
		break;
	case RR_MSITE: 
		sType.LoadString(IDS_RR_MSITE);
		break;			
	case RR_TRAIN: 
		sType.LoadString(IDS_RR_TRAIN);
		break;
		
	default:
		sType.LoadString(IDS_RR_NIL);
	}
	
	return sType;
}

std::string CRadioHelper::timeToString(const time_t t)
{
 	tm *timeStruct = localtime(&t);
	if (timeStruct == NULL) return "";

	std::ostringstream strm;
	
	//we will justify right and pad zeros to the left
	strm << std::right;
	
	//YYYY
	strm << std::setw(4) << (timeStruct->tm_year + 1900) << "/" ;
	
	//MM/
	strm << std::setw(2) << std::setfill('0') << (timeStruct->tm_mon + 1) << "/" ;
	
	//DD/
	strm << std::setw(2) << std::setfill('0') << timeStruct->tm_mday << " " ;
	
	//HH:
	strm << std::setw(2) << std::setfill('0') << timeStruct->tm_hour << ":";
	
	//MM:
	strm << std::setw(2) << std::setfill('0') << timeStruct->tm_min << ":";
	
	//SS
	strm << std::setw(2) << std::setfill('0') << timeStruct->tm_sec;
	
	std::string str = strm.str();
	
	return str;
}

