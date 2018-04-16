// StepExecutionEngine.cpp: implementation of the StepExecutionEngine class.
//
//////////////////////////////////////////////////////////////////////


#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/StepExecutionTask.h"
#include "app/response_plans/plan_agent/src/ThreadReleaseTask.h"
#include "app/response_plans/plan_agent/src/StepExecutionEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{
    StepExecutionEngine::StepExecutionEngine( ActivePlan& refTask ) :
    m_refTask( refTask ),
    m_oTaskQueue(),
    m_bInSvc( false )
	{
	}

	StepExecutionEngine::~StepExecutionEngine()
	{
	}

	int StepExecutionEngine::open(void*)
    {
		return 0;
	}

	int StepExecutionEngine::close(u_long flags )
	{
		return 0;
	}

	int StepExecutionEngine::svc(void)
	{
		while( m_bInSvc )
		{
			//std::auto_ptr<ACE_Method_Request> ptrTask(m_oTaskQueue.dequeue());

			// No method to execute.
			//if( NULL == ptrTask.get() )
			//{
   //             // Check the activation queue is OK.
   //             if ( ESHUTDOWN == ACE_OS::last_error() )
   //             {
   //                 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Idle thread receive hang up signal" );
   //             }
   //             else
   //             {
   //                 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
   //                     "ACE_Activation_Queue::dequeue failed with errno %d", ACE_OS::last_error() );
   //             }

   //             break;
			//}

            int nNewTaskCount = 0;

            try
            {
                // Method to execute
                //nNewTaskCount = ptrTask->call();
                nNewTaskCount = m_refTask.runStep();
            }
            catch( const std::exception& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when executing task" );
            }

            if ( 0 > nNewTaskCount )
            {
                break;
            }
            else if ( 0 < nNewTaskCount )
            {
                parallelAll( nNewTaskCount );
            }
		}

		return 0;
	}

    void StepExecutionEngine::parallelAll( const std::list<ActiveStep*>& lstNewSteps, const int nResuedStepCount )
    {
        if ( !m_bInSvc )
        {
            return;
        }

        int nDiffer = nResuedStepCount - static_cast<int>(lstNewSteps.size());

        if ( nDiffer < 0 )
        {
            static const int ForceNewThread = 1;

            activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, -nDiffer, ForceNewThread );
        }
        else
        {
            for ( int nLoop = 0; nLoop < nDiffer; ++nLoop )
            {
                m_oTaskQueue.enqueue( new ThreadReleaseTask() );
            }
        }

        for (std::list<ActiveStep*>::const_iterator itLoop = lstNewSteps.begin(); itLoop != lstNewSteps.end(); ++itLoop )
        {
            ActiveStep* pNewStep = *itLoop;
            if ( NULL == pNewStep )
            {
                continue;
            }

            //m_oTaskQueue.enqueue( new StepExecutionTask( *pNewStep ));
        }
    }

    void StepExecutionEngine::parallelAll( const int nNewTaskCount )
    {
        if ( !m_bInSvc )
        {
            return;
        }

        static const int ForceNewThread = 1;

        activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, nNewTaskCount, ForceNewThread );

        for ( int nLoop = 0; nLoop < nNewTaskCount; ++nLoop )
        {
            //m_oTaskQueue.enqueue( new StepExecutionTask( m_refOwner ) );
        }
    }

    void StepExecutionEngine::start( const int nCount )
    {
        m_bInSvc = true;
        activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, nCount );

        for ( int nLoop = 0; nLoop < nCount; ++nLoop )
        {
            //m_oTaskQueue.enqueue( new StepExecutionTask( m_refOwner ) );
        }
    }

    void StepExecutionEngine::stop()
    {
        m_bInSvc = false;
        for ( size_t nLoop = 0; nLoop < thr_count(); ++nLoop )
        {
            //m_oTaskQueue.enqueue( new ThreadReleaseTask() );
        }

        wait();
    }
}