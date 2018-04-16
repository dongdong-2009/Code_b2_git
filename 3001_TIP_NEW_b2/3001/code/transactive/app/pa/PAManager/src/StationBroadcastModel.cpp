#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/StationBroadcastModel.cpp $
  * @author:  Ripple
  * @version: $Revision: #15 $
  *
  * Last modification: $DateTime: 2017/04/20 14:22:59 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */


#include "app/pa/PAManager/src/stdafx.h"

#include "core/exceptions/src/ValueNotSetException.h"

#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "app/pa/PAManager/src/BroadcastParametersDlg.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/IStationBroadcastPage.h"
#include "app/pa/PAManager/src/PaLayoutHelper.h"
#include "app/pa/PAManager/src/PaManagerModel.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/StationBroadcastModel.h"
#include "app/pa/PAManager/src/StationDvaStatusModel.h"
#include "app/pa/PAManager/src/PAManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

static const int MAX_SELECTED_ITEMS = 4;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


StationBroadcastModel::StationBroadcastModel(IStationBroadcastPage& page) :
GenericBroadcastModel(page),
m_selectedZoneGroupName(""),
m_dwellInSecs(0),
m_startTime(0),
m_stopTime(0),
m_tmPeriod(0),
m_isSynchronisedWithTIS(false),
m_isMultiMsgBroadcast(false),//Multi-Messages function
m_hasChime(true),
m_cyclicalEnabled(false),
m_bCycSun( false ),
m_bCycMon( false ),
m_bCycTue( false ),
m_bCycWed( false ),
m_bCycThu( false ),
m_bCycFri( false ),
m_bCycSat( false ),
m_stationBroadcastPage(page),
m_broadcastMusicSource(TA_Base_Bus::IPAAgentCorbaDef::NoMusic)
// Please ensure that this initialisation list corresponds with the state of the
{
    FUNCTION_ENTRY( "StationBroadcastModel" );

    m_selectedMessages.clear();
    m_selectedZones.clear();

    FUNCTION_EXIT;
}


StationBroadcastModel::~StationBroadcastModel()
{
    FUNCTION_ENTRY( "~StationBroadcastModel" );

    FUNCTION_EXIT;
}


void StationBroadcastModel::executeBroadcast()
{
    FUNCTION_ENTRY( "executeBroadcast" );

    {
        ThreadGuard sync(m_lock);
        // if it is a schematc group then simplify the name
        if (m_selectedZoneGroupName.find("Schematic") != std::string::npos)
        {
            m_selectedZoneGroupName = "Schematic";
        }
    }

    // Make cursor into hour glass for duration of the execution
    // Note: If the function succeeds and brings up the new dialog, 
    // the cursor will automatically be restored to normal..
    CWaitCursor hourGlassCursor;

    try
    {
    	switch (m_broadcastType.getValue())
    	{
        // Music broadcasts are no longer executed (performed automatically)
    	/*
        case TA_Base_Bus::IPAAgentCorbaDef::StationMusic:
        	{
    			executeMusic();
    		}
    		break;
        */
        case TA_Base_Bus::IPAAgentCorbaDef::StationDva:
    		{
    			executeDVA();
    		}
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::StationLive:
            {
    			executeLive();
    		}
            break;
    	default:
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognized BroadcastType" );
    		TA_THROW(TA_Base_Core::TransactiveException("Unrecognized BroadcastType"));
    	}
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastLimitReached:
            PaErrorHandler::displayError(PaErrorHandler::MAXIMUM_BROADCAST_COUNT_REACHED);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eZoneNotMapped:
            PaErrorHandler::displayError(PaErrorHandler::ZONE_NOT_MAPPED);
            break;
        default:
            break;
        }
    }
    catch(const TA_Base_Core::ValueNotSetException&)
    {
        // The broadcast type hasn't been set, abort broadcast
        PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
    }
    catch(const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (...)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
    }

    FUNCTION_EXIT;
}


