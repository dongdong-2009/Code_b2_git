/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibrary.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Responsible for managing the retrieval and updates of the Telephone Speed Dial
  * Button details. It will call on the TelephoneDirectLineAccessFactory for
  * interfacing to the database.
  */

#include "bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibrary.h"
#include "bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibraryException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/ITelephoneDirectLine.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineAccessFactory.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/ConfigTelephoneDirectLine.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TelephoneManagerSpeedDialBroadcastMessage_MessageTypes.h"
#include "bus/telephone/telephone_manager/IDL/src/ITelephoneManagerCommonCorbaDef.h"

using namespace std;
using namespace TA_IRS_Core;

using TA_Base_Core::ISession;
using TA_Base_Core::SessionAccessFactory;

unsigned long TelephoneDirectLinesLibrary::UNUSED_KEY_VALUE = -1;

const string TelephoneDirectLinesLibrary::SETNAME_HOTLINES ("HotLines");
const string TelephoneDirectLinesLibrary::SETNAME_USER     ("User");
const string TelephoneDirectLinesLibrary::SETNAME_STATIONS ("Stations");
const string TelephoneDirectLinesLibrary::SETNAME_OCR      ("OCR");
const string TelephoneDirectLinesLibrary::SETNAME_DCR      ("DCR");
const string TelephoneDirectLinesLibrary::SETNAME_OTHERS   ("Others");
const string TelephoneDirectLinesLibrary::SETNAME_PCP      ("PCP");

const int TelephoneDirectLinesLibrary::MAX_HOTLINES_BTN = 8;
const int TelephoneDirectLinesLibrary::MAX_TABS_BTN = 36;

long TelephoneDirectLinesLibrary::getOperatorKey(const std::string& sessionId, const std::string& SetName)
{
    FUNCTION_ENTRY("getOperatorKey");

    long operatorKey = UNUSED_KEY_VALUE;
	//TD 15894, 15895
//    if (SetName.compare(SETNAME_USER) == 0)
//    {
//        ISession* session = SessionAccessFactory::getInstance().getSession(sessionId);
//        
//        operatorKey = session->getOperatorKey();
//        
//        delete session;
//        session = NULL;
//    }
	//TD 15894, 15895

    FUNCTION_EXIT;
    return operatorKey;
}


long TelephoneDirectLinesLibrary::getProfileKey(const std::string& sessionId, const std::string& SetName)
{
    FUNCTION_ENTRY("getProfileKey");

    long profileKey = UNUSED_KEY_VALUE;

	//TD 15894, 15895
    if (SetName.compare(SETNAME_USER) == 0)
    // if (SetName.compare(SETNAME_USER) != 0)
	//TD 15894, 15895
    {
        ISession* session = SessionAccessFactory::getInstance().getSession(sessionId);

        profileKey = session->getProfileKey();
        
        delete session;
        session = NULL;
    }

    FUNCTION_EXIT;
    return profileKey;
}


TelephoneDirectLinesLibrary::TelephoneDirectLinesLibrary()
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
}


TelephoneDirectLinesLibrary::~TelephoneDirectLinesLibrary()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}


TelephoneSpeedDialButtonDetails TelephoneDirectLinesLibrary::getButtonDetails(const std::string& sessionId, const std::string& SetName, const int buttonNo)
{
    FUNCTION_ENTRY("getButtonDetails");

    TelephoneSpeedDialButtonDetails btnDetails;

    long operatorKey = 0;
    long profileKey = 0;

    try
    {
        operatorKey = getOperatorKey(sessionId, SetName);
        profileKey = getProfileKey(sessionId, SetName);
    }
    catch(TA_Base_Core::DatabaseException&)
    {
        FUNCTION_EXIT;
        return btnDetails;
    }
    catch(TA_Base_Core::DataException&)
    {
        FUNCTION_EXIT;
        return btnDetails;
    }

    ITelephoneDirectLine* pTelephoneDirectLine = NULL;
    try
    {
        pTelephoneDirectLine = TelephoneDirectLineAccessFactory::getInstance().getButtonDetails(operatorKey, profileKey, SetName, buttonNo);

        if(pTelephoneDirectLine == NULL)
        {
            FUNCTION_EXIT;
            return btnDetails;
        }
    }
    catch(TA_Base_Core::DatabaseException&)
    {
        FUNCTION_EXIT;
        return btnDetails;
    }
    catch(TA_Base_Core::DataException&)
    {
        FUNCTION_EXIT;
        return btnDetails;
    }


    btnDetails.SetName = SetName;
    btnDetails.buttonPosition = pTelephoneDirectLine->getButtonPosition();
    btnDetails.label = pTelephoneDirectLine->getLabel();
    btnDetails.telephoneNumber = pTelephoneDirectLine->getTelephoneNumber();

    delete pTelephoneDirectLine;
    pTelephoneDirectLine = NULL;

    FUNCTION_EXIT;
    return btnDetails;
}


