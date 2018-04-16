/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $2008-01-09$
  * Last modified by:  $LiangHua Yuan$
  *
  */


#include "bus/trains/PECCallBannerLibrary/src/PECCallEntry.h"
#include "bus/trains/PECCallBannerLibrary/src/PECCallFactory.h"
#include "bus/trains/PECCallBannerLibrary/src/PECDialog.h"
#include "bus/trains/PECCallBannerLibrary/src/PECCallInitialiser.h"
#include "bus/trains/PECCallBannerLibrary/src/LocationSelectionDialog.h"
//#include "bus/trains/PECCallBannerLibrary/src/PECCallCCTV.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/MpuChangeoverException.h"
#include "bus/trains/TrainCommonLibrary/src/BadCommandException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainCommonLibrary/src/CctvException.h"
#include "bus/trains/TrainCommonLibrary/src/CctvConflictException.h"
#include "bus/trains/TrainCommonLibrary/src/PecException.h"

#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCorbaProxy/src/TrainCctvCorbaProxy.h"

#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/applauncher.h"

#include <iomanip>
#include <windows.h>
#include <string.h>


#define STRING_EQUAL		 0

/**
 * This is a macro to catch all common PEC exceptions. This is to simplify the
 * code, and have one consisted method of reporting errors.
 *
 * @param string    The operation that was being performed.
 */