void StationBroadcastModel::executeMusic(const TA_Base_Bus::IPAAgentCorbaDef::EMusicType& musicSource)
{
    FUNCTION_ENTRY( "executeMusic" );

    ThreadGuard sync(m_lock);

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
    TA_IRS_Bus::PaZoneRecords paRecords;
    if ( CachedConfig::getInstance()->isAtOcc() )
    {
        paRecords = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().getAllRecords();
    } 
    else
    {
        paRecords = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().getPaZoneRecordsFromLocationKey(CachedConfig::getInstance()->getSchematicLocKey());
    }
    zones.length(paRecords.size());
    size_t loop = 0;
    for (TA_IRS_Bus::PaZoneRecordsIt it = paRecords.begin(); it != paRecords.end(); ++it, ++loop)
    {
        zones[loop] = it->m_pKey;
    }

    try
    {
        if (PaRightsManager::getInstance().hasRight( PaRightsManager::PA_BROADCAST_STATION_MUSIC ))
        {
            // [Work around start] For music status can not be retrieved from PAS
            m_broadcastMusicSource = musicSource;
            // [Work around end] For music status can not be retrieved from PAS

            CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), broadcastStationMusic, //limin
                       ( musicSource, zones, CachedConfig::getInstance()->getSessionId().c_str() ) );
            m_broadcastMusicSource = musicSource;
        }
        else
        {
            // No right to perform this operation
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
        }
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            m_broadcastMusicSource = TA_Base_Bus::IPAAgentCorbaDef::NoMusic;
            PaErrorHandler::displayError(PaErrorHandler::FEPC_HARDWARE_FAILURE);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            // The request failed (just won't be carried out)
            PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastStationMusicFailure:
            PaErrorHandler::displayError(PaErrorHandler::PA_PROTOCOL_FAILURE);
            break;
        default:
            break;
        }
    }
    catch(const TA_Base_Core::ValueNotSetException&)
    {
        // The broadcast type hasn't been set, abort broadcast
        PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
    }
    catch(const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (...)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
    }

    // refresh music button status
    m_stationBroadcastPage.updateMusicButtonStatus( m_broadcastMusicSource );

    FUNCTION_EXIT;
}


void StationBroadcastModel::validateAndUpdateStartAndStop(const time_t tmStart, const time_t tmStop)
{
    FUNCTION_ENTRY( "validateAndUpdateStartAndStop" );

    setStart(tmStart);
    setStop(tmStop);

    FUNCTION_EXIT;
}


