/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/signs/RATISCallBannerLibrary/src/RATISCallEntry.cpp $
  * @author:  Rob Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
  * Last modified by:  $Author: builder $
  * 
  * This class extends AbstractCallEntry to provide the implementation of
  * a RATIS call entry.
  * When activated, it opens the RATIS approval manager with the corresponding
  * incoming RATIS message selected.
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "bus/signs/RATISCallBannerLibrary/src/RATISCallEntry.h"
#include "bus/signs/RATISCallBannerLibrary/src/RatisApprovalDialog.h"
#include "bus/signs_4669/TisManagerCorbaDef/src/ISTISManagerCorbaDef.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/exceptions/src/ApplicationException.h"

#include "bus\security\access_control\actions\src\AccessControlledActions.h"
#include "bus\security\rights_library\src\RightsLibrary.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core\data_access_interface\entity_access\src\CallBannerPage.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\src\IResource.h"
#include "core\data_access_interface\src\ResourceAccessFactory.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::CorbaUtil;

namespace TA_IRS_Bus
{
    RatisApprovalDialog* RATISCallEntry::s_dialog = NULL;

    const std::string RATISCallEntry::CALL_ENTRY_TYPE = "RATIS";
	const std::string RATIS_CALL_SOURCE = "RXH";
    const std::string RATISCallEntry::ALL_STATIONS = "AST";
    const std::string RATISCallEntry::ALL_TRAINS = "ATR";
    const std::string RATISCallEntry::TRAIN = "TRN";


