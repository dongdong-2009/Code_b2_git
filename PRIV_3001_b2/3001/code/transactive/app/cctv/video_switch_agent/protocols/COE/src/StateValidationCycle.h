#if !defined(STATE_VALIDATION_CYCLE_INCLUDED_H)
#define STATE_VALIDATION_CYCLE_INCLUDED_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#include "core/threads/src/Thread.h"
#include "core/utilities/src/Timer.h"
#include "core/utilities/src/Timer.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"

#include <list>

namespace TA_IRS_App
{
    class IStateValidationItem;
}

namespace TA_IRS_App
{
    class StateValidationCycle : public TA_Base_Core::Thread
    {
    public:
        
        /**
          * StateValidationCycle
          *
          * Standard constructor.
          *
          */
        StateValidationCycle();


        /**
          * ~StateValidationCycle
          *
          * Standard destructor.
          */
        virtual ~StateValidationCycle();


        /** 
          * setValidationCyclePeriods
          *
          * description
          *
          * @param lowPriority
          * @param 
          *
          */
        void setValidationCyclePeriods( unsigned long lowPriority, unsigned long highPriority );


        /** 
          * startValidationCycle
          *
          * description
          *
          */
        void startValidationCycle();


        /** 
          * stopValidationCycle
          *
          * description
          *
          */
        void stopValidationCycle();


        /** 
          * addValidationItem
          *
          * description
          *
          * @param 
          */
        void addValidationItem( IStateValidationItem& item, bool isHighPriorityItem );


        /** 
          * removeValidationItem
          *
          * description
          *
          * @param 
          */
        void removeValidationItem( IStateValidationItem& item, bool isHighPriorityItem );


        ////////////////////////
        //// Thread methods ////
        ////////////////////////


        /**
          * run
          *
          * Method executed by thread.
          */
        virtual void run();


        /** 
          * terminate
          *
		  * Signals the run method to complete. This method should not be called
          * directly.
          */
		virtual void terminate();	

    private:

        //
        // Disable copy constructor and assignment operator.
        //
        StateValidationCycle( const StateValidationCycle& theStateValidationCycle );
        StateValidationCycle& operator=( const StateValidationCycle& );

        //
        // Private Helper Functions
        //

        /** 
          * validateLowPriorityItems
          *
          * description
          */
        void validateLowPriorityItems();


        /** 
          * validateHighPriorityItems
          *
          * description
          */
        void validateHighPriorityItems();


        /** 
          * calculateWaitPeriod
          *
          * description
          *
          *
          * @return 
          */
        unsigned long calculateWaitPeriod();

        
        /** 
          * hasLowPriorityPeriodExpired
          *
          * description
          *
          *
          * @return 
          */
        bool hasLowPriorityPeriodExpired();


        /** 
          * hasHighPriorityPeriodExpired
          *
          * description
          *
          *
          * @return 
          */
        bool hasHighPriorityPeriodExpired();

        //
        // Private Types
        //
        
        //
        // Private member variables
        //

        volatile bool m_doTerminate;

        unsigned long m_lowPriorityPeriod;
        unsigned long m_highPriorityPeriod;

        TA_Base_Core::Timer m_lowPriorityTimer;
        TA_Base_Core::Timer m_highPriorityTimer;

        std::list< IStateValidationItem* > m_lowPriorityItems;
        std::list< IStateValidationItem* > m_highPriorityItems;

        TA_Base_Core::Condition m_cycleCondition;
        TA_Base_Core::ReEntrantThreadLockable m_threadStateLock;


        static const unsigned long TERMINATE_DELAY;
    }; // StateValidationCycle

} // TA_IRS_App
    
#endif // !defined(STATE_VALIDATION_CYCLE_INCLUDED_H)