void StationBroadcastModel::executeDVA()
{
    FUNCTION_ENTRY( "executeDVA" );

    TA_Base_Bus::TransActiveMessage userMsg;
	//Add  mulit-message warning
	if( m_isMultiMsgBroadcast )
	{
		if (IDNO == userMsg.showMsgBox( IDS_UW_670003 ))
		{
			FUNCTION_EXIT;
			return;
		}
	}
	else if(m_isSynchronisedWithTIS)
	{
        std::list<unsigned long>::reverse_iterator itMessageFirst = m_selectedMessages.rbegin();
		TA_IRS_Bus::StationDvaMessageRecord Record = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(*itMessageFirst);
		userMsg << Record.strMsgName;
		if (IDNO == userMsg.showMsgBox( IDS_UW_670004 ))
		{
			FUNCTION_EXIT;
			return;
		}
	}
	else
	{
		if (IDNO == userMsg.showMsgBox( IDS_UW_670002 ))
		{
			FUNCTION_EXIT;
			return;
		}
	}

    std::string broadcastId;
    {
        ThreadGuard sync(m_lock);

        KeysIt it;

        TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq messages;
        messages.length(m_selectedMessages.size());
        int i = 0;

        // TES#748 - Traverse the selected messages list in reverse order, because items
        // at bottom of list (visually) are located at the head of the array (because
        // they were added to the list most recently, and the old items pop off the top
        // of the list) - and we want items at the top of the list to broadcast first
        for (std::list<unsigned long>::reverse_iterator messageItr = m_selectedMessages.rbegin(); 
                messageItr != m_selectedMessages.rend(); ++messageItr, ++i)
        {
            messages[i] = *messageItr;
        }

		if(!m_isMultiMsgBroadcast)
		{
            messages.length(1);
		}
        
        TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
        zones.length(m_selectedZones.size());
        i = 0;
        for (it=m_selectedZones.begin(); it!=m_selectedZones.end(); ++it, ++i)
        {
            zones[i] = *it;
        }

        if (!PaRightsManager::getInstance().hasRight(PaRightsManager::PA_BROADCAST_STATION_DVA))
        {
            // No right to make this broadcast
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);

            FUNCTION_EXIT;
            return;
        }

        if (!m_cyclicalEnabled)
        {
            CORBA_CALL_RETURN( broadcastId, CachedConfig::getInstance()->getAgentObject(), broadcastStationDva, (   
                messages, zones, unsigned short(m_dwellInSecs), m_hasChime, m_isSynchronisedWithTIS, 
                m_selectedZoneGroupName.c_str(), CachedConfig::getInstance()->getSessionId().c_str() ) );
        }
        else
        {
            TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams stCyclicParams;
            stCyclicParams.bCycSun = m_bCycSun;
            stCyclicParams.bCycMon = m_bCycMon;
            stCyclicParams.bCycTue = m_bCycTue;
            stCyclicParams.bCycWed = m_bCycWed;
            stCyclicParams.bCycThu = m_bCycThu;
            stCyclicParams.bCycFri = m_bCycFri;
            stCyclicParams.bCycSat = m_bCycSat;
            stCyclicParams.startTime = m_startTime;
            stCyclicParams.stopTime = m_stopTime;
            stCyclicParams.period = m_tmPeriod;
            stCyclicParams.usScheduleId = 0u; // Managed by Agent
            CORBA_CALL_RETURN( broadcastId, CachedConfig::getInstance()->getAgentObject(), broadcastStationScheduleDva, (   
                messages, zones, unsigned short(m_dwellInSecs), m_hasChime, m_isSynchronisedWithTIS,
                m_selectedZoneGroupName.c_str(), stCyclicParams, CachedConfig::getInstance()->getSessionId().c_str() ) );
        }
    }

    // Pop up the dialog
    m_stationBroadcastPage.displayDvaStatusDialog(broadcastId);

    FUNCTION_EXIT;
}


void StationBroadcastModel::executeLive()
{
    FUNCTION_ENTRY( "executeLive" );

    std::auto_ptr<ThreadGuard> sync(new ThreadGuard(m_lock));

    KeysIt it;
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;

    zones.length(m_selectedZones.size());

    int i = 0;
    for (it=m_selectedZones.begin(); it!=m_selectedZones.end(); ++it, ++i)
    {
        zones[i] = *it;
    }
    
    if (PaRightsManager::getInstance().hasRightToBroadcastStationLive())
    {
        std::string broadcastId;
    	CORBA_CALL_RETURN( broadcastId, CachedConfig::getInstance()->getAgentObject(), broadcastStationLive, ( 
				  m_hasChime,
                  CachedConfig::getInstance()->getLocalConsoleRecordingSourceId(), 
                  zones,
                  m_selectedZoneGroupName.c_str(),
                  CachedConfig::getInstance()->getSessionId().c_str() ) );//limin

        // We must release the model lock before displaying broadcast - since
        // there may be interactions while the stationBroadcastPage modal dialog displayed
        delete (sync.release());

        m_stationBroadcastPage.displayLiveStatusDialog(broadcastId);
    }
    else
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
    }

    FUNCTION_EXIT;
}


void StationBroadcastModel::setSelectedMessages(const std::list<unsigned long>& newSelectedMessages, bool updateView)
{
    FUNCTION_ENTRY( "setSelectedMessages" );

    {
        ThreadGuard sync(m_lock);
        m_selectedMessages = newSelectedMessages;
    }

    if (updateView)
    {
        // Update changes visually
        onSelectedMessagesListUpdate();
    }

    FUNCTION_EXIT;
}


