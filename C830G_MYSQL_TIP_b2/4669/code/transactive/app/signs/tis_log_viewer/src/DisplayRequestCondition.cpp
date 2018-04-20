/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/signs/tis_log_viewer/src/DisplayRequestCondition.h"

#include "core/utilities/src/DebugUtil.h"

#include <iomanip>
#include <string>


namespace TA_IRS_App
{

    DisplayRequestCondition::DisplayRequestCondition( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                      time_t timestamp,
                                                      const std::string& sessionId,
                                                      TA_Base_Core::TisCommandType originalCommand,
                                                      unsigned long maximumRequestTime )
            : m_trainId( trainId ),
              m_timestamp( timestamp ),
              m_sessionId( sessionId ),
              m_originalCommand( originalCommand ),
              m_waitTimeMillis( maximumRequestTime ),
              m_semaphore( 0 ),
              m_success( false ),   // initialise to a timeout in case a response isnt received
              m_errorDetails( "Timed Out" )
    {
        FUNCTION_ENTRY( "DisplayRequestCondition" );
        FUNCTION_EXIT;
    }


    DisplayRequestCondition::~DisplayRequestCondition()
    {
        FUNCTION_ENTRY( "~DisplayRequestCondition" );
        FUNCTION_EXIT;
    }


    void DisplayRequestCondition::waitForCompletion()
    {
        FUNCTION_ENTRY( "waitForCompletion" );

        m_semaphore.timedWait( m_waitTimeMillis );

        FUNCTION_EXIT;
    }


    bool DisplayRequestCondition::wasSuccessful() const
    {
        FUNCTION_ENTRY( "wasSuccessful" );
        FUNCTION_EXIT;
        return m_success;
    }


    std::string DisplayRequestCondition::getErrorDetails() const
    {
        FUNCTION_ENTRY( "getErrorDetails" );
        FUNCTION_EXIT;
        return m_errorDetails;
    }


    bool DisplayRequestCondition::handleDisplayResult( const TA_Base_Core::TTISDisplayResult& displayResult )
    {
        FUNCTION_ENTRY( "handleDisplayResult" );

        // check the details match
        if ( ( displayResult.trainId != m_trainId ) ||
             ( displayResult.timestamp != m_timestamp ) ||
             ( displayResult.sessionId.in() != m_sessionId ) ||
             ( displayResult.originalCommand != m_originalCommand ) )
        {
            FUNCTION_EXIT;
            return false;
        }

        // update the train's status
        m_success = displayResult.success;
        m_errorDetails = displayResult.errorDetails.in();

        // unblock the wait method
        m_semaphore.post();

        FUNCTION_EXIT;
        return true;
    }

}
