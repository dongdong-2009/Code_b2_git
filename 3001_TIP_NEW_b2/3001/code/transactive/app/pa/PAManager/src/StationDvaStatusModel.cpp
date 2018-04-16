/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/StationDvaStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2014/05/16 18:00:29 $
  * Last modified by: $Author: qi.huang $
  * 
  * StationDvaStatusModel controls additional layer of business logic for DVA
  * specific broadcast dialog
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/StationDvaStatusModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


StationDvaStatusModel::StationDvaStatusModel(GenericStatusPage& page, const std::string& broadcastId) :
DvaStatusModel(page, broadcastId, GenericStatusModel::STATION)
{
}


StationDvaStatusModel::~StationDvaStatusModel()
{
}


void StationDvaStatusModel::removeAnnouncement()
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