void StationBroadcastModel::switchSelectedMsgKey(const unsigned int unDragOrder, const unsigned int unDropOrder)
{
    FUNCTION_ENTRY( "switchSelectedMsgKey" );

    if (( m_selectedMessages.size() <= unDragOrder ) || ( m_selectedMessages.size() <= unDropOrder ))
    {
        return;
    }

    std::list<unsigned long>::iterator itDrag = m_selectedMessages.begin();
    const int unInvertDragOrder = m_selectedMessages.size() - 1u - unDragOrder;
    for ( unsigned int unDragLoop = 0u; unDragLoop < unInvertDragOrder; ++unDragLoop )
    {
        ++itDrag;
    }

    std::list<unsigned long>::iterator itDrop = m_selectedMessages.begin();
    const int unInvertDropOrder = m_selectedMessages.size() - 1u - unDropOrder;
    for ( unsigned int unDropLoop = 0u; unDropLoop < unInvertDropOrder; ++unDropLoop )
    {
        ++itDrop;
    }

    unsigned long ulTemp = *itDrop;
    *itDrop = *itDrag;
    *itDrag = ulTemp;

    FUNCTION_EXIT;
}


void StationBroadcastModel::unsetBroadcastType()
{
    FUNCTION_ENTRY( "unsetBroadcastType" );

    ThreadGuard sync(m_lock);

    m_broadcastType.resetValue();

    FUNCTION_EXIT;
}


void StationBroadcastModel::setBroadcastType(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType)
{
    FUNCTION_ENTRY( "setBroadcastType" );

	if ( broadcastType == TA_Base_Bus::IPAAgentCorbaDef::StationMusic)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Use extended function" );

		FUNCTION_EXIT;
		return;
	}

    ThreadGuard sync(m_lock);
    m_broadcastType.setValue(broadcastType);

    FUNCTION_EXIT;
}


void StationBroadcastModel::setChime(bool chime)
{
    FUNCTION_ENTRY( "setChime" );

    ThreadGuard sync(m_lock);
    m_hasChime = chime;

    FUNCTION_EXIT;
}


void StationBroadcastModel::setStisBroadcast(bool stisBroadcast)
{
    FUNCTION_ENTRY( "setStisBroadcast" );

    ThreadGuard sync(m_lock);
    m_isSynchronisedWithTIS = stisBroadcast;

    FUNCTION_EXIT;
}

//added by lixiaoxia
void StationBroadcastModel::setMultiMsgBroadcast(bool multiMsgBroadcast)
{
	FUNCTION_ENTRY( "setMultiMsgBroadcast" );

	ThreadGuard sync(m_lock);
	m_isMultiMsgBroadcast = multiMsgBroadcast;

	FUNCTION_EXIT;
}

void StationBroadcastModel::setDwell(time_t dwell)
{
    FUNCTION_ENTRY( "setDwell" );

    ThreadGuard sync(m_lock);
    m_dwellInSecs = dwell;

    FUNCTION_EXIT;
}


void StationBroadcastModel::setCyclicalEnabled(bool enabled)
{
    FUNCTION_ENTRY( "setCyclicalEnabled" );

    ThreadGuard sync(m_lock);
    m_cyclicalEnabled = enabled;

    FUNCTION_EXIT;
}


bool StationBroadcastModel::getCyclicalEnabled()
{
    FUNCTION_ENTRY( "getCyclicalEnabled" );

    ThreadGuard sync(m_lock);

    FUNCTION_EXIT;
    return m_cyclicalEnabled;
}


void StationBroadcastModel::setPeriod(const time_t period)
{
    FUNCTION_ENTRY( "setPeriod" );

    ThreadGuard sync(m_lock);
    m_tmPeriod = period;

    FUNCTION_EXIT;
}


time_t StationBroadcastModel::getPeriod()
{
    FUNCTION_ENTRY( "getPeriod" );

    ThreadGuard sync(m_lock);

    FUNCTION_EXIT;
    return m_tmPeriod;
}


