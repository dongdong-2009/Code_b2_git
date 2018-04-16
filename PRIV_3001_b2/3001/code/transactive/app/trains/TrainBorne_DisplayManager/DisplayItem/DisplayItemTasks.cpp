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

#include "app/trains/TrainBorne_DisplayManager/DisplayItem/DisplayItemTasks.h"
#include "app/trains/TrainBorne_DisplayManager/DisplayItem/IDisplayItemLoader.h"

#include "core/synchronisation/src/SingleThreadBarrier.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{
//
    LoadCameraTask::LoadCameraTask( IDisplayItemLoader& callback,
                                    TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_barrier( barrier )
 // LoadCameraTask::LoadCameraTask( IDisplayItemLoader& callback )
   // : m_callback( callback )
    {
        FUNCTION_ENTRY( "LoadCameraTask" );
        FUNCTION_EXIT;
    }


    LoadCameraTask::~LoadCameraTask()
    {
        FUNCTION_ENTRY( "~LoadCameraTask" );
        FUNCTION_EXIT;
    }


    void LoadCameraTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.loadCamerasFromDatabase();

       m_barrier.post();

        FUNCTION_EXIT;
    }


    LoadQuadTask::LoadQuadTask( IDisplayItemLoader& callback,
                                TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_barrier( barrier )
//    LoadQuadTask::LoadQuadTask( IDisplayItemLoader& callback)
 // : m_callback( callback )
    {
        FUNCTION_ENTRY( "LoadQuadTask" );
        FUNCTION_EXIT;
    }


    LoadQuadTask::~LoadQuadTask()
    {
        FUNCTION_ENTRY( "~LoadQuadTask" );
        FUNCTION_EXIT;
    }


    void LoadQuadTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.loadQuadsFromDatabase();

        m_barrier.post();

        FUNCTION_EXIT;
    }


    LoadSequenceTask::LoadSequenceTask( IDisplayItemLoader& callback,
                                        TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_barrier( barrier )
//    LoadSequenceTask::LoadSequenceTask( IDisplayItemLoader& callback)
  // : m_callback( callback )
    {
        FUNCTION_ENTRY( "LoadSequenceTask" );
        FUNCTION_EXIT;
    }


    LoadSequenceTask::~LoadSequenceTask()
    {
        FUNCTION_ENTRY( "~LoadSequenceTask" );
        FUNCTION_EXIT;
    }


    void LoadSequenceTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.loadSequencesFromDatabase();

        m_barrier.post();

        FUNCTION_EXIT;
    }


    LoadStageTask::LoadStageTask( IDisplayItemLoader& callback,
                                  TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_barrier( barrier )
//    LoadStageTask::LoadStageTask( IDisplayItemLoader& callback)
//        : m_callback( callback )
    {
        FUNCTION_ENTRY( "LoadStageTask" );
        FUNCTION_EXIT;
    }


    LoadStageTask::~LoadStageTask()
    {
        FUNCTION_ENTRY( "~LoadStageTask" );
        FUNCTION_EXIT;
    }


    void LoadStageTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

   //     m_callback.loadStagesFromDatabase();

        m_barrier.post();

        FUNCTION_EXIT;
    }


   LoadMonitorTask::LoadMonitorTask( IDisplayItemLoader& callback,
                                      TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_barrier( barrier )
 // LoadMonitorTask::LoadMonitorTask( IDisplayItemLoader& callback)
   // : m_callback( callback )
    {
        FUNCTION_ENTRY( "LoadMonitorTask" );
        FUNCTION_EXIT;
    }


    LoadMonitorTask::~LoadMonitorTask()
    {
        FUNCTION_ENTRY( "~LoadMonitorTask" );
        FUNCTION_EXIT;
    }


    void LoadMonitorTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.loadMonitorsFromDatabase();

        m_barrier.post();

        FUNCTION_EXIT;
    }


    LoadQuadFromAgentTask::LoadQuadFromAgentTask( IDisplayItemLoader& callback,
                                                  TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_barrier( barrier )
//    LoadQuadFromAgentTask::LoadQuadFromAgentTask( IDisplayItemLoader& callback)
//        : m_callback( callback )
    {
        FUNCTION_ENTRY( "LoadQuadFromAgentTask" );
        FUNCTION_EXIT;
    }


    LoadQuadFromAgentTask::~LoadQuadFromAgentTask()
    {
        FUNCTION_ENTRY( "~LoadQuadFromAgentTask" );
        FUNCTION_EXIT;
    }


    void LoadQuadFromAgentTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

     //   m_callback.loadQuadsFromAgent();

        m_barrier.post();

        FUNCTION_EXIT;
    }

    LoadSequenceFromAgentTask::LoadSequenceFromAgentTask( IDisplayItemLoader& callback,
                                                          TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_barrier( barrier )
//    LoadSequenceFromAgentTask::LoadSequenceFromAgentTask( IDisplayItemLoader& callback)
 // : m_callback( callback )

    {
        FUNCTION_ENTRY( "LoadSequenceFromAgentTask" );
        FUNCTION_EXIT;
    }


    LoadSequenceFromAgentTask::~LoadSequenceFromAgentTask()
    {
        FUNCTION_ENTRY( "~LoadSequenceFromAgentTask" );
        FUNCTION_EXIT;
    }


    void LoadSequenceFromAgentTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.loadSequencesFromAgent();

        m_barrier.post();

        FUNCTION_EXIT;
    }


    LoadStageFromAgentTask::LoadStageFromAgentTask( IDisplayItemLoader& callback,
                                                    TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_barrier( barrier )
//    LoadStageFromAgentTask::LoadStageFromAgentTask( IDisplayItemLoader& callback)
 // : m_callback( callback )
    {
        FUNCTION_ENTRY( "LoadStageFromAgentTask" );
        FUNCTION_EXIT;
    }


    LoadStageFromAgentTask::~LoadStageFromAgentTask()
    {
        FUNCTION_ENTRY( "~LoadStageFromAgentTask" );
        FUNCTION_EXIT;
    }


    void LoadStageFromAgentTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

     //   m_callback.loadStagesFromAgent();

        m_barrier.post();

        FUNCTION_EXIT;
    }


} // TA_IRS_App
