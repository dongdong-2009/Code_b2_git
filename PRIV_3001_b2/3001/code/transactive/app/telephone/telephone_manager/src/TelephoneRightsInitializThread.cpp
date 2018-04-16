
#include "app/telephone/telephone_manager/src/TelephoneRightsInitializThread.h"
#include "bus/security/rights_library/src/CtaRights.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/shared_ptr.hpp>


TelephoneRightsInitializThread::TelephoneRightsInitializThread()
    : m_user( NULL ),
      m_semaphore( NULL )
{
    FUNCTION_ENTRY( "TelephoneRightsInitializThread" );
    FUNCTION_EXIT;
}


void TelephoneRightsInitializThread::setSessionIdEntityKey( TelephoneRightsInitializThreadUserProcessRights& user, const std::string& sessionId, unsigned long entityKey )
{
    FUNCTION_ENTRY( "setSessionIdEntityKey" );

    m_user = &user;
    m_sessionId = sessionId;
    m_entityKey = entityKey;

    FUNCTION_EXIT;
}


void TelephoneRightsInitializThread::setRightsSemaphore( TelephoneRightsInitializThreadUserSetRights& user2, TA_Base_Core::Semaphore& semaphore )
{
    FUNCTION_ENTRY( "setRightsSemaphore" );

    m_user2 = &user2;
    m_semaphore = &semaphore;

    FUNCTION_EXIT;
}


TelephoneRightsInitializThread::~TelephoneRightsInitializThread()
{
    FUNCTION_ENTRY( "~TelephoneRightsInitializThread" );

    terminateAndWait();

    FUNCTION_EXIT;
}


void TelephoneRightsInitializThread::run()
{
    FUNCTION_ENTRY( "run" );
	LOGBEFORECOTS("boost::shared_ptr");
    boost::shared_ptr< TA_Base_Bus::ItaRights > ctaRights( new TA_Base_Bus::CtaRights() );
	LOGAFTERCOTS("boost::shared_ptr");
    m_user2->setRights( ctaRights );
    m_semaphore->post();

    TelephoneRights rights;

    std::string reason;

    rights.acaTelephoneConfigureDirectory = ctaRights->isActionPermittedOnResource( m_sessionId, m_entityKey, TA_Base_Bus::aca_TELEPHONE_CONFIGURE_DIRECTORY, reason );
    rights.acaTelephoneSearchDirectory = ctaRights->isActionPermittedOnResource( m_sessionId, m_entityKey, TA_Base_Bus::aca_TELEPHONE_SEARCH_DIRECTORY, reason );
    rights.acaTelephoneUploadDirectory = ctaRights->isActionPermittedOnResource( m_sessionId, m_entityKey, TA_Base_Bus::aca_TELEPHONE_UPLOAD_DIRECTORY, reason );

    m_user->processTelephoneRights( rights );

    FUNCTION_EXIT;
}


void TelephoneRightsInitializThread::terminate()
{
    FUNCTION_ENTRY( "terminate" );
    FUNCTION_EXIT;
}


