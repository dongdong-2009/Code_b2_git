// GRCType3SubTaskMO.cpp: implementation of the GRCType3SubTaskMO class.
//
//////////////////////////////////////////////////////////////////////

#include "PlanAgent.h"
#include "GRCType3SubTaskMO.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{

	GRCType3SubTaskMO::GRCType3SubTaskMO( GRCType3SubTask * pSubTask, ACE_Future<bool> & execResult )
		: m_execResult(execResult),
		  m_pSubTask(pSubTask)
	{
		TA_Assert( m_pSubTask != NULL );		
	}

	int GRCType3SubTaskMO::call()
	{
		m_pSubTask->getDataPointSetStep()->setExecResult(m_execResult);
		try
		{
			execute();
			m_execResult.set(true);
		}
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException, reason=%s", ex.what() );
			try
			{
				m_execResult.set(false);
			}
			catch(...)
			{
				LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set return value. Dead-lock has occurred." );
			}
        }
		catch( ... )
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Error talking to peers." );

			try
			{
				m_execResult.set(false);
			}
			catch(...)
			{
				LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set return value. Dead-lock has occurred." );
			}
		}
		
		return 0 ;
	}

	void GRCType3SubTaskMO::execute()
	{
		m_pSubTask->doSetDataPointImpl();
	}
	


}