void StationBroadcastModel::setStart(time_t start)
{
    FUNCTION_ENTRY( "setStart" );

    ThreadGuard sync(m_lock);
    m_startTime = start;

    FUNCTION_EXIT;
}


void StationBroadcastModel::setStop(time_t stop)
{
	ThreadGuard sync(m_lock);
	m_stopTime = stop;	
}


time_t StationBroadcastModel::getStart()
{
    FUNCTION_ENTRY( "getStart" );

    ThreadGuard sync(m_lock);

    FUNCTION_EXIT;
    return m_startTime;
}


time_t StationBroadcastModel::getStop()
{
    FUNCTION_ENTRY( "getStop" );

    ThreadGuard sync(m_lock);

    FUNCTION_EXIT;
    return m_stopTime;
}


std::list<unsigned long> StationBroadcastModel::getSelectedMessages()
{
    FUNCTION_ENTRY( "getSelectedMessages" );

    ThreadGuard sync(m_lock);

    FUNCTION_EXIT;
    return m_selectedMessages;
}


TA_IRS_Bus::StationDvaMessageRecords StationBroadcastModel::getNormalMessages(const std::string filterKeyword)
{
    FUNCTION_ENTRY( "getNormalMessages" );

    // Ownership transferred
    FUNCTION_EXIT;
    return TA_IRS_Bus::CachedMaps::getInstance()->getClonedPrerecordedMessageRecords( CachedConfig::getInstance()->isAtOcc(), 
        TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_NORMAL, filterKeyword );
}


TA_IRS_Bus::StationDvaMessageRecords StationBroadcastModel::getEmergencyMessages(const std::string filterKeyword)
{
    FUNCTION_ENTRY( "getEmergencyMessages" );

    // Ownership transferred
    FUNCTION_EXIT;
    return TA_IRS_Bus::CachedMaps::getInstance()->getClonedPrerecordedMessageRecords( 
        CachedConfig::getInstance()->isAtOcc(), TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_EMERGENCY, filterKeyword );
}


TA_IRS_Bus::StationDvaMessageRecords StationBroadcastModel::getAdhocMessages()
{
    FUNCTION_ENTRY( "getAdhocMessages" );

    // Ownership transferred
    FUNCTION_EXIT;
    return TA_IRS_Bus::CachedMaps::getInstance()->getClonedAdhocMessageRecords();
}


void StationBroadcastModel::onChangeNormalFilterTb(const std::string& filterKeyword)
{
    FUNCTION_ENTRY( "onChangeNormalFilterTb" );

    m_stationBroadcastPage.updateNormalMessageList(getNormalMessages(filterKeyword));

    FUNCTION_EXIT;
}


void StationBroadcastModel::onChangeEmergencyFilterTb(const std::string& filterKeyword)
{
    FUNCTION_ENTRY( "onChangeEmergencyFilterTb" );

    m_stationBroadcastPage.updateEmergencyMessageList(getEmergencyMessages(filterKeyword));

    FUNCTION_EXIT;
}


void StationBroadcastModel::onAdhocListUpdate()
{
    FUNCTION_ENTRY( "onAdhocListUpdate" );

    m_stationBroadcastPage.updateAdhocMessageList(getAdhocMessages());

    FUNCTION_EXIT;
}


void StationBroadcastModel::onSelectedMessagesListUpdate()
{
    FUNCTION_ENTRY( "onSelectedMessagesListUpdate" );

    // Clone records for the update of the list control
    std::list<unsigned long> keys = getSelectedMessages();

    TA_IRS_Bus::StationDvaMessageRecords records;

    for (std::list<unsigned long>::iterator it = keys.begin(); it != keys.end(); ++it)
    {
        TA_IRS_Bus::StationDvaMessageRecord clonedRecord = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(*it);
        records.push_back(clonedRecord);
    }

    m_stationBroadcastPage.updateSelectedMessagesList(records);
    m_stationBroadcastPage.updateExecuteButtonEnabledState();

    FUNCTION_EXIT;
}


