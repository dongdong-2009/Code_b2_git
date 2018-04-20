/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #6 $
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

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <iomanip>



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
        showExceptionCatchDetails( prefixMessage, "Train Timeout" );\
    }\
    catch (const TransactionCancelledException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactionCancelledException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "Operation Cancelled" );\
    }\
    catch (const MpuChangeoverException& ex)\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "MpuChangeoverException", ex.what() );\
        showExceptionCatchDetails( prefixMessage, "MPU changeover" );\
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
                showExceptionCatchDetails( prefixMessage, "PEC In Use" );\
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

    PECDialog* PECCallEntry::s_dialog = NULL;


    PECCallEntry::PECCallEntry( unsigned long id,
                                const TA_IRS_Bus::PecTypes::PecUpdate& point,
                                PECCallFactory& factory,
                                TrainPecCorbaProxyPtr pecCorbaProxy )
        : AbstractCallEntry( id, "", "" ),
          m_pecDetails( point ),
          m_pecCallFactory( factory ),
          m_trainPecCorbaProxy( pecCorbaProxy ),
          m_pecCallInitialiser( NULL )
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
                ostr << "Inner Track";
                break;

            case TrainInformationTypes::AtsTrackOuter:
                ostr << "Outer Track";
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

        // Based on the PEC information, derive a
        // string for its status based on the state

        switch ( m_pecDetails.pec.source )
        {
            case PecTypes::PecSourceDriver:
                {
                    FUNCTION_EXIT;
                    return STATUS_DRIVER;
                }
                break;

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
                    FUNCTION_EXIT;
                    return STATUS_IN_PROGRESS;
                }

                // sessionId doesnt match operatorSessionId : STATUS_ANSWERED
                FUNCTION_EXIT;
                return STATUS_ANSWERED;
            }

            break;

            case PecTypes::PecSourceUnassigned:
            {
                if ( false == m_pecDetails.sessionId.empty() )
                {
                    // if a session ID is set, another PEC on this train is in progress
                    FUNCTION_EXIT;
                    return STATUS_TRAIN_BUSY;
                }
                
                // sessionId is empty
                // no PEC in this train are answered by an operator

                // if there is another PEC on this train answered by the driver
                // the the train is busy
                if ( true == m_pecCallFactory.trainHasPecAnsweredByDriver( m_pecDetails.trainId ) )
                {
                    FUNCTION_EXIT;
                    return STATUS_DRIVER;
                }

                // if the PEC is not valid
                if ( false == m_pecDetails.pec.valid )
                {
                    FUNCTION_EXIT;
                    return STATUS_INVALID;
                }
                
                // nothing is using this PEC, it is new
                FUNCTION_EXIT;
                return STATUS_NEW;
            }

            break;

            default:
                break;
        }

        FUNCTION_EXIT;
        return STATUS_UNKNOWN;
    }


    std::string PECCallEntry::getLocationStr() const
    {
        FUNCTION_ENTRY( "getLocationStr" );

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

        // otherwise the location can not be
        // determined, return UNKNOWN_LOCATION_NAME
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

        ostr << "PEC " << m_pecCallFactory.getPecStringForPecId( m_pecDetails.pec.pec )  << " "
             << "Car " << static_cast<unsigned int> ( m_pecDetails.pec.car );

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
                ostr << "IT";
                break;

            case TrainInformationTypes::AtsTrackOuter:
                ostr << "OT";
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
            //s_dialog->setStatusCallLost();

            // Bohong++ for CL-20793
            TA_THREADGUARD( m_resetLock );
			
            if ( s_dialog->isShown() )
            {
                s_dialog->setStatusCallLost();
				
                s_dialog->showInformationMessage( "The audio connection has been cut and the PEC will be considered as reset" );
				
                s_dialog->hideDialog();
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
                s_dialog->showInformationMessage( "PEC has been reset." );

                // once the dialog is acknowledged, the PEC dialog must be closed
                s_dialog->hideDialog();
            }
        }

        FUNCTION_EXIT;
    }


    void PECCallEntry::processMpuChangeover()
    {
        FUNCTION_ENTRY( "processMpuChangeover" );

        if ( ( NULL != s_dialog ) &&
             ( true == s_dialog->isShown() ) &&
             ( s_dialog->getCallEntry()->getId() == getId() ) )
        {
            TA_THREADGUARD( m_resetLock );

            if ( s_dialog->isShown() )
            {
                s_dialog->setStatusMpuChangeover();

                s_dialog->showInformationMessage( "PEC has been reset due to MPU failure." );

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
                                            m_pecDetails.pec.pec, sessionId );
        }
        // use the PEC_CATCH_HANDLER to catch and show any errors.
        PEC_CATCH_HANDLER( RESET_ERROR_PREFIX )

        // close the dialog (the call will automatically be removed by an agent update)
        s_dialog->hideDialog();

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
            m_trainPecCorbaProxy->resetAllPec( m_pecDetails.trainId, sessionId );
        }
        // use the PEC_CATCH_HANDLER to catch and show any errors.
        PEC_CATCH_HANDLER( RESET_ALL_ERROR_PREFIX )

        // close the dialog (the call willautomatically be removed by an agent update)
        s_dialog->hideDialog();

        FUNCTION_EXIT;
    }


    void PECCallEntry::onActivate()
    {
        FUNCTION_ENTRY( "onActivate" );

        bool doAnswer = true;

        // Check the PEC is not already answered by this operator
        std::string operatorSessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        //if the source is operator, and the session ID is this operator,
        if ( ( PecTypes::PecSourceOperator == m_pecDetails.pec.source ) &&
             ( operatorSessionId == m_pecDetails.sessionId ) )
        {
            // then the dialog will simply be shown,
            // but the answer command will not be sent
            doAnswer = false;
        }

        // Check the PEC can be answered
        // - ensure it has source as PecSourceUnassigned
        // - ensure that the session ID is empty
        // (this indicates the train is busy with another PEC in progress)
        // If these conditions are not met, show an error and return
        else if ( ( PecTypes::PecSourceUnassigned != m_pecDetails.pec.source ) ||
                  ( true == m_pecCallFactory.trainHasPecAnsweredByDriver( m_pecDetails.trainId ) ) )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_650002 );

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
            userMsg.showMsgBox( IDS_UE_650004 );

            FUNCTION_EXIT;
            return;
        }

        // otherwise create the dialog
        if ( NULL == s_dialog )
        {
            s_dialog = new PECDialog( this );

        }
        else if ( true == s_dialog->isShown() )
        {
            s_dialog->SetForegroundWindow();
            
            FUNCTION_EXIT;
            return;
        }
        else
        {
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
        s_dialog->disableDialog();

        bool activateCctvFirst = m_pecCallFactory.isRadioInFallback();
			if ( true == activateCctvFirst )
			{
				s_dialog->activateCCTV();
			}
        try
        {
			

            CWaitCursor wait;

            // set the dialog status to answering in progress
            s_dialog->setStatusAnswerInProgress();

            std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
            // answer the PEC using the train corba proxy
            m_trainPecCorbaProxy->answerPec( m_pecDetails.trainId, m_pecDetails.pec.car, m_pecDetails.pec.pec, sessionId );
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Call to answer PEC was sucessful");
            // activate the cctv (if not done previously)
            if ( false == activateCctvFirst )
            {
                s_dialog->activateCCTV();
            }

            // set the status to answered
            s_dialog->setStatusPecAnswered();

            // update the PEC dialog
            s_dialog->updatePecDetails();

            // enable the dialog window
            s_dialog->enableDialog();

            FUNCTION_EXIT;
            return;
        }
        // catch any exceptions using the PEC_CATCH_HANDLER macro
        PEC_CATCH_HANDLER( ANSWER_ERROR_PREFIX )
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Hiding the dialog as there was problem answering PEC");
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

        // check the dialog exists, and show the error message using the dialog.
        TA_ASSERT( NULL != s_dialog, "Cannot display error message when dialog is NULL" );

        // if it doesnt exist, how can an error be generated, so ASSERT.
		s_dialog->displayErrorMessage(errorMessage);
		//s_dialog->showErrorMessage( errorMessage );

        FUNCTION_EXIT;
    }


    void PECCallEntry::activateCctv()
    {

        //activate the CCTV using the train PEC corba proxy
        //depending on whether the location is to be overridden, and whether a location was selected, call either activatePecCamera or activatePecCameraDelocalisedTrain
        //
        //catch any cctv conflict exceptions and prompt the user to deactivate a train
        //once the train is deactivated, try to activate the PEC camera again
        //
        //use the PEC_CATCH_HANDLER to catch and show any errors.
        //
        //If the train was successfully activated, add the camera numbers returned from the activatePecCamera call to the CCTV alarm stack (using the PEC call factory)

        FUNCTION_ENTRY( "activateCctv" );

        // Check if CCTV is available from the pec call factory
        if ( false == m_pecCallFactory.isCctvAvailable() )
        {
            // if it isnt, return.
            FUNCTION_EXIT;
            return;
        }

        // Get the train information from the pec call factory
        TrainInformationTypes::TrainDetails trainInfo = m_pecCallFactory.getDetailsForTrain( m_pecDetails.trainId );

        bool delocalised = false;
        unsigned long overrideLocation = 0;
        unsigned long cctvLocation = trainInfo.currentLocation;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Current cctv location: %u",cctvLocation);
		// if the ats information is invalid, or the localisation is invalid
        if ( ( false == trainInfo.atsValid ) ||
             ( false == trainInfo.isLocalisationValid ) )
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Displaying the location selection dialog");
			// create a LocationSelectionDialog, and call DoModal.
            LocationSelectionDialog locationSelectionDialog( trainInfo.currentLocation );

            // if the return value is not IDOK, return
            if ( IDOK != locationSelectionDialog.DoModal() )
            {
                FUNCTION_EXIT;
                return;
            }

            // if the return value is IDOK, get the selected location from the dialog
            delocalised = true;

            overrideLocation = locationSelectionDialog.getSelectedLocation();
            cctvLocation = overrideLocation;

            // no location was selected...
            if ( 0 == overrideLocation )
            {
                FUNCTION_EXIT;
                return;
            }
        }

        try
        {
            CctvTypes::TrainCameraSequence trainAgentCameraList;

            std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            try
            {
                // activate the train, optionally override the location if the train is delocalised
                if ( true == delocalised )
                {
                    trainAgentCameraList = m_trainPecCorbaProxy->activatePecCameraDelocalisedTrain( m_pecDetails.trainId, m_pecDetails.pec.car, m_pecDetails.pec.pec, overrideLocation, sessionId );
                }
                else
                {
                    trainAgentCameraList = m_trainPecCorbaProxy->activatePecCamera( m_pecDetails.trainId, m_pecDetails.pec.car, m_pecDetails.pec.pec, sessionId );
                }

            }
            catch ( const CctvConflictException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TrainCctvConflictException", ex.what() );


                // This means the train can't be activated due to conflict management
                // need to give the operator the option to fix it


                std::string message = ex.what();
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << message;

                if ( IDOK == userMsg.showMsgBox( IDS_UW_650001 ) )
                {
                    bool success = false;

                    try
                    {
                        TrainCctvCorbaProxyPtr trainCctvCorbaProxy( TrainAgentProxyFactory::getInstance().createTrainCctvCorbaProxy() );
                        trainCctvCorbaProxy->setCarrierOff( ex.getConflictingTrain(), sessionId );

                        success = true;
                    }
                    PEC_CATCH_HANDLER( DEACTIVATE_OWNING_TRAIN_ERROR_PREFIX )

                    // if they deactivated the other train then they
                    // can retry activating this one
                    if ( true == success )
                    {
                        if ( true == delocalised )
                        {
                            trainAgentCameraList = m_trainPecCorbaProxy->activatePecCameraDelocalisedTrain( m_pecDetails.trainId, m_pecDetails.pec.car, m_pecDetails.pec.pec, overrideLocation, sessionId );
                        }
                        else
                        {
                            trainAgentCameraList = m_trainPecCorbaProxy->activatePecCamera( m_pecDetails.trainId, m_pecDetails.pec.car, m_pecDetails.pec.pec, sessionId );
                        }
                    }
                }
            }

            // if activating cctv did succeed, then the switch must be made in cctv
            if ( trainAgentCameraList.size() > 0 )
            {
                m_pecCallFactory.addImageToAlarmStack( m_pecDetails.trainId, cctvLocation, trainAgentCameraList, sessionId );
            }
        }
        PEC_CATCH_HANDLER( CCTV_ERROR_PREFIX )
		FUNCTION_EXIT;
    }

}