#define PEC_CATCH_HANDLER(prefixMessage)\
    catch (const AgentCommunicationException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "AgentCommunicationException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Error communicating with Train Agent" );\
    }\
    catch (const TrainAccessDeniedException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TrainAccessDeniedException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Access Denied" );\
    }\
    catch (const InvalidTrainException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "InvalidTrainException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Invalid Train" );\
    }\
    catch (const InvalidParameterException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "InvalidParameterException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Invalid Parameter" );\
    }\
    catch (const TrainTransmissionFailureException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TransmissionFailureException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Error communicating with the Train" );\
    }\
    catch (const TrainTimeoutException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TrainTimeoutException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "No Response from Train" );\
    }\
    catch (const TransactionCancelledException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactionCancelledException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Operation Cancelled" );\
    }\
    catch (const MpuChangeoverException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "MpuChangeoverException", ex.what() );\
        processMpuChangeover ();\
    }\
    catch (const BadCommandException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "BadCommandException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Bad Command response from train" );\
    }\
    catch (const TrainRadioException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TrainRadioException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Error initiating radio call" );\
    }\
    catch (const PecException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "PecException", ex.what() );\
        \
        ProtocolPecTypes::EPecCommandResult errorType = ex.getErrorType();\
        switch ( errorType )\
        {\
            case ProtocolPecTypes::PecInUse:\
                showExceptionCatchDetails( prefixMessage, "PEC is in use" );\
                break;\
\
            case ProtocolPecTypes::PecFailed:\
                showExceptionCatchDetails( prefixMessage, "PEC Failed" );\
                break;\
\
            case ProtocolPecTypes::PecProceeding:\
            default:\
                TA_ASSERT( false, "This is not a valid failure reason" );\
        }\
    }\
    catch (const CctvException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TrainCctvException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Error activating CCTV" );\
    }\
    catch (const CctvConflictException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "CctvConflictException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Error activating CCTV" );\
    }\
    catch (...)\
    {\
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "??");\
        showExceptionCatchDetails(prefixMessage, "Unknown Error");\
    }


namespace TA_IRS_Bus
{
    const std::string PECCallEntry::CALL_ENTRY_TYPE = "PEC";
    const std::string PECCallEntry::RESET_MENU_ITEM = "Reset";
    const std::string PECCallEntry::RESET_ALL_MENU_ITEM = "Reset All";
    const std::string PECCallEntry::CLEAR_MENU_ITEM = "Clear";
    const std::string PECCallEntry::CLEAR_ALL_MENU_ITEM = "Clear All";
    const std::string PECCallEntry::STATUS_DRIVER = "Driver";
    const std::string PECCallEntry::STATUS_TRAIN_BUSY = "Train Busy";
    const std::string PECCallEntry::STATUS_IN_PROGRESS = "In Progress";
    const std::string PECCallEntry::STATUS_ANSWERED = "Answered";
    const std::string PECCallEntry::STATUS_NEW = "New";
    const std::string PECCallEntry::STATUS_INVALID = "Invalid";
    const std::string PECCallEntry::STATUS_UNKNOWN = "Unknown";
    const std::string PECCallEntry::UNKNOWN_LOCATION_NAME = "???";
    const std::string PECCallEntry::ANSWERED_BY_DRIVER_MESSAGE = "The call has been answered by a driver";
    const std::string PECCallEntry::ANSWER_ERROR_PREFIX = "Failed to accept PEC call";
    const std::string PECCallEntry::CCTV_ERROR_PREFIX = "Failed to activate CCTV";
    const std::string PECCallEntry::DEACTIVATE_OWNING_TRAIN_ERROR_PREFIX = "Failed to deactivate conflicting train CCTV";
    const std::string PECCallEntry::RESET_ERROR_PREFIX = "Failed to reset PEC call";
    const std::string PECCallEntry::RESET_ALL_ERROR_PREFIX = "Failed to reset all PEC calls";
	const std::string PECCallEntry::TRAINLOC_INNER_PREFIX = "BB";
	const std::string PECCallEntry::TRAINLOC_OUTER_PREFIX = "XB";


    PECDialog* PECCallEntry::s_dialog = NULL;


    PECCallEntry::PECCallEntry( unsigned long id,
                                const TA_IRS_Bus::PecTypes::PecUpdate& point,
                                PECCallFactory& factory,
                                TrainPecCorbaProxyPtr pecCorbaProxy )
        : AbstractCallEntry( id, "", "" ),
          m_pecDetails( point ),
          m_pecCallFactory( factory ),
          m_trainPecCorbaProxy( pecCorbaProxy ),
          m_pecCallInitialiser( NULL ),
		  m_bIsActive(false),
		  m_bIsCallTerminated(false),
		  m_bOnGoingChangeOverProc(false)
		  //m_pecCallCCTV(NULL)
    {
        FUNCTION_ENTRY( "PECCallEntry( id,point,factory,pecCorbaProxy )" );

        // Creates a new call entry.

        // Set the source and information in AbstractCallEntry
        AbstractCallEntry::setSource( getPecSourceString() );
        AbstractCallEntry::setInformation( getPecInformationString() );

        FUNCTION_EXIT;
    }


    PECCallEntry::~PECCallEntry()
    {
        FUNCTION_ENTRY( "~PECCallEntry" );

        // acquire the reset lock
        // this prevents the call entry being deleted when an operator
        // has the pec dialog open with this PEC as the subject
        TA_THREADGUARD( m_resetLock );

        // if the PEC initialiser thread exists
        if ( m_pecCallInitialiser != NULL )
        {
            // stop the thread (and wait for it to join)
            m_pecCallInitialiser->terminateAndWait();

            // delete it
            delete m_pecCallInitialiser;
            m_pecCallInitialiser = NULL;
        }

// 		if ( m_pecCallCCTV != NULL )
// 		{
// 			// stop the thread (and wait for it to join)
// 			m_pecCallCCTV->terminateAndWait();
// 
// 			// delete it
// 			delete m_pecCallCCTV;
// 			m_pecCallCCTV = NULL;
// 		}

        FUNCTION_EXIT;
    }


    std::string PECCallEntry::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );

        FUNCTION_EXIT;
        return CALL_ENTRY_TYPE;
    }


    std::string PECCallEntry::getType() const
    {
        FUNCTION_ENTRY( "getType" );

        FUNCTION_EXIT;
        return CALL_ENTRY_TYPE;
    }


    bool PECCallEntry::isEmergency() const
    {
		return true;
    }


    std::string PECCallEntry::getPecSourceString() const
    {
        FUNCTION_ENTRY( "getPecSourceString" );

        // Format a string like "PEC 1A Car 3 Train PV 07"
        std::ostringstream ostr;

        ostr << getPECDetailString() << " "
        << "Train PV " << std::setfill( '0' ) << std::setw( 2 )
        << static_cast<unsigned int> ( m_pecDetails.trainId );

        FUNCTION_EXIT;
        return ostr.str();
    }


    std::string PECCallEntry::getPecInformationString() const
    {
        FUNCTION_ENTRY( "getPecInformationString" );

        // Format a string like "Service 123 Inner Track"

        // obtained train details from the PEC factory
        TA_IRS_Bus::TrainInformationTypes::TrainDetails trainInfo =
            m_pecCallFactory.getDetailsForTrain( m_pecDetails.trainId );

        std::ostringstream ostr;

        ostr << "Service " << trainInfo.serviceNumber << " ";

        switch ( trainInfo.track )
        {

            case TrainInformationTypes::AtsTrackInner:
                ostr << TRAINLOC_INNER_PREFIX;
                break;

            case TrainInformationTypes::AtsTrackOuter:
                ostr << TRAINLOC_OUTER_PREFIX;
                break;

            case TrainInformationTypes::AtsTrackUnknown:
                ostr << "Unknown";
                break;

            default:
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Invalid enumeration value for track %d",
                             trainInfo.track );
                ostr << "Unknown";
                break;
        }

        FUNCTION_EXIT;

        return ostr.str();
    }


    std::string PECCallEntry::getStatusStr() const
    {
        FUNCTION_ENTRY( "getStatusStr" );

        std::string operatorSessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"getStatusStr");
        // Based on the PEC information, derive a
        // string for its status based on the state

        switch ( m_pecDetails.pec.source )
        {
            case PecTypes::PecSourceDriver:
                {
					std::string strStatus;
					//If the PEC is invalid due to TB rights have been degraded or delegated to stations
					//then the Driver state must be change to Invalid state.
					if (!m_pecDetails.pec.valid)
					{
						strStatus = STATUS_INVALID;
					}
					else
					{
						strStatus = STATUS_DRIVER;
					}
					FUNCTION_EXIT;
					return strStatus;
                }

            case PecTypes::PecSourceOperator:
            {
                // sessionId is empty : STATUS_TRAIN_BUSY
                if ( true == m_pecDetails.sessionId.empty() )
                {
                    FUNCTION_EXIT;
                    return STATUS_TRAIN_BUSY;
                }

                // sessionId matches operatorSessionId : STATUS_IN_PROGRESS
                if ( m_pecDetails.sessionId == operatorSessionId )
                {
                    
					std::string strStatus;
					if (!m_pecDetails.pec.valid)
					{
						strStatus = STATUS_INVALID;
					}
					else
					{
						strStatus = STATUS_IN_PROGRESS;
					}
					FUNCTION_EXIT;
                    return strStatus;
                }

                // sessionId doesnt match operatorSessionId : STATUS_ANSWERED
                FUNCTION_EXIT;
                return STATUS_ANSWERED;
            }

            case PecTypes::PecSourceUnassigned:
            {
                if ( false == m_pecDetails.sessionId.empty() )
                {
                    // if a session ID is set, another PEC on this train is in progress
                    FUNCTION_EXIT;
                    //return STATUS_TRAIN_BUSY;

					LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
								"Session ID is empty");

					return STATUS_NEW;
					
                }
                
                // sessionId is empty
                // no PEC in this train are answered by an operator

                // if there is another PEC on this train answered by the driver
                // the the train is busy
				// DTL-667, show status NEW for other PEC(s)
                if ( true == m_pecCallFactory.trainHasPecAnsweredByDriver( m_pecDetails.trainId ) )
                {
                    FUNCTION_EXIT;

					LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
								 "Train has a PEC answered by driver");

                    return STATUS_NEW;
                }

                // if the PEC is not valid
                if ( false == m_pecDetails.pec.valid )
                {
                    FUNCTION_EXIT;
                    return STATUS_INVALID;
                }
                
                // nothing is using this PEC, it is new
                FUNCTION_EXIT;
				
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
							"nothing is using this PEC, it is new");
				
                return STATUS_NEW;
            }

            default:
                break;
        }

        FUNCTION_EXIT;
        return STATUS_UNKNOWN;
    }


    std::string PECCallEntry::getLocationStr() const
    {
        FUNCTION_ENTRY( "getLocationStr" );

		try 
		{
			// get the TrainDetails from the PEC factory
			TA_IRS_Bus::TrainInformationTypes::TrainDetails trainInfo = m_pecCallFactory.getDetailsForTrain( m_pecDetails.trainId );

			// if the ats is valid and the localisation is valid

			if ( ( true == trainInfo.atsValid ) &&
				 ( true == trainInfo.isLocalisationValid ) )
			{
				// convert the location to its string name format and return it
				return LocationCache::getInstance().getLocationByKey( trainInfo.currentLocation );
			}

			// otherwise, ats or localisation is not valid
			// if the location key is not 0
			else if ( 0 != trainInfo.currentLocation )
			{
				// convert the location to its string name format,
				// and add ( ) around the name, then return it
				std::ostringstream temp;
				temp << '('
					 << LocationCache::getInstance().getLocationByKey( trainInfo.currentLocation )
					 << ')';

				FUNCTION_EXIT;
				return temp.str();
			}

		}
		catch(...) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "unknown location information" );
		}
        FUNCTION_EXIT;
        return UNKNOWN_LOCATION_NAME;
    }


    time_t PECCallEntry::getCreateTime() const
    {
        FUNCTION_ENTRY( "getCreateTime" );

        // get the time from the pec details

        FUNCTION_EXIT;
        return m_pecDetails.pec.creationTime;
    }


    std::string PECCallEntry::getPECDetailString() const
    {
        FUNCTION_ENTRY( "getPECDetailString" );

        // Format a string like "PEC 1A Car 3"
        // use m_pecCallFactory.getPecIdAsString
        // to translate the correct PEC number

        std::ostringstream ostr;

        ostr << "PEC" << m_pecCallFactory.getPecStringForPecId( m_pecDetails.pec.pec )  << " in "
             //<< "Car " << static_cast<unsigned int> ( m_pecDetails.pec.car );
			 << m_pecCallFactory.getPecStringForPecCarNum( m_pecDetails.pec.car ) 
			 << m_pecCallFactory.getPecStringForPecTrainSource( m_pecDetails.pec.pecTrainSource );

        FUNCTION_EXIT;
        return ostr.str();
    }


    std::string PECCallEntry::getLocationDetailString() const
    {
        FUNCTION_ENTRY( "getLocationDetailString" );

        // Format a string like "Station Name /bound" eg "MSM / IT" or "MSM / OT"
        // This is present in ATS information, which can be obtained from the
        // PEC factory, getLocationStr can be used to get the station name

        TA_IRS_Bus::TrainInformationTypes::TrainDetails trainInfo = m_pecCallFactory.getDetailsForTrain( m_pecDetails.trainId );

        std::ostringstream ostr;

        ostr << getLocationStr() << " / ";

        switch ( trainInfo.track )
        {

            case TrainInformationTypes::AtsTrackInner:
                ostr << TRAINLOC_INNER_PREFIX;
                break;

            case TrainInformationTypes::AtsTrackOuter:
                ostr << TRAINLOC_OUTER_PREFIX;
                break;

            case TrainInformationTypes::AtsTrackUnknown:
                ostr << "Unknown";
                break;

            default:
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Invalid enumeration value for track %d",
                             trainInfo.track );
                ostr << "Unknown";
                break;
        }

        FUNCTION_EXIT;
        return ostr.str();
    }


    std::string PECCallEntry::getTrainDetailString() const
    {
        FUNCTION_ENTRY( "getTrainDetailString" );

        // Simply return the PV/Service string from
        // the ATS information available from the PEC factory
        TA_IRS_Bus::TrainInformationTypes::TrainDetails trainInfo =
            m_pecCallFactory.getDetailsForTrain( m_pecDetails.trainId );

        FUNCTION_EXIT;
        return trainInfo.serviceNumber;
    }


    bool PECCallEntry::isCctvAvailable()
    {
        FUNCTION_ENTRY( "isCctvAvailable" );
        FUNCTION_EXIT;
        return m_pecCallFactory.isCctvAvailable();
    }


    TA_IRS_Bus::CommonTypes::TrainIdType PECCallEntry::getTrainId()
    {
        FUNCTION_ENTRY( "getTrainId" );

        // get the train ID from the PEC details

        FUNCTION_EXIT;
        return m_pecDetails.trainId;
    }


    std::string PECCallEntry::getSessionId()
    {
        FUNCTION_ENTRY( "getSessionId" );

        // get the session ID from the PEC details

        FUNCTION_EXIT;
        return m_pecDetails.sessionId;
    }


    PecTypes::EPecSource PECCallEntry::getPecSource()
    {
        FUNCTION_ENTRY( "getPecSource" );

        // get the source from the PEC details

        FUNCTION_EXIT;
        return m_pecDetails.pec.source;
    }


    std::vector<std::string> PECCallEntry::getContextMenuItems()
    {
        FUNCTION_ENTRY( "getContextMenuItems" );

        std::vector<std::string> menuList;

        // If the PEC source is unassigned

        if ( PecTypes::PecSourceUnassigned == m_pecDetails.pec.source )
        {
            // if it is valid, insert RESET_MENU_ITEM and RESET_ALL_MENU_ITEM
            if ( true == m_pecDetails.pec.valid )
            {
                menuList.push_back( RESET_MENU_ITEM );
                menuList.push_back( RESET_ALL_MENU_ITEM );
            }

            // if it is not valid, insert CLEAR_MENU_ITEM and CLEAR_ALL_MENU_ITEM
            else
            {
                menuList.push_back( CLEAR_MENU_ITEM );
                menuList.push_back( CLEAR_ALL_MENU_ITEM );
            }
        }

        FUNCTION_EXIT;
        return menuList;
    }


    void PECCallEntry::onContextMenuCommand( std::string command )
    {
        FUNCTION_ENTRY( "onContextMenuCommand" );

        //If the PECDialog does not yet exist,

        if ( NULL == s_dialog )
        {
            // create it with the details of this call entry.
            s_dialog = new PECDialog( this );
        }

        if ( ( RESET_MENU_ITEM == command ) ||
             ( CLEAR_MENU_ITEM == command ) )
        {
            dialogResetButtonPressed();
        }
        else if ( ( RESET_ALL_MENU_ITEM == command ) ||
                  ( CLEAR_ALL_MENU_ITEM == command ) )
        {
            dialogResetAllButtonPressed();
        }

        FUNCTION_EXIT;
    }


    void PECCallEntry::trainDetailsChanged()
    {
        FUNCTION_ENTRY( "trainDetailsChanged" );

        // Update the banner information string,
        AbstractCallEntry::setInformation( getPecInformationString() );

        // if it is displaying this PEC, and is shown

        if ( ( NULL != s_dialog ) &&
             ( true == s_dialog->isShown() ) &&
             ( s_dialog->getCallEntry()->getId() == getId() ) )
        {
            // update the PEC dialog
            s_dialog->updatePecDetails();
        }

        FUNCTION_EXIT;
    }


    void PECCallEntry::processUpdate( const TA_IRS_Bus::PecTypes::PecUpdate& update )
    {
        FUNCTION_ENTRY( "processUpdate" );

        // If the source is PecSourceDriver, and it wasnt previously

        bool thisPecHasBeenAnsweredByDriver = ( PecTypes::PecSourceDriver == update.pec.source ) &&
                                              ( PecTypes::PecSourceDriver != m_pecDetails.pec.source );

        // or another pec has been answered by the driver
        bool otherPecAnsweredByDriver = ( PecTypes::PecSourceUnassigned == update.pec.source ) &&
                                        ( PecTypes::PecSourceUnassigned != m_pecDetails.pec.source );

        if ( ( true == thisPecHasBeenAnsweredByDriver ) ||
             ( true == otherPecAnsweredByDriver ) )
        {
            // check if a dialog for this PEC is visible
            if ( ( NULL != s_dialog ) &&
                 ( true == s_dialog->isShown() ) &&
                 ( s_dialog->getCallEntry()->getId() == getId() ) )
            {
                // if it is inform the user this PEC has been ansered by a driver
                s_dialog->showInformationMessage( ANSWERED_BY_DRIVER_MESSAGE );

                // hide the dialog
                s_dialog->hideDialog();
            }
        }

        // set the new pec details
        m_pecDetails = update;

        FUNCTION_EXIT;
    }


    void PECCallEntry::processCallLost()
    {
        FUNCTION_ENTRY( "processCallLost" );

        // If the PEC dialog is displayed for this PEC call

        if ( ( NULL != s_dialog ) &&
             ( true == s_dialog->isShown() ) &&
             ( s_dialog->getCallEntry()->getId() == getId() ) )
        {
            // update its status to indicate the call was lost
            // s_dialog->setStatusCallLost();

            // Bohong++ for CL-20793
            TA_THREADGUARD( m_resetLock );
			
            if ( s_dialog->isShown() )
            {
                s_dialog->setStatusCallLost();
				
                s_dialog->showInformationMessage( "The audio connection has been cut and the PEC will be considered as reset" );
				
                s_dialog->hideDialog();
				m_bIsActive = false;
            }
			// ++Bohong
        }

        FUNCTION_EXIT;
    } 


    void PECCallEntry::processReset()
    {
        FUNCTION_ENTRY( "processReset" );

        // inform the user that the call has been reset and close the dialog.
        // Otherwise just ignore the notification (because the user has already closed the dialog)
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Log , processReset");
        // If the dialog has previously been created. and the dialog is displaying the current call entry
        if ( ( NULL != s_dialog ) &&
             ( true == s_dialog->isShown() ) &&
             ( s_dialog->getCallEntry()->getId() == getId() ) )
        {
            // acquire the reset lock, as a reset may be in progress
			TA_THREADGUARD( m_resetLock );
			
            // after the reset lock is acquired, recheck the visibility of the dialog,
            if ( s_dialog->isShown() )
            {
                // if it is still visible
                // update the dialog status to reset
                s_dialog->setStatusReset();

                // a modal dialog must be shown informing the user of the PEC reset
                //s_dialog->showInformationMessage( "PEC has been reset." );

                // once the dialog is acknowledged, the PEC dialog must be closed
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Log , processReset : Closing the dialog");
                s_dialog->hideDialog();

				m_bIsActive = false;
            }
        }

        FUNCTION_EXIT;
    }


    void PECCallEntry::processMpuChangeover()
    {
        FUNCTION_ENTRY( "processMpuChangeover" );
		
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"MPU CHange Over Entry Status : %s",this->getStatusStr().c_str());


        if ( ( NULL != s_dialog ) &&
             ( true == s_dialog->isShown() ) &&
             ( s_dialog->getCallEntry()->getId() == getId()) &&
			   m_bOnGoingChangeOverProc == false)
        {
            TA_THREADGUARD( m_resetLock );

            if ( s_dialog->isShown() )
            {
                m_bOnGoingChangeOverProc = true;
				s_dialog->setStatusMpuChangeover();
	
                ::PostMessage(s_dialog->GetSafeHwnd(), PEC_CHANGE_OVER_FAIL_MSG,WPARAM(0),LPARAM(0));

                s_dialog->hideDialog();
            }
        }

        FUNCTION_EXIT;
    }


    void PECCallEntry::dialogCctvButtonPressed()
    {
        FUNCTION_ENTRY( "dialogCctvButtonPressed" );

        // Activate the CCTV for this PEC using the utility method
        activateCctv();
		::PostMessage(s_dialog->GetSafeHwnd(), PEC_UPDATE_THREAD_STATUS,WPARAM(0),LPARAM(0));

        FUNCTION_EXIT;
    }


    void PECCallEntry::dialogResetButtonPressed()
    {
        FUNCTION_ENTRY( "dialogResetButtonPressed" );

        // acquire the reseet lock to prevent this object being deleted before this method completes
        TA_THREADGUARD( m_resetLock );

        try
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
            
            // reset the PEC using the train pec corba proxy
			m_trainPecCorbaProxy->resetPec( m_pecDetails.trainId, m_pecDetails.pec.car,
                                            m_pecDetails.pec.pec, sessionId, m_pecDetails.pec.pecTrainSource,
											m_bIsCallTerminated);

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"dialogResetButtonPressed: resetPec, trainId=%d, car=%d, pec=%d, sessionId=%s, pecTrainSource=%d, callTerminated=%d",
						 static_cast<int> ( m_pecDetails.trainId ),
						 static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber> ( m_pecDetails.pec.car ),
						 static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecNumber> ( m_pecDetails.pec.pec ),
						 sessionId.c_str(),
						 static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource> (m_pecDetails.pec.pecTrainSource ),
						 m_bIsCallTerminated);

		}
        // use the PEC_CATCH_HANDLER to catch and show any errors.
        PEC_CATCH_HANDLER( RESET_ERROR_PREFIX )

			::PostMessage(s_dialog->GetSafeHwnd(), PEC_UPDATE_THREAD_STATUS ,WPARAM(0),LPARAM(1));

        // close the dialog (the call will automatically be removed by an agent update)
		//s_dialog->hideDialog();
        

        FUNCTION_EXIT;
    }


    void PECCallEntry::dialogResetAllButtonPressed()
    {
        FUNCTION_ENTRY( "dialogResetAllButtonPressed" );

        // acquire the reset lock to prevent this object being deleted before this method completes
        TA_THREADGUARD( m_resetLock );

        try
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
            
          
			// reset all PEC using the train pec corba proxy
			m_trainPecCorbaProxy->resetAllPec( m_pecDetails.trainId, sessionId, m_pecDetails.pec.pecTrainSource,m_bIsCallTerminated );

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"dialogResetAllButtonPressed: resetAllPec, trainId=%d, sessionId=%s, pecTrainSource=%d",
						static_cast<int> ( m_pecDetails.trainId ),
						sessionId.c_str(),
						static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource> (m_pecDetails.pec.pecTrainSource ) );
	
        }
        // use the PEC_CATCH_HANDLER to catch and show any errors.
        PEC_CATCH_HANDLER( RESET_ALL_ERROR_PREFIX )
		
			::PostMessage(s_dialog->GetSafeHwnd(),PEC_UPDATE_THREAD_STATUS, WPARAM(0),LPARAM(1));

		// close the dialog (the call willautomatically be removed by an agent update)
		//s_dialog->hideDialog();
		
        FUNCTION_EXIT;
    }

	void PECCallEntry::dialogHoldButtonPressed()
    {
        FUNCTION_ENTRY( "dialogHoldButtonPressed" );

        // acquire the reseet lock to prevent this object being deleted before this method completes
        TA_THREADGUARD( m_resetLock );

        try
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
            
            // reset the PEC using the train pec corba proxy
            m_trainPecCorbaProxy->holdPec( m_pecDetails.trainId, m_pecDetails.pec.car,
                                            m_pecDetails.pec.pec, sessionId, m_pecDetails.pec.pecTrainSource );

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"dialogHoldButtonPressed: holdPec, trainId=%d, car=%d, pec=%d, sessionId=%s, pecTrainSource=%d",
				static_cast<int> ( m_pecDetails.trainId ),
				static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber> ( m_pecDetails.pec.car ),
				static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecNumber> ( m_pecDetails.pec.pec ),
				sessionId.c_str(),
				static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource> (m_pecDetails.pec.pecTrainSource ) );
        }
        // use the PEC_CATCH_HANDLER to catch and show any errors.
        PEC_CATCH_HANDLER( RESET_ERROR_PREFIX )

			::PostMessage(s_dialog->GetSafeHwnd(), PEC_UPDATE_THREAD_STATUS,WPARAM(0),LPARAM(0));
        // close the dialog (the call will automatically be removed by an agent update)
        //s_dialog->hideDialog();

        FUNCTION_EXIT;
    }


    void PECCallEntry::onActivate()
    {
        FUNCTION_ENTRY( "onActivate" );

        bool doAnswer = true;

        // Check the PEC is not already answered by this operator
        std::string operatorSessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        // first create the dialog
		bool wasNull = false;
        if ( NULL == s_dialog )
        {
            s_dialog = new PECDialog( this );
			wasNull = true;
        }

        //if the source is operator, and the session ID is this operator,
        if ( ( PecTypes::PecSourceOperator == m_pecDetails.pec.source ) &&
             ( operatorSessionId == m_pecDetails.sessionId ) )
        {
			// DTL-346, peter.wong, print popup message without answering the PEC
			//TA_Base_Bus::TransActiveMessage userMsg;
			//userMsg.showMsgBox( IDS_UE_650002 );
			
			// use for commit
			s_dialog->showErrorMessage("PEC is in use and cannot be answered");
			
			// use for testing
			//::MessageBox (NULL,"PEC is in use and cannot be answered", "PEC Call Banner",MB_OK);

			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						 "Test Log : Selected a call that is already in progress, source is operator");

			FUNCTION_EXIT;
			return;
		}

		// Check the PEC can be answered
		// - ensure it has source as PecSourceUnassigned
		// - ensure that the session ID is empty
		// (this indicates the train is busy with another PEC in progress)
		// If these conditions are not met, show an error and return
		else if ( ( PecTypes::PecSourceUnassigned != m_pecDetails.pec.source ) ||
			( true == m_pecCallFactory.trainHasPecAnsweredByDriver( m_pecDetails.trainId ) ) )
		{
			//TA_Base_Bus::TransActiveMessage userMsg;
			//userMsg.showMsgBox( IDS_UE_650002 );
			
			// use for commit
			s_dialog->showErrorMessage("PEC is in use, and cannot be answered");
			
			//use for testing
			//::MessageBox (NULL,"PEC is in use and cannot be answered", "PEC Call Banner",MB_OK);
			
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"Test Log : Selected a call that is already in progress, source is driver");

			FUNCTION_EXIT;
			return;
		}

        // Now check if the operator is busy (has
        // another PEC answered on another banner)
        // Check from the PEC call factory
        if ( ( true == doAnswer ) &&
             ( true == m_pecCallFactory.isOperatorBusy() ) )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            // and if busy, show an error and return
            //userMsg.showMsgBox( IDS_UE_650004 );
			
			// use for commit
			s_dialog->showErrorMessage("PEC is in use, and cannot be answered");
			
			// use for testing 
			//::MessageBox (NULL,"PEC is in use and cannot be answered", "PEC Call Banner",MB_OK);
			
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"Test Log : Selected a call that is already in progress, source is unsassinged");

            FUNCTION_EXIT;
            return;
        }

		if (wasNull)
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"Test Log : OnActivate Func:Train ID : %d, The PEC Acceptance Window is just created",
						this->getTrainId ());
		}
		else if ( true == s_dialog->isShown() )
        {
            
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"Test Log : OnActivate Func:Train ID : %d, The PEC Acceptance is already shown",
						this->getTrainId ());
			
			::PostMessage(s_dialog->GetSafeHwnd(), PEC_SET_FORE_WINDOW, WPARAM(0),LPARAM(0));
            
            FUNCTION_EXIT;
            return;
        }
        else
        {
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						 "Test PEC Loading PEC acceptance, OnActivate Func:Train ID : %d, Window already created, setting CallEntry to dialog",
						 this->getTrainId ());
			
			s_dialog->setCallEntry( this );
        }

        // if the call is to be answered
        if ( true == doAnswer )
        {
            // start the PEC call intialiser
            if ( m_pecCallInitialiser != NULL )
            {
                m_pecCallInitialiser->terminateAndWait();
               
                delete m_pecCallInitialiser;
                m_pecCallInitialiser = NULL;
            }

			//FOR CCTV Activation
// 			if ( m_pecCallCCTV != NULL )
// 			{
// 				m_pecCallCCTV->terminateAndWait();
// 
// 				delete m_pecCallCCTV;
// 				m_pecCallCCTV = NULL;
// 			}
// 
// 			//FOR CCTV Activation
// 			//Activate the CCTV only if PEC Source = 0
// 			if (!isCoupledTrain())
// 			{
// 				m_pecCallCCTV = new PECCallCCTV( *this );
// 				m_pecCallCCTV->start();
// 			}

            m_pecCallInitialiser = new PECCallInitialiser( *this );
            m_pecCallInitialiser->start();
        }

        // show the dialog
        s_dialog->showDialog();
        FUNCTION_EXIT;
    }


    void PECCallEntry::threadedAnswer()
    {
        FUNCTION_ENTRY( "threadedAnswer" );

        // Disable the dialog window while answering is in progress
        //s_dialog->disableDialog();
		s_dialog->disableAllButtons();
		
		// do not activate cctv when coupled train is selected
		if (!isCoupledTrain())
		{
			s_dialog->setStatusActivatingCctv();
			activateCctv();
		}


        try
        {
			s_dialog->BeginWaitCursor();

            // set the dialog status to answering in progress
            s_dialog->setStatusAnswerInProgress();

            std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            // answer the PEC using the train corba proxy
            m_trainPecCorbaProxy->answerPec( m_pecDetails.trainId, m_pecDetails.pec.car, m_pecDetails.pec.pec, sessionId, m_pecDetails.pec.pecTrainSource );

            // set the status to answered
            s_dialog->setStatusPecAnswered();

            // update the PEC dialog
            s_dialog->updatePecDetails();

            // enable the dialog window
            //s_dialog->enableDialog();
			s_dialog->enableAllButtons();

			m_bIsActive = true;

            FUNCTION_EXIT;
			s_dialog->EndWaitCursor();
            return;
        }
        // catch any exceptions using the PEC_CATCH_HANDLER macro
        PEC_CATCH_HANDLER( "" )

        // if there were any errors, hide the dialog
        s_dialog->hideDialog();

        FUNCTION_EXIT;
    }


    void PECCallEntry::showExceptionCatchDetails( const std::string& prefixMessage, const std::string& errorDetails )
    {
        FUNCTION_ENTRY( "showExceptionCatchDetails" );

        // Join the two strings, and show an error message.
        // "prefix message : error details"

        std::ostringstream errorStr;

        errorStr << prefixMessage;

        if ( ( false == prefixMessage.empty() ) &&
             ( false == errorDetails.empty() ) )
        {
            errorStr << ": ";
        }

        errorStr << errorDetails;

        showErrorMessage( errorStr.str() );

        FUNCTION_EXIT;
    }


    void PECCallEntry::showErrorMessage( const std::string& errorMessage )
    {
        FUNCTION_ENTRY( "showErrorMessage" );
		
		if (!s_dialog)
		{
			return;
		}

        // check the dialog exists, and show the error message using the dialog.
        TA_ASSERT( NULL != s_dialog, "Cannot display error message when dialog is NULL" );

        // if it doesnt exist, how can an error be generated, so ASSERT.
        s_dialog->showErrorMessage( errorMessage );

        FUNCTION_EXIT;
    }


    void PECCallEntry::activateCctv()
    {

       
        FUNCTION_ENTRY( "activateCctv" );

        try
		{
			CctvTypes::TrainCameraSequence trainAgentCameraList;
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

			trainAgentCameraList = m_trainPecCorbaProxy->activatePecCamera( m_pecDetails.trainId, m_pecDetails.pec.car, m_pecDetails.pec.pec, sessionId );

		}

		PEC_CATCH_HANDLER( CCTV_ERROR_PREFIX )

        FUNCTION_EXIT;
    }


	void PECCallEntry::onForceReset ()
	{
		FUNCTION_ENTRY ("onForceReset");

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : onForceReset , performing Reset All");
		
		// acquire the reset lock to prevent this object being deleted before this method completes
		TA_THREADGUARD( m_resetLock );

		try
		{
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

			// reset all PEC using the train pec corba proxy
			m_trainPecCorbaProxy->resetAllPec( m_pecDetails.trainId, sessionId, m_pecDetails.pec.pecTrainSource );

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"onForceReset: resetAllPec, trainId=%d, sessionId=%s, pecTrainSource=%d",
						static_cast<int> ( m_pecDetails.trainId ),
						sessionId.c_str(),
						static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource> (m_pecDetails.pec.pecTrainSource ) );

			m_bIsActive = false;

		}

		// use the PEC_CATCH_HANDLER to catch and show any errors.
		PEC_CATCH_HANDLER( RESET_ALL_ERROR_PREFIX )

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : onForceReset , performed Reset All");
		
		FUNCTION_EXIT;
	}
	
	bool PECCallEntry::isCoupledTrain ()
	{
		FUNCTION_ENTRY ("isCoupledTrain");
		
		bool bIsCoupledTrain = true;
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Coupled Train : Train ID : %d , PEC Train Source : %d",m_pecDetails.trainId,m_pecDetails.pec.pecTrainSource);

		if (PecTypes::EPecTrainSource::LocalTrain == m_pecDetails.pec.pecTrainSource)
		{
			bIsCoupledTrain = false;
		}
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Coupled Train : Is Coupled Train : %d",bIsCoupledTrain);
		
		FUNCTION_EXIT;

		return bIsCoupledTrain;
	}

	bool PECCallEntry::isCallInvalid ()
	{
		FUNCTION_ENTRY ("isCallStatusValid");
		
		bool bIsInvalid = false;

 		if (STRING_EQUAL == getStatusStr().compare(STATUS_INVALID))
 		{
 			bIsInvalid = true;
 		}

		FUNCTION_EXIT;

		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					 "Call validity : %d",bIsInvalid);
		return bIsInvalid;
	}

	void PECCallEntry::onClearCall ()
	{
		FUNCTION_ENTRY ("onClearCall");

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : onClearCall , performing Reset on Invalide calls");

		// acquire the reset lock to prevent this object being deleted before this method completes
		TA_THREADGUARD( m_resetLock );

		try
		{
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

			// reset all PEC using the train pec corba proxy
			m_trainPecCorbaProxy->resetPec( m_pecDetails.trainId, 
											m_pecDetails.pec.car,
											m_pecDetails.pec.pec,
											sessionId,
											m_pecDetails.pec.pecTrainSource );

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"onClearCall: reset PEC Call, trainId=%d, sessionId=%s, pecTrainSource=%d",
				static_cast<int> ( m_pecDetails.trainId ),
				sessionId.c_str(),
				static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource> (m_pecDetails.pec.pecTrainSource ) );

		}

		// use the PEC_CATCH_HANDLER to catch and show any errors.
		PEC_CATCH_HANDLER( RESET_ERROR_PREFIX )

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : onForceReset , performed Reset All");

		FUNCTION_EXIT;
	}

	void PECCallEntry::onCallTermination ()
	{
		FUNCTION_ENTRY("onCallTermination");
		
		m_bIsCallTerminated = true;

		if (!this->s_dialog->getHoldResetStatus())
		{
			// show MessageBox
			::PostMessage(s_dialog->GetSafeHwnd(), PEC_CALL_TERMINATE_MSG,WPARAM(0),LPARAM(0));
			this->s_dialog->enableDialog();
		}
	
		FUNCTION_EXIT;
	}

	bool PECCallEntry::isCallActive ()
	{
		FUNCTION_ENTRY("isCallActive");
		
		FUNCTION_EXIT;
		return m_bIsActive;
		
	}

	void PECCallEntry::callCannotSetupExcepProc ()
	{
		// if there is an error error should be display on the status string
		s_dialog->setConnectionError();
		s_dialog->enableDialog();
	}

	void PECCallEntry::onCallSetupFailed ()
	{
		if (s_dialog->isShown())
		{
			s_dialog->setConnectionError();
		}
	}

	unsigned long PECCallEntry::getPrimaryTsi()
	{
		FUNCTION_ENTRY("getPrimaryTsi");
		FUNCTION_EXIT;
		return m_pecDetails.primaryTsi;
	}

	unsigned long PECCallEntry::getSecondaryTsi()
	{
		FUNCTION_ENTRY("getSecondaryTsi");
		FUNCTION_EXIT;
		return m_pecDetails.secondaryTsi;
	}

// 	void PECCallEntry::threadedCCTV()
// 	{
// 		FUNCTION_ENTRY("threadedCCTV");
// 
// 		
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
// 					"Activating CCTV ...");
// 
// 		activateCctv();
// 		
// 		FUNCTION_EXIT;
// 	}
}