void StationBroadcastModel::onClearOneBtn(int nItem)
{
    FUNCTION_ENTRY( "onClearOneBtn" );

    std::list<unsigned long> keys = getSelectedMessages();
    // The messages are sorted added to the list in such a way
    // that they are numbered inversely (that is, '1' visually at the
    // top of list is the last element of this vector array)
    std::list<unsigned long>::iterator it = keys.begin();

    const int nInvertIndex = keys.size() - 1 - nItem;
    // nItem will be between 0 and size-1
    for (int i = 0 ; i < nInvertIndex; ++i)
    {
        ++it;
    }
    keys.erase(it);

    setSelectedMessages(keys, true);

    FUNCTION_EXIT;
}


void StationBroadcastModel::onClearAllBtn()
{
    FUNCTION_ENTRY( "onClearAllBtn" );

    std::list<unsigned long> keys;
    setSelectedMessages(keys, true);

    FUNCTION_EXIT;
}


void StationBroadcastModel::addToSelectedMessagesList(unsigned long key)
{
    FUNCTION_ENTRY( "addToSelectedMessagesList" );

    if ( TA_Base_Core::DatabaseKey::getInvalidKey() == key )
    {
        return;
    }

    TA_IRS_Bus::StationDvaMessageRecord oDvaMsg = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( key );
    if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == oDvaMsg.usMsgType )
    {
        return;
    }

    std::list<unsigned long> newKeys;
    newKeys.push_back(key);

    std::list<unsigned long> currentKeys = getSelectedMessages();

    // Would be better to use push_front here, then lop off the back end..
    if ( m_isMultiMsgBroadcast )
    {
        std::list<unsigned long>::iterator it = currentKeys.begin();
        int count = 1;

        while (count < MAX_SELECTED_ITEMS && it != currentKeys.end())
        {
            unsigned long nextKey = *it;
            newKeys.push_back(nextKey);
            ++it;
            ++count;
        }
    }
    else if( currentKeys.size() > 1 )
    {
        std::list<unsigned long>::reverse_iterator itLastMsg = currentKeys.rbegin();
        *itLastMsg =  key;
        newKeys = currentKeys;
    }

    setSelectedMessages(newKeys, true);

    FUNCTION_EXIT;
}


void StationBroadcastModel::validateNewAdhocLabel( std::string& strNewLabel )
{
    FUNCTION_ENTRY( "validateNewAdhocLabel" );

    if (strNewLabel.length() > TA_Base_Bus::IPAAgentCorbaDef::MAX_ADHOC_LABEL_LENGTH)
    {
        // Truncate end of string if too long
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "InvalidState", "Adhoc label allowed to be too long");

        strNewLabel = strNewLabel.substr( 0, TA_Base_Bus::IPAAgentCorbaDef::MAX_ADHOC_LABEL_LENGTH );
    }

    FUNCTION_EXIT;
}


bool StationBroadcastModel::postAdhocLabelChange(unsigned long messageKey, const std::string& label)
{
    FUNCTION_ENTRY( "postAdhocLabelChange" );

    if (!PaRightsManager::getInstance().hasRight(PaRightsManager::PA_MODIFY_ADHOC_MESSAGE))
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);

        FUNCTION_EXIT;
        return false;
    }

    TA_IRS_Bus::StationDvaMessageRecord stMsg = 
        TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(messageKey);

    if (( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == stMsg.usMsgType ) || 
        ( stMsg.strMsgName.compare( label ) == 0 ))
    {
        FUNCTION_EXIT;
        return false;
    }

    ThreadGuard sync(m_lock);

    CWaitCursor hourGlassCursor;

    try
    {
        CORBA::Boolean ret = static_cast<CORBA::Boolean>(false);
        CORBA_CALL_RETURN( ret, CachedConfig::getInstance()->getAgentObject(), setAdhocLabel, (
            messageKey, label.c_str(), CachedConfig::getInstance()->getSessionId().c_str() ) );

        FUNCTION_EXIT;
        return static_cast<bool>(ret);
    }
    catch(const TA_Base_Core::BadParameterException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
    }
    catch (const TA_Base_Core::AccessDeniedException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
    }
    catch (const TA_Base_Core::DatabaseErrorException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::DATABASE_ERROR);
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        // Comms error (couldn't resolve agent)
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch(const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNABLE_TO_PERFORM_REQUEST);
    }
    catch(const TA_Base_Core::TransactiveException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNABLE_TO_PERFORM_REQUEST);
    }

    FUNCTION_EXIT;
    return false;
}


