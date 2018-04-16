#include "app/response_plans/plan_step_plugin/pa/station_pa/active/src/ActiveStationPaStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include "core/exceptions/src/ObjectResolutionException.h"

namespace TA_Base_App
{
    ActiveStationPaStep::ActiveStationPaStep(const BasicParameter& detail, IActivePlan& iPlan)
		: ActiveStep(m_params, iPlan),
          m_params(detail)
    {
    }

    ActiveStationPaStep::~ActiveStationPaStep()
    {
    }

    bool ActiveStationPaStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Staiton PA step %lu", id() );

        bool bExecSucceed = false;

        try
        {

				
			   /*string broadcastStationDva 
            ( in MessageKeySeq messages, 
              in ZoneCoverage zones, 
              in unsigned short dwellInSecs, 
              in boolean hasChime, 
              in TA_Base_Core::DateTime startTime, 
              in TA_Base_Core::DateTime stopTime, 
              in unsigned short periodInSecs, 
              in boolean isSynchronisedWithTis, 
				  in string groupName,				// zone group selection for broadcast Id
              in string sessionId ) */
           
            //On successful completion of the step
            bExecSucceed = true;
        }
        catch( const TA_Base_Core::ObjectResolutionException& e )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.what();
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
        }
        catch( ... )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": ";

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );
        }

        return bExecSucceed;
    }
}