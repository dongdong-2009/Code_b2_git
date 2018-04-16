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
#if !defined(DisplayItemTasks_H)
#define DisplayItemTasks_H


#include "core/threads/src/IWorkItem.h"


namespace TA_IRS_App
{
    class IDisplayItemLoader;
}

namespace TA_Base_Core
{
    class SingleThreadBarrier;
}

namespace TA_IRS_App
{

    class LoadCameraTask : public TA_Base_Core::IWorkItem
    {

        public:

            /**
             * Constructor
             *
             * @param callback    The object that will do the actual task work
             * @param barrie      To block on when complete
             */
            LoadCameraTask(IDisplayItemLoader& callback,
                           TA_Base_Core::SingleThreadBarrier& barrier);


            /**
             * Destructor
             */
            virtual ~LoadCameraTask();


            /**
             * Executes the task
             */
            virtual void executeWorkItem();


        private:


            /**
             * Private default constructor
             */
            LoadCameraTask();


            /**
             * This is the object used to make the call when executing the task
             */
            IDisplayItemLoader& m_callback;

            /**
             * The barrier to block execution when complete
             */
            TA_Base_Core::SingleThreadBarrier& m_barrier;

    };


    class LoadQuadTask : public TA_Base_Core::IWorkItem
    {

        public:

            /**
             * Constructor
             *
             * @param callback    The object that will do the actual task work
             * @param barrie      To block on when complete
             */
            LoadQuadTask(IDisplayItemLoader& callback,
                         TA_Base_Core::SingleThreadBarrier& barrier);


            /**
             * Destructor
             */
            virtual ~LoadQuadTask();


            /**
             * Executes the task
             */
            virtual void executeWorkItem();


        private:


            /**
             * Private default constructor
             */
            LoadQuadTask();


            /**
             * This is the object used to make the call when executing the task
             */
            IDisplayItemLoader& m_callback;


            /**
             * The barrier to block execution when complete
             */
            TA_Base_Core::SingleThreadBarrier& m_barrier;

    };


    class LoadSequenceTask : public TA_Base_Core::IWorkItem
    {

        public:

            /**
             * Constructor
             *
             * @param callback    The object that will do the actual task work
             * @param barrie      To block on when complete
             */
            LoadSequenceTask(IDisplayItemLoader& callback,
                             TA_Base_Core::SingleThreadBarrier& barrier);


            /**
             * Destructor
             */
            virtual ~LoadSequenceTask();


            /**
             * Executes the task
             */
            virtual void executeWorkItem();


        private:


            /**
             * Private default constructor
             */
            LoadSequenceTask();


            /**
             * This is the object used to make the call when executing the task
             */
            IDisplayItemLoader& m_callback;


            /**
             * The barrier to block execution when complete
             */
            TA_Base_Core::SingleThreadBarrier& m_barrier;

    };


    class LoadStageTask : public TA_Base_Core::IWorkItem
    {

        public:

            /**
             * Constructor
             *
             * @param callback    The object that will do the actual task work
             * @param barrie      To block on when complete
             */
            LoadStageTask(IDisplayItemLoader& callback,
                          TA_Base_Core::SingleThreadBarrier& barrier);


            /**
             * Destructor
             */
            virtual ~LoadStageTask();


            /**
             * Executes the task
             */
            virtual void executeWorkItem();


        private:


            /**
             * Private default constructor
             */
            LoadStageTask();


            /**
             * This is the object used to make the call when executing the task
             */
            IDisplayItemLoader& m_callback;


            /**
             * The barrier to block execution when complete
             */
            TA_Base_Core::SingleThreadBarrier& m_barrier;

    };


    class LoadMonitorTask : public TA_Base_Core::IWorkItem
    {

        public:

            /**
             * Constructor
             *
             * @param callback    The object that will do the actual task work
             * @param barrie      To block on when complete
             */
            LoadMonitorTask(IDisplayItemLoader& callback,
                            TA_Base_Core::SingleThreadBarrier& barrier);


            /**
             * Destructor
             */
            virtual ~LoadMonitorTask();


            /**
             * Executes the task
             */
            virtual void executeWorkItem();


        private:


            /**
             * Private default constructor
             */
            LoadMonitorTask();


            /**
             * This is the object used to make the call when executing the task
             */
            IDisplayItemLoader& m_callback;


            /**
             * The barrier to block execution when complete
             */
            TA_Base_Core::SingleThreadBarrier& m_barrier;

    };

    class LoadQuadFromAgentTask : public TA_Base_Core::IWorkItem
    {

        public:

            /**
             * Constructor
             *
             * @param callback    The object that will do the actual task work
             * @param barrie      To block on when complete
             */
            LoadQuadFromAgentTask(IDisplayItemLoader& callback,
                                  TA_Base_Core::SingleThreadBarrier& barrier);


            /**
             * Destructor
             */
            virtual ~LoadQuadFromAgentTask();


            /**
             * Executes the task
             */
            virtual void executeWorkItem();


        private:


            /**
             * Private default constructor
             */
            LoadQuadFromAgentTask();


            /**
             * This is the object used to make the call when executing the task
             */
            IDisplayItemLoader& m_callback;


            /**
             * The barrier to block execution when complete
             */
            TA_Base_Core::SingleThreadBarrier& m_barrier;

    };


    class LoadSequenceFromAgentTask : public TA_Base_Core::IWorkItem
    {

        public:

            /**
             * Constructor
             *
             * @param callback    The object that will do the actual task work
             * @param barrie      To block on when complete
             */
            LoadSequenceFromAgentTask(IDisplayItemLoader& callback,
                                      TA_Base_Core::SingleThreadBarrier& barrier);


            /**
             * Destructor
             */
            virtual ~LoadSequenceFromAgentTask();


            /**
             * Executes the task
             */
            virtual void executeWorkItem();


        private:


            /**
             * Private default constructor
             */
            LoadSequenceFromAgentTask();


            /**
             * This is the object used to make the call when executing the task
             */
            IDisplayItemLoader& m_callback;


            /**
             * The barrier to block execution when complete
             */
            TA_Base_Core::SingleThreadBarrier& m_barrier;

    };


    class LoadStageFromAgentTask : public TA_Base_Core::IWorkItem
    {

        public:

            /**
             * Constructor
             *
             * @param callback    The object that will do the actual task work
             * @param barrie      To block on when complete
             */
            LoadStageFromAgentTask(IDisplayItemLoader& callback,
                                   TA_Base_Core::SingleThreadBarrier& barrier);


            /**
             * Destructor
             */
            virtual ~LoadStageFromAgentTask();


            /**
             * Executes the task
             */
            virtual void executeWorkItem();


        private:


            /**
             * Private default constructor
             */
            LoadStageFromAgentTask();


            /**
             * This is the object used to make the call when executing the task
             */
            IDisplayItemLoader& m_callback;


            /**
             * The barrier to block execution when complete
             */
            TA_Base_Core::SingleThreadBarrier& m_barrier;

    };


} // TA_IRS_App

#endif // !defined(DisplayItemTasks_H)