bool StationBroadcastModel::postAdhocTypeChange(unsigned long messageKey, unsigned short newMessageType)
{
    FUNCTION_ENTRY( "postAdhocTypeChange" );

    if (!PaRightsManager::getInstance().hasRight(PaRightsManager::PA_MODIFY_ADHOC_MESSAGE))
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);

        FUNCTION_EXIT;
        return false;
    }

    const unsigned short usCurMsgType = 
        TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(messageKey).usMsgType;
	/*
    if (( usCurMsgType == newMessageType ) || 
        ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == usCurMsgType ))
		*/
    if ( usCurMsgType == newMessageType )
    {
        FUNCTION_EXIT;
        return false;
    }

    ThreadGuard sync(m_lock);

    CWaitCursor hourGlassCursor;

    try
    {
        CORBA::Boolean ret;
        CORBA_CALL_RETURN( ret, CachedConfig::getInstance()->getAgentObject(), setAdhocType, (
            messageKey, newMessageType, CachedConfig::getInstance()->getSessionId().c_str() ) );

        bool bResult = static_cast< bool >( ret );

        if( !bResult )
        {
            PaErrorHandler::displayError(PaErrorHandler::AD_HOC_TYPE_NOT_CHANGED);
        }

        FUNCTION_EXIT;
        return bResult;
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            PaErrorHandler::displayError(PaErrorHandler::FEPC_HARDWARE_FAILURE);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eSetAdhocTypeFailure:
            PaErrorHandler::displayError(PaErrorHandler::UNABLE_TO_PERFORM_REQUEST);
            break;
        default:
            break;
        }
    }
    catch (const TA_Base_Core::DatabaseErrorException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::DATABASE_ERROR);
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch(const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch(...)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
    }

    FUNCTION_EXIT;
    return false;
}


Keys StationBroadcastModel::getZonesAssociatedWithGroup(const std::string& label)
{
    FUNCTION_ENTRY( "getZonesAssociatedWithGroup" );

    std::list<unsigned long> listZoneKeys = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupCachedMap().
        getZonesAssociatedWithGroup( CachedConfig::getInstance()->getSchematicLocKey(), label );

    Keys vecZoneKeys;
    for ( std::list<unsigned long>::iterator itLoop = listZoneKeys.begin(); listZoneKeys.end() != itLoop; ++itLoop )
    {
        vecZoneKeys.push_back( *itLoop );
    }

    // Allow exceptions to propogate
    FUNCTION_EXIT;
    return vecZoneKeys;
}


void StationBroadcastModel::removeSelectedMessagesWithKey(unsigned long key)
{
    FUNCTION_ENTRY( "removeSelectedMessagesWithKey" );

    std::list<unsigned long> keys = getSelectedMessages();

    // Remove all keys with matching type
    keys.remove(key);

    // Expect input corresponds to one of existing selected items
	if ( keys == getSelectedMessages() )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No removal occurred" );

		FUNCTION_EXIT;
		return;
	}

    setSelectedMessages(keys, true);

    FUNCTION_EXIT;
}