void TelephoneDirectLinesLibrary::getButtonSet(std::vector<TelephoneSpeedDialButtonDetails>& btnDetails, const std::string& sessionId, const std::string& SetName)
{
    FUNCTION_ENTRY("getButtonSet");
    
    long operatorKey = 0;
    long profileKey = 0;

    try
    {
        operatorKey = getOperatorKey(sessionId, SetName);
        profileKey = getProfileKey(sessionId, SetName);
    }
    catch(TA_Base_Core::DatabaseException&)
    {
        FUNCTION_EXIT;
        return;
    }
    catch(TA_Base_Core::DataException&)
    {
        FUNCTION_EXIT;
        return;
    }

    ITelephoneDirectLines telephoneDirectLines;

    try
    {
        telephoneDirectLines = TelephoneDirectLineAccessFactory::getInstance().getButtonSet(operatorKey, profileKey, SetName);
    }
    catch (TA_Base_Core::DatabaseException&) 
    {
        FUNCTION_EXIT;
        return;
    }

    btnDetails.clear();
    btnDetails.resize( telephoneDirectLines.size() );

    for ( size_t i = 0; i < telephoneDirectLines.size(); ++i )
    {
        btnDetails[ i ].SetName = SetName;
        btnDetails[ i ].label = telephoneDirectLines[ i ]->getLabel();
        btnDetails[ i ].buttonPosition = telephoneDirectLines[ i ]->getButtonPosition();
        btnDetails[ i ].telephoneNumber = telephoneDirectLines[ i ]->getTelephoneNumber();

        delete telephoneDirectLines[i];
    }
}