    ///////////////////////////////////////////////////////////////////////////////////////////////
    /** 
     * DEFAULT_RATIS_CATCH_HANDLER
     *
     * Provides a default exception catch handler, exceptions
     * passed on to this function will show an error dialog showing
     * the prefixMessage, following by details of the exception
     *
     * @param prefixMessage additional information describing the 
     *      operation that led to the exception, do not append trailing full stop
     */
    #define DEFAULT_RATIS_CATCH_HANDLER(prefixMessage)\
    catch (const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException&)\
    {\
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException", "");\
        showExceptionCatchDetails(prefixMessage, "InvalidParameter", "");\
    }\
    catch (const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException&)\
    {\
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException", "");\
        showExceptionCatchDetails(prefixMessage, "STISCommunicationTimeoutException", "");\
    }\
    catch (const TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException&)\
    {\
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException", "");\
        showExceptionCatchDetails(prefixMessage, "STISFunctionalityNotAvailableException", "");\
    }\
    catch( const TA_Base_Core::TransactiveException& te )\
    {\
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());\
        showExceptionCatchDetails(prefixMessage, "TransactiveException", te.what());\
    }\
    catch( const CORBA::Exception& ce )\
    {\
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA", CorbaUtil::getInstance().exceptionToString(ce).c_str());\
        showExceptionCatchDetails(prefixMessage, "CORBA exception", CorbaUtil::getInstance().exceptionToString(ce).c_str());\
    }\
    catch (...)\
    {\
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "??");\
        showExceptionCatchDetails(prefixMessage, "Uknown exception", "");\
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////

    RATISCallEntry::RATISCallEntry(
        unsigned long id,
        const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& details,
        RATISCallFactory& factory)
        :
        AbstractCallEntry(
            id,
            RATIS_CALL_SOURCE,
            RATISCallFactory::getMessageInformation(details) ),

        m_details(details),
        m_RATISCallFactory(factory),
        m_resourceKey(0)
    {
        TA_Base_Bus::RightsLibraryFactory rightsFactory;
        m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);

        // Load the Control Station entity and retrieve all the data we need
        std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);

        TA_Base_Core::CallBannerPage* banner = NULL;
        TA_Base_Core::IResource* resource = NULL;
        try
        {
            banner = dynamic_cast<TA_Base_Core::CallBannerPage*>(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName));
            if (banner == NULL)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cannot find the call banner entity");
                return;
            }

            resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(banner->getKey());
            if (resource == NULL)
            {
                delete banner;
                banner = NULL;
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cannot find the resource key for the call banner");
                return;
            }

            m_resourceKey = resource->getKey();
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "IResource key = %d", m_resourceKey);
        }
        catch (...)
        {
            delete banner;
            banner = NULL;
            delete resource;
            resource = NULL;
        }
    }


    RATISCallEntry::~RATISCallEntry()
    {
    }


    void RATISCallEntry::update(const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& details)
    {
        m_details.messageID = details.messageID;
        m_details.sessionRef = details.sessionRef;
        m_details.startTime = details.startTime;
        m_details.endTime = details.endTime;
        m_details.requiresVetting = details.requiresVetting;
        m_details.overridable = details.overridable;
        m_details.type = details.type;
        m_details.status = details.status;
        m_details.timeCreated = details.timeCreated;
        m_details.destination = details.destination;
        m_details.tag = details.tag;
        m_details.priority = details.priority;
        m_details.messageText = details.messageText;
    }


    void RATISCallEntry::updateStatus(TA_Base_Core::ERATISMessageStatus status)
    {
        m_details.status = status;
    }


    std::string RATISCallEntry::getType() const
    {
		return CALL_ENTRY_TYPE;
    }

    
    void RATISCallEntry::onActivate()
    {
        FUNCTION_ENTRY("onActivate");

        // see if the operator has rights to do this
        try
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
            std::string reason;
            if (!m_rightsMgr->isActionPermittedOnResource(sessionId, m_resourceKey, TA_Base_Bus::aca_STATION_TIS_RATIS_APPROVE, reason, decisionModule))
            {
                // TODO: Use the TransActiveMessage library
				// TD14164 ++
                /*AfxMessageBox("Operator has insufficient rights to approve RATIS message.");*/
				TA_Base_Bus::TransActiveMessage userMsg;
				CString fieldName = "RATIS message";
				userMsg << fieldName;
				userMsg.showMsgBox(IDS_UE_070127);
				// ++ TD14164 
                return;
            }
        }
        catch (...)
        {
            // TODO: Use the TransActiveMessage library
			// TD14164 ++
            /*AfxMessageBox("Unable to determine the rights of the current operator.");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_010023);
			// ++ TD14164 
            return;
        }

        if(s_dialog == NULL)
        {
            s_dialog = new RatisApprovalDialog(this);
        }

        //
        // If the dialog is already open, don't overwrite it
        //
        else if( s_dialog->IsWindowVisible() )
        {
            // Force window into foreground, in case user doesn't realise
            // window is open
            s_dialog->SetForegroundWindow();
            return;
        }

        //
        // If the dialog is not open, set the call entry being
        // displayed to this object.
        //
        else
        {
            s_dialog->setCallEntry(this);
        }

        s_dialog->ShowWindow(SW_SHOW);

        FUNCTION_EXIT;
    }


    std::string RATISCallEntry::getLocationStr() const
    {
        if (isTTIS())
        {
            return RATISCallEntry::TRAIN;
        }
        else
        {
            // First 3 characters of destination is the station.
            std::string destination = m_details.destination;
            return destination.substr(0, 3);
        }
    }


    std::string RATISCallEntry::getStatusStr() const
    {
        return RATISCallFactory::getStatusAsString(m_details.status);
    }


    time_t RATISCallEntry::getCreateTime() const
    {
        return m_details.timeCreated;
    }
    
    int RATISCallEntry::getSeverity() const
    {
		//TODO - map the priority to severity if necessary;
		return 3;
    }


    std::string RATISCallEntry::getRATISTag() const
    {
        return m_details.tag;
    }


    TA_Base_Core::ERATISMessageType RATISCallEntry::getRATISType() const
    {
        return m_details.type;
    }


    std::string RATISCallEntry::getRATISTypeAsString() const
    {
        return RATISCallFactory::getTypeAsString(m_details.type);
    }


    std::string RATISCallEntry::getStartTime() const
    {
        return formatDate(std::string(m_details.startTime));
    }


    std::string RATISCallEntry::getEndTime() const
    {
        return formatDate(std::string(m_details.endTime));
    }


    std::string RATISCallEntry::getDestination() const
    {
        return m_details.destination;
    }

/*
    std::string RATISCallEntry::getDestination() const
    {
        std::string destination(m_details.destination);

        // e.g. the destination strings to expect:
        //  - CCLCVC000 (all station PIDS at CVC), 
        //  - CCLDBG002001002 (2 station PIDs at DBG)
        //  - CCLAST (All PIDs at all Stations)
        //  - CCLATR (All Trains)
        //  - CCLTRN001002 (1 train)

        // the first 3 characters is the system Id, next 3 is the station
        static const int beginStationIndex = 3;
        static const int stationSize = 3;
        static const int numIdSize = 3;
        static const int idSize = 3;
        static const int endStationIndex = beginStationIndex + stationSize;
        static const int endNumIdIndex = endStationIndex + numIdSize;

        std::string station = destination.substr(beginStationIndex, stationSize);

        if (station.compare("ATR") == 0)
        {
            return "All Trains";
        }
        else if (station.compare("AST") == 0)
        {
            return "All PIDs at all Stations";
        }
        else if (station.compare("TRN") == 0)
        {
            // TODO: what to do for train?
            std::string destStr = "Train ID: ";

            int numIds = atol(destination.substr(endStationIndex, numIdSize).c_str());
            // check if size is right
            int num = (destination.size() - endNumIdIndex) / idSize;
            if (num < numIds)
            {
                // the string is somehow incomplete, we'll just display what we can
                numIds = num;
            }

            for (int i = 0; i < numIds; i++)
            {
                destStr += " " + destination.substr(endNumIdIndex + i * idSize, idSize);
            }
            return destStr;
        }
        else
        {
            // Stations.  see how many items there are
            std::string destStr = station + ": ";

            // if the number of id is 0, then it means all pids are involved
            int numIds = atol(destination.substr(endStationIndex, numIdSize).c_str());
            if (numIds == 0)
            {
                destStr += "All PIDs";
                return destStr;
            }

            // check if size is right
            int num = (destination.size() - endNumIdIndex) / idSize;
            if (num < numIds)
            {
                // the string is somehow incomplete, we'll just display what we can
                numIds = num;
            }

            for (int i = 0; i < numIds; i++)
            {
                destStr += " " + m_RATISCallFactory.getPidName(station, destination.substr(endNumIdIndex + i * idSize, idSize));
            }

            return destStr;
        }
        return destination;
    }
*/

    int RATISCallEntry::getPriority() const
    {
        return m_details.priority;
    }


    RATISCallEntry::EPriorityGroup RATISCallEntry::getPriorityGroup(const int priority) const
    {
        switch (priority)
        {
        case 1:
        case 2:
        case 3:
            return EMERGENCY;
        case 6:
            return PRIORITY6;
        case 4:
        case 5:
        case 7:
        case 8:
        default:
            return NORMAL;
        }

        // keep the compiler happy
        return NORMAL;
    }


    bool RATISCallEntry::isOverridable() const
    {
        return ( m_details.overridable == CORBA::Boolean(true) );
    }


    bool RATISCallEntry::isClearRequest() const
    {
        return ( m_details.type == TA_Base_Core::RATIS_IN_CLEAR || m_details.type == TA_Base_Core::RATIS_OUT_CLEAR );
    }


    bool RATISCallEntry::isTTIS() const
    {
        return m_details.isTTIS;
        /*
        std::string station(m_details.destination);
        station = station.substr(3, 3);

        if ((station.compare("ATR") == 0) || (station.compare("TRN") == 0))
        {
            return true;
        }

        return false;
        */
	}


    int RATISCallEntry::getMessageContentMaximumLength() const
    {
        if (isTTIS())
        {
            return 124;
        }
        return 256; // STIS
    }


    void RATISCallEntry::submitRATISVettingResponse(bool approved, const unsigned short priority /* = 0 */, const char* content /* NULL */)
    {
        try
        {
            std::string sessionID = RunParams::getInstance().get(RPARAM_SESSIONID);

            if (!approved)
            {
                CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), submitRATISVettingResponse, ( m_details.messageID, 0, 0, "", sessionID.c_str() ) );//limin
                return;
            }
    
            CORBA::UShort newPriority = 0;
            std::string newContent("");
            if (isTTIS() && isClearRequest())
            {
                // determine if it is still the same group
                if (getPriorityGroup(priority) != getPriorityGroup(m_details.priority))
                {
                    newPriority = priority;
                }
            }
            else if (isOverridable())
            {
                // set the priority and content only if they're different to before
                if (priority != m_details.priority)
                {
                    newPriority = priority;
                }

                newContent = content;
                if (newContent.compare(m_details.messageText) == 0)
                {
                    newContent = "";
                }
            }
            CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), submitRATISVettingResponse, ( m_details.messageID, 1, newPriority, newContent.c_str(), sessionID.c_str() ) );//limin
        }
        DEFAULT_RATIS_CATCH_HANDLER("Failed to submit RATIS vetting response");
    }


    void RATISCallEntry::showExceptionCatchDetails(const char* prefixMessage, const char* exceptionName, const char* exceptionDetails)
    {
        std::ostringstream errorStr;

        if (0 != prefixMessage)
        {
            errorStr << prefixMessage << ": ";
        }

        // Format messages as:
        // prefixMessage: ExceptionName (<details>)
        // Failed to reset call: InvalidParameterException (call ID not found)
        if (0 != exceptionName)
        {
            errorStr << exceptionName;
        }
        else
        {
            errorStr << "Unknown exception";
        }

        if (0 != exceptionDetails && strlen(exceptionDetails) > 0)
        {
            errorStr << " (" << exceptionDetails << ")";
        }

        showErrorMessage(errorStr.str());
    }


    void RATISCallEntry::showErrorMessage(const std::string& errorMessage)
    {
        TA_ASSERT(
            s_dialog != NULL,
            "Cannot display error message when dialog is NULL");


        s_dialog->showErrorMessage(errorMessage);
    }


    std::string RATISCallEntry::formatDate(const std::string& timeStr) const
    {
        // the date time is in the form of YYYYMMDDHHMISS, we're going to change it into
        // DD-MM-YYYY HH24:MI:SS 

        if (timeStr.size() != 14)
        {
            return "Invalid Date";
        }

        std::string year = timeStr.substr(0, 4);
        std::string month = timeStr.substr(4, 2);
        std::string day = timeStr.substr(6, 2);
        std::string hour = timeStr.substr(8, 2);
        std::string minute = timeStr.substr(10, 2);
        std::string second = timeStr.substr(12, 2);

        std::string date = day + "-" + month + "-" + year + " " + hour + ":" + minute + ":" + second;

        // TODO: make this 24 hours
//        std::string date = hour + ":" + minute + ":" + second;
        return date;
    }



} // TA_IRS_Bus
