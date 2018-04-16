#include "app/signs/pids_manager/src/AtsTrainDataPoller.h"
#include "app/signs/pids_manager/src/CachedConfig.h"
#include "app/signs/pids_manager/src/TrainList.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include <ace/Guard_T.h>
#include <ace/OS.h>

namespace TA_IRS_App
{

AtsTrainDataPoller::AtsTrainDataPoller(int pollingInterval, ITrainList& trainList) :
        m_shouldRunning(true),
        m_pollingInterval(pollingInterval),
        m_cond(m_lock),
        m_trainList(trainList)
{
    try
    {
        std::vector<TA_Base_Core::CorbaName> atsCorbaNames =
            TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(TA_Base_Core::AtsAgentEntityData::getStaticType(),
                                                                                           CachedConfig::instance().getLocationKey(),
																						   true);
        m_atsTrainCorbaObject.setCorbaName(atsCorbaNames[0]);
    }
    catch (...)
    {
    }
}

AtsTrainDataPoller::~AtsTrainDataPoller()
{
}

void AtsTrainDataPoller::run()
{
    while (m_shouldRunning)
    {
        TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList_var var;
		try
		{
			CORBA_CALL_RETURN(var, m_atsTrainCorbaObject, getTrainInformationList, ());
		}
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );
        }
        catch ( const CORBA::Exception& ce )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );
        }
        catch ( const TA_Base_Core::TransactiveException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "RightsException", ex.what() );
        }
        catch ( ... )
        {
        }

        m_trainList.updateTrainList(var);

        ACE_Time_Value t = ACE_OS::gettimeofday() + ACE_Time_Value(m_pollingInterval, 0);
        ACE_Guard<ACE_Thread_Mutex> lock(m_lock);
        m_cond.wait(&t);
    }
}

void AtsTrainDataPoller::terminate()
{
    m_shouldRunning = false;
    ACE_Guard<ACE_Thread_Mutex> lock(m_lock);
    m_cond.broadcast();
}

}
