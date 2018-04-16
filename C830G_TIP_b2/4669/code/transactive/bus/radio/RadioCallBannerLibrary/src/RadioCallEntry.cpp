/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: PRIV_4669_TRAIN_ECP/CODE_4669/transactive/bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.cpp $
  * @author:  oliverk
  * @version: $Revision: #2 $
  *
  * Last modification: $Date: 2009/02/10 $
  * Last modified by:  $Author: builder $
  *
  */


#include "bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h"
#include "bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/generic_gui/src/AppLauncher.h"

#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    const std::string RadioCallEntry::CALL_ENTRY_TYPE = "Radio";
    
    const std::string RadioCallEntry::SDS_ENTRY_TYPE = "SDS";


    RadioCallEntry::RadioCallEntry( unsigned long id,
			                        unsigned long consoleId,
                                    const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& details,
                                    RadioCallFactory& radioCallFactory )

        : AbstractCallEntry( id,
                             getCallSourceAsString( details ),
                             getCallInformation( consoleId, details, radioCallFactory ) ),
          m_details( details ),
          m_consoleId( consoleId ),
          m_radioCallFactory( radioCallFactory )
    {
        FUNCTION_ENTRY( "RadioCallEntry" );
        FUNCTION_EXIT;
    }


    RadioCallEntry::~RadioCallEntry()
    {
        FUNCTION_ENTRY( "~RadioCallEntry" );
        FUNCTION_EXIT;
    }


    std::string RadioCallEntry::getCallStatusAsString( TA_Base_Bus::IRadioCorbaDef::ECallStatusType status )
    {
        FUNCTION_ENTRY( "getCallStatusAsString" );

        std::string statusString;

        switch( status )
        {
            case TA_Base_Bus::IRadioCorbaDef::Ringing:
                statusString = "Ringing";
                break;

            case TA_Base_Bus::IRadioCorbaDef::Connected:
                statusString = "Connected";
                break;

            case TA_Base_Bus::IRadioCorbaDef::ConnectedGroup:
                statusString = "Connected Group";
                break;
            
            case TA_Base_Bus::IRadioCorbaDef::Disconnected:
                statusString = "Disconnected";
                break;

            case TA_Base_Bus::IRadioCorbaDef::OnHold:
                statusString = "On Hold";
                break;

            case TA_Base_Bus::IRadioCorbaDef::Deleted:
                statusString = "Deleted";
                break;

            case TA_Base_Bus::IRadioCorbaDef::SessionUpdate:
                statusString = "SessionUpdate";
                break;

            default:
                statusString = "Unrecognised";
                break;
        }

        FUNCTION_EXIT;
        return statusString;
    }


    std::string RadioCallEntry::getCategory() const
    {
        FUNCTION_ENTRY( "getCategory" );
        FUNCTION_EXIT;
        return CALL_ENTRY_TYPE;
    }


    std::string RadioCallEntry::getType() const
    {
        FUNCTION_ENTRY( "getType" );

        if ( TA_Base_Bus::IRadioCorbaDef::TextMessage == m_details.callCategory )
        {
            FUNCTION_EXIT;
            return SDS_ENTRY_TYPE;
        }

        FUNCTION_EXIT;
        return CALL_ENTRY_TYPE;
    }


    std::string RadioCallEntry::getLocationStr() const
    {
        FUNCTION_ENTRY( "getLocationStr" );
        FUNCTION_EXIT;
        return m_details.callerLocation.in();
    }


    int RadioCallEntry::getSeverity() const
    {
        FUNCTION_ENTRY( "getSeverity" );
        if ( true == isEmergency() )
        {
            FUNCTION_EXIT;
            return 2;
        }
        
        FUNCTION_EXIT;
        return 1;
    }


    std::string RadioCallEntry::getStatusStr() const
    {
        FUNCTION_ENTRY( "getStatusStr" );
        FUNCTION_EXIT;
        return getCallStatusAsString( m_details.callStatus );
    }


    time_t RadioCallEntry::getCreateTime() const
    {
        FUNCTION_ENTRY( "getCreateTime" );
        FUNCTION_EXIT;
        return m_details.time;
    }


    bool RadioCallEntry::isEmergency() const
    {
        FUNCTION_ENTRY( "isEmergency" );
        FUNCTION_EXIT;
        return m_details.isEmergency;
    }


    void RadioCallEntry::onActivate()
    {
        FUNCTION_ENTRY("onActivate");

        std::ostringstream runParams;
        runParams << "--call-id=" << m_details.callID;

        std::string message = "Launching Radio Manager with run params '";
        message            += runParams.str();
        message            += "'";

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );

        //
        // Launch the Radio Manager with the call id as a parameter.
        //

        using namespace TA_Base_Bus::TA_GenericGui;
        CPoint pt( GetMessagePos() );
        RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect( SCREEN_CURRENT,
                                                                         AREA_SCHEMATIC,
                                                                         pt.x );

        try
        {
            CWaitCursor wait;

            TA_Base_Bus::AppLauncher::getInstance().launchApplication( RADIO_CONTROLLER_GUI_APPTYPE,
                                                                       runParams.str(),
                                                                       POS_BOUNDED,
                                                                       ALIGN_FIT,
                                                                       NULL,
                                                                       &position );
        }
        catch ( const TA_Base_Core::ApplicationException& ae )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ApplicationException (Radio Manager)", ae.what() );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_630015 );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown (Radio Manager)", "Launching Radio Manager" );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_630016 );
        }

        FUNCTION_EXIT;
    }


    void RadioCallEntry::updateDetails( const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& details )
    {
        FUNCTION_ENTRY( "updateDetails" );

        m_details = details;

        // the details have changed, so update the strings in the banner
        setSource( getCallSourceAsString( m_details ) );
        setInformation( getCallInformation( m_consoleId, m_details, m_radioCallFactory ) );

        FUNCTION_EXIT;
    }


    std::string RadioCallEntry::getCallInformation( unsigned long consoleId,
                                                    const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& call,
                                                    RadioCallFactory& radioCallFactory ) const
    {
        FUNCTION_ENTRY( "getCallInformation" );

        std::string callInformation;

        if ( TA_Base_Bus::IRadioCorbaDef::TextMessage == call.callCategory )
        {
            // Text messages display the text to user
            //CORBA::String_var sdsText = radioCallFactory.getRadioAgent()->getCallText( call.callID, consoleId );//limin
            CORBA::String_var sdsText;
            CORBA_CALL_RETURN( sdsText, radioCallFactory.getRadioAgent(), getCallText, ( call.callID, consoleId ) );//limin

			callInformation = sdsText.in();
        }
        else
        {
            std::ostringstream ostr;

            if ( true == call.isEmergency )
            {
                ostr << "EMERGENCY ";
            }
            
            if ( TA_Base_Bus::IRadioCorbaDef::SingleCall == call.callType )
            {
                ostr << "Voice Call";
            }
            else
            {
                // Anything else (other than SingleCall) is a Conference
                ostr << "Conference Call";
            }
    
            callInformation = ostr.str();
        }

        FUNCTION_EXIT;
        return callInformation;
    }


    std::string RadioCallEntry::getCallSourceAsString( const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& call ) const
    {
        FUNCTION_ENTRY( "getCallSourceAsString" );

        std::ostringstream ostr;
        ostr << call.callerName.in() << " ("
             << call.callerTSI.in() << ")";

        FUNCTION_EXIT;
        return ostr.str();
    }

}
