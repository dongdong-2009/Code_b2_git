/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/StationLiveStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * StationLiveStatusModel controls additional layer of business logic for Live
  * broadcast dialog
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/StationLiveStatusModel.h"
#include "app/pa/PAManager/src/CachedConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


StationLiveStatusModel::StationLiveStatusModel(GenericStatusPage& page, const std::string& broadcastId) :
LiveStatusModel(page, broadcastId, GenericStatusModel::STATION)
{
}


StationLiveStatusModel::~StationLiveStatusModel()
{
}

void StationLiveStatusModel::removeAnnouncement()
{
    FUNCTION_ENTRY( "removeAnnouncement" );

    try
    {
        CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), removeBroadcast, ( getBroadcastId().c_str(), "" ) );
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
    }
    catch (const TA_Base_Core::ObjectResolutionException& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", expWhat.what() );
    }
    catch(const CORBA::Exception&)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", "Agent fail" );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unknown exception when removeAnnouncement" );
    }

    FUNCTION_EXIT;
}
