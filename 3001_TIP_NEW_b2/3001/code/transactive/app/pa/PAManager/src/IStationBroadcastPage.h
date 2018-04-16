/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/IStationBroadcastPage.h $
  * @author Sean Liew
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by: $Author: qi.huang $
  * 
  */

#if !defined(AFX_IStationBroadcastPage_H__B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_)
#define AFX_IStationBroadcastPage_H__B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/pa/PAManager/src/IGenericBroadcastPage.h"

namespace TA_IRS_Bus
{
    struct StationDvaMessageRecord;
}

class IStationBroadcastPage : public IGenericBroadcastPage
{
public:
	virtual void updateNormalMessageList(std::list<TA_IRS_Bus::StationDvaMessageRecord>& records) = 0;
	virtual void updateEmergencyMessageList(std::list<TA_IRS_Bus::StationDvaMessageRecord>& records) = 0;
	virtual void updateAdhocMessageList(std::list<TA_IRS_Bus::StationDvaMessageRecord>& records) = 0;
    virtual void updateSelectedMessagesList(std::list<TA_IRS_Bus::StationDvaMessageRecord>& records) = 0;

    virtual void displayDvaStatusDialog(const std::string& broadcastId) = 0;
    virtual void displayLiveStatusDialog(const std::string& broadcastId) = 0;

    virtual void updateMusicButtonStatus(const TA_Base_Bus::IPAAgentCorbaDef::EMusicType& eCurType) = 0;
};

#endif // !defined(AFX_IStationBroadcastPage_H__B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_)
