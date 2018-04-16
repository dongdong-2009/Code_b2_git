#include <iomanip>
#include <time.h>
#include "ActiveStationTisStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "bus/signs/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent/IDL/src/TISMessageDataDefinitions.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryTable.h"

namespace TA_Base_App
{
    ActiveStationTisStep::ActiveStationTisStep(const BasicParameter& detail,
                                               IActivePlan& flowControl)
        : ActiveStep(m_params, flowControl),
          m_params(detail)
    {
    }

    ActiveStationTisStep::~ActiveStationTisStep()
    {
    }

    bool ActiveStationTisStep::executeImpl()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Station TIS step %lu", id());

        bool bExecSucceed = false;

        try
        {
            //check the version
            unsigned short currentVersion;
            CORBA_CALL_RETURN(currentVersion, TA_Base_Bus::TISAgentAccessFactory::getInstance().getSTISAgent(), getCurrentSTISMessageLibraryVersion, ());  //limin

            if(currentVersion == m_params.message.libraryVersion)
            {
                //get the message from DB
                std::string libraryType(TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);
                TA_Base_Core::PredefinedMessage message =
                    TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessage(currentVersion,
                                                                                                            libraryType,
                                                                                                            (int)m_params.message.librarySection,
                                                                                                            (unsigned short)m_params.message.messageId);

                //get start time and ent time
                //get the current time as the start time
                time_t startTimet = time(NULL);
                time_t endTimet = startTimet + m_params.duration;

                struct tm startTM = *(localtime(&startTimet));
                std::stringstream timeString;
                timeString  << startTM.tm_year + 1900
                            << std::setw(2) << std::setfill('0') << startTM.tm_mon + 1
                            << std::setw(2) << std::setfill('0') << startTM.tm_mday
                            << std::setw(2) << std::setfill('0') << startTM.tm_hour
                            << std::setw(2) << std::setfill('0') << startTM.tm_min
                            << std::setw(2) << std::setfill('0') << startTM.tm_sec;
                std::string startTime = timeString.str();

                struct tm endTM = *(localtime(&endTimet));
                timeString.str("");
                if(startTM.tm_mday != endTM.tm_mday)
                {
                    //the end time is in the next day, so use the "23:59:59" instead
                    timeString  << startTM.tm_year + 1900
                                << std::setw(2) << std::setfill('0') << startTM.tm_mon + 1
                                << std::setw(2) << std::setfill('0') << startTM.tm_mday
                                << std::setw(2) << std::setfill('0') << 23
                                << std::setw(2) << std::setfill('0') << 59
                                << std::setw(2) << std::setfill('0') << 59;
                }
                else
                {
                    timeString  << endTM.tm_year + 1900
                                << std::setw(2) << std::setfill('0') << endTM.tm_mon + 1
                                << std::setw(2) << std::setfill('0') << endTM.tm_mday
                                << std::setw(2) << std::setfill('0') << endTM.tm_hour
                                << std::setw(2) << std::setfill('0') << endTM.tm_min
                                << std::setw(2) << std::setfill('0') << endTM.tm_sec;
                }
                std::string endTime = timeString.str();

                //convert the destinatonList
                //in plan we only can select all station or one station,
                //so the destionationList length is 1
                TA_Base_Core::STISDestinationList destinationList;
                destinationList.length(1);

                if(m_params.allStations)
                {
                    destinationList[0].station = "AST";
                }
                else
                {
                    destinationList[0].station = m_params.station.c_str();
                }

                destinationList[0].pids.length(m_params.stationPids.size());
                for(unsigned int i = 0 ; i < m_params.stationPids.size() ; ++i)
                {
                    destinationList[0].pids[i] = m_params.stationPids[i].c_str();
                }

                std::string sessionId = m_iPlan.session();
                //std::string sessionId = ActivePlanBulletinBoard::instance()->getSession(m_iPlan.getPlan());
                //call tis agent to send the message
                CORBA_CALL(TA_Base_Bus::TISAgentAccessFactory::getInstance().getSTISAgent(), submitPredefinedDisplayRequest, (destinationList,
                                                                                                                              message.librarySection == 1 ? TA_Base_Core::NORMAL_SECTION : TA_Base_Core::EMERGENCY_SECTION,
                                                                                                                              currentVersion,
                                                                                                                              m_params.message.messageId,
                                                                                                                              startTime.c_str(),
                                                                                                                              endTime.c_str(),
                                                                                                                              message.priority,
                                                                                                                              sessionId.c_str()));  //limin

                //On successful completion of the step
                bExecSucceed = true;
            }
            else
            {
                std::ostringstream msg;
                msg << dumpContext() << ": ";
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "%s STIS message version conflict. Failed to run plan.", msg.str().c_str());
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << e.what();

            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", msg.str().c_str());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << e.what();

            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", msg.str().c_str());
        }
        catch(const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException&)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << "Timed out while waiting for STIS server response";

            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationTimeoutException", msg.str().c_str());
        }
        catch(const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException&)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << "STIS Communications error.";

            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationFailureException", msg.str().c_str());
        }
        catch(const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException&)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << "TIS Agent not connected to STIS Server.";

            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISServerNotConnectedException", msg.str().c_str());
        }
        catch(const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << e.details.in() ;

            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISInvalidParameterException", msg.str().c_str());
        }
        catch(const TA_Base_Core::ObjectResolutionException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << e.what();

            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str());
        }
        catch(const CORBA::Exception& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str();

            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", msg.str().c_str());
        }
        catch(...)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": ";

            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", msg.str().c_str());
        }

        return bExecSucceed;
    }

    int ActiveStationTisStep::waitImpl()
    {
        // Wait for operator response
        return eWaitSucceed;
    }
}