bool TelephoneDirectLinesLibrary::setButton(const std::string& sessionId, TelephoneSpeedDialButtonDetails& details)
{
    FUNCTION_ENTRY("setButton");
    
    long operatorKey = 0;
    long profileKey = 0;

    try
    {
        operatorKey = getOperatorKey(sessionId, details.SetName);
        profileKey = getProfileKey(sessionId, details.SetName);
    }
    catch(TA_Base_Core::DatabaseException& dbE)
    {
        std::ostringstream desc;
        desc << "DatabaseException thrown when getting operatory/profile key - " << dbE.what() ;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, desc.str().c_str());

        FUNCTION_EXIT;
        return false;
    }
    catch(TA_Base_Core::DataException& e)
    {
        std::ostringstream desc;
        desc << "DataException thrown when getting operatory/profile key - " << e.what() ;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, desc.str().c_str());

        FUNCTION_EXIT;
        return false;
    }

    try
    {
        ConfigTelephoneDirectLine* cast = NULL;
        ITelephoneDirectLine* pTelephoneDirectLine = NULL;
        try
        {
            pTelephoneDirectLine =
                TelephoneDirectLineAccessFactory::getInstance().getButtonDetails(operatorKey, profileKey, details.SetName, details.buttonPosition, true);
        }
        catch(TA_Base_Core::DatabaseException& dbE)
        {
            std::ostringstream desc;
            desc << "TelephoneDirectLineAccessFactory::getButtonDetails(...) throws DatabaseException [" << dbE.what() << "]";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, desc.str().c_str());

            FUNCTION_EXIT;
            return false;
        }
        catch(TA_Base_Core::DataException&)
        {
            pTelephoneDirectLine = NULL;
        }

        if(pTelephoneDirectLine == NULL)// Either Setname or button information are not present
        {
            // Since no record exist and both label and number are empty, then, there's no point in creating the record in the DB
            if(details.label.empty() && details.telephoneNumber.empty())
            {
                return true; 
            }

            bool isNewSet = false;
            try
            {
                pTelephoneDirectLine = 
                    TelephoneDirectLineAccessFactory::getInstance().getSetDetails(operatorKey, profileKey, details.SetName, true);
            }
            catch(TA_Base_Core::DatabaseException& dbE)
            {
                std::ostringstream desc;
                desc << "TelephoneDirectLineAccessFactory::getSetDetails(...) throws DatabaseException [" << dbE.what() << "]";
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, desc.str().c_str());

                FUNCTION_EXIT;
                return false;
            }
            catch(TA_Base_Core::DataException& e)
            {
                pTelephoneDirectLine = NULL;

                if(e.getFailType() == TA_Base_Core::DataException::NO_VALUE) // Set and button does not exist
                {
                    // Brand new data, no button or setname
                    pTelephoneDirectLine = TelephoneDirectLineAccessFactory::getInstance().createNewButtonSetRecord(true);
                    isNewSet = true;
                }
                else // Other data exception such as NOT_UNIQUE will cause exceptions to be raised
                {
                    std::ostringstream reason;
                    reason << "TelephoneDirectLineAccessFactory::getSetDetails(...) throws DataException [" << e.what() << "]";
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

                    FUNCTION_EXIT;
                    TA_THROW( TelephoneDirectLinesLibraryException(reason.str().c_str()));
                }
            }
            
            TA_ASSERT(pTelephoneDirectLine != NULL, "TelephoneDirectLineAccessFactory could not create new button set record");

            cast = dynamic_cast<ConfigTelephoneDirectLine*>(pTelephoneDirectLine);
            TA_ASSERT(cast != NULL, "IConfigTelephoneDirectLine object could not be converted into a ConfigTelephoneDirectLine");

            cast->setButtonPosition(details.buttonPosition);
            cast->setLabel(details.label);
            cast->setTelephoneNumber(details.telephoneNumber);
            if(isNewSet == true)
            {
                cast->setOperatorKey(operatorKey);
                cast->setSeProfId(profileKey);
                cast->setSetName(details.SetName);
                cast->applyChanges();
            }
            else
            {
                cast->writeBtnData();
            }

            delete cast;
            cast = NULL;

        }
        else // Both 
        {
            cast = dynamic_cast<ConfigTelephoneDirectLine*>(pTelephoneDirectLine);
            TA_ASSERT(cast != NULL, "IConfigTelephoneDirectLine object could not be converted into a ConfigTelephoneDirectLine");

            if(details.label.empty() && details.telephoneNumber.empty())
            {
                cast->deleteTeSpeedDialButtonRecord();
            }
            else
            {
                cast->setLabel(details.label);
                cast->setTelephoneNumber(details.telephoneNumber);
                cast->writeBtnData();
            }
            delete cast;
            cast = NULL;
        }
        
    }
    catch(TA_Base_Core::DatabaseException& dbE)
    {
        std::ostringstream reason;
        reason << "DatabaseException caught [" << dbE.what() << "]";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

        FUNCTION_EXIT;
        TA_THROW( TelephoneDirectLinesLibraryException(reason.str().c_str()));
    }
    catch(TA_Base_Core::DataException& e)
    {
        std::ostringstream reason;
        reason << "DataException caught [" << e.what() << "]";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

        FUNCTION_EXIT;
        TA_THROW( TelephoneDirectLinesLibraryException(reason.str().c_str()));
    }
    catch(...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught");

        FUNCTION_EXIT;
        TA_THROW( TelephoneDirectLinesLibraryException("Unknown exception caught"));
    }

    // ---------------------------------------------------------------------------
    // send corba messages
    // ---------------------------------------------------------------------------
    
    TA_Base_Core::CommsMessageSender* sender = 
        TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::TelephoneManagerSpeedDialBroadcastMessage::Context);

    TA_IRS_Bus::DirectLineEvent directLineEvent;
    directLineEvent.SetName = details.SetName.c_str();
    directLineEvent.buttonPosition = details.buttonPosition;
    directLineEvent.telephoneNumber = details.telephoneNumber.c_str();
    directLineEvent.label = details.label.c_str();

    CORBA::Any commsData;
    commsData <<= directLineEvent;
    sender->sendBroadcastCommsMessage(TA_Base_Core::TelephoneManagerSpeedDialBroadcastMessage::DirectLineModifiedEvent, commsData);

    delete sender;
    
    // ---------------------------------------------------------------------------

    FUNCTION_EXIT;
    return true;
}
