/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/TrainDvaStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * TrainDvaStatusModel controls additional layer of business logic for DVA
  * specific broadcast dialog
  *
  */

#include "app/trains/TrainBorne_PA/src/StdAfx.h"
#include "app/trains/TrainBorne_PA/src/TrainDvaStatusModel.h"
//#include "app/trains/TrainBorne_PA/src/StationBroadcastModel.h"
#include "core/exceptions/src/PasExceptions.h"
//#include "app/trains/TrainBorne_PA/src/PaManagerModel.h"
#include "app/trains/TrainBorne_PA/src/CachedConfig.h"

#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TrainDvaStatusModel::TrainDvaStatusModel(GenericStatusPage& page, const std::string& broadcastId) :
DvaStatusModel(page, broadcastId, GenericStatusModel::TRAIN),
m_hasOverrideOption(false)
{
}


TrainDvaStatusModel::~TrainDvaStatusModel()
{
}


bool TrainDvaStatusModel::getHasOverrideOption()
{
    return m_hasOverrideOption;
}

void TrainDvaStatusModel::setHasOverrideOption(bool hasOption)
{
    m_hasOverrideOption = hasOption;
}

void TrainDvaStatusModel::removeAnnouncement()
{
    FUNCTION_ENTRY("removeAnnouncement");

    try
    {
        CORBA_CALL(CachedConfig::getInstance()->getAgentObject(), removeBroadcast, (getBroadcastId().c_str(), ""));
    }
    catch (const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in());
    }
    catch (const TA_Base_Core::ObjectResolutionException& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", expWhat.what());
    }
    catch (const CORBA::Exception&)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", "Agent fail");
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught unknown exception when removeAnnouncement");
    }

    FUNCTION_EXIT;
}