DWORD StationBroadcastModel::getSelectedMessageFlags()
{
    FUNCTION_ENTRY( "getSelectedMessageFlags" );

    DWORD resultFlags = 0;

    for (std::list<unsigned long>::iterator it = m_selectedMessages.begin(); it != m_selectedMessages.end(); it ++)
    {
        try
        {
            TA_IRS_Bus::StationDvaMessageRecord record =
               TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(*it);

            if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY == record.usMsgType )
            {
                resultFlags = EMERGENCY_ADHOC;
            }
            else if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL == record.usMsgType )
            {
                resultFlags = NORMAL_ADHOC;
            }
            else if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_EMERGENCY == record.usMsgType )
            {
                resultFlags = EMERGENCY_PRERECORDED;
            }
            else if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_NORMAL == record.usMsgType )
            {
                resultFlags = NORMAL_PRERECORDED;
            }
        }
        catch (const TA_Base_Core::TransactiveException&)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Invalid message", "Selected message not found");
        }
    }

    FUNCTION_EXIT;
    return resultFlags;
}

void StationBroadcastModel::setCycSun( bool bEnable )
{
    FUNCTION_ENTRY( "setCycSun" );

    m_bCycSun = bEnable;

    FUNCTION_EXIT;
}

bool StationBroadcastModel::getCycSun()
{
    FUNCTION_ENTRY( "getCycSun" );

    FUNCTION_EXIT;
    return m_bCycSun;
}

void StationBroadcastModel::setCycMon( bool bEnable )
{
    FUNCTION_ENTRY( "setCycMon" );

    m_bCycMon = bEnable;

    FUNCTION_EXIT;
}

bool StationBroadcastModel::getCycMon()
{
    FUNCTION_ENTRY( "getCycMon" );

    FUNCTION_EXIT;
    return m_bCycMon;
}

void StationBroadcastModel::setCycTue( bool bEnable )
{
    FUNCTION_ENTRY( "setCycTue" );

    m_bCycTue = bEnable;

    FUNCTION_EXIT;
}

bool StationBroadcastModel::getCycTue()
{
    FUNCTION_ENTRY( "getCycTue" );

    FUNCTION_EXIT;
    return m_bCycTue;
}

void StationBroadcastModel::setCycWed( bool bEnable )
{
    FUNCTION_ENTRY( "setCycWed" );

    m_bCycWed = bEnable;

    FUNCTION_EXIT;
}

bool StationBroadcastModel::getCycWed()
{
    FUNCTION_ENTRY( "getCycWed" );

    FUNCTION_EXIT;
    return m_bCycWed;
}

void StationBroadcastModel::setCycThu( bool bEnable )
{
    FUNCTION_ENTRY( "setCycThu" );

    m_bCycThu = bEnable;

    FUNCTION_EXIT;
}

bool StationBroadcastModel::getCycThu()
{
    FUNCTION_ENTRY( "getCycThu" );

    FUNCTION_EXIT;
    return m_bCycThu;
}

void StationBroadcastModel::setCycFri( bool bEnable )
{
    FUNCTION_ENTRY( "setCycFri" );

    m_bCycFri = bEnable;

    FUNCTION_EXIT;
}

bool StationBroadcastModel::getCycFri()
{
    FUNCTION_ENTRY( "getCycFri" );

    FUNCTION_EXIT;
    return m_bCycFri;
}

void StationBroadcastModel::setCycSat( bool bEnable )
{
    FUNCTION_ENTRY( "setCycSat" );

    m_bCycSat = bEnable;

    FUNCTION_EXIT;
}

bool StationBroadcastModel::getCycSat()
{
    FUNCTION_ENTRY( "getCycSat" );

    FUNCTION_EXIT;
    return m_bCycSat;
}

void StationBroadcastModel::updateZoneList( const std::vector<unsigned long>& vecZoneKeys )
{
    FUNCTION_ENTRY( "updateZoneList" );

    m_selectedZones = vecZoneKeys;

    FUNCTION_EXIT;
}

void StationBroadcastModel::updateCurZoneGroup( const std::string& strGroupName )
{
    FUNCTION_ENTRY( "updateCurZoneGroup" );

    m_selectedZoneGroupName = strGroupName.c_str();

    FUNCTION_EXIT;
}

Keys StationBroadcastModel::getSelectedZones()
{
    FUNCTION_ENTRY( "getSelectedZones" );

    FUNCTION_EXIT;
    return m_selectedZones;
}