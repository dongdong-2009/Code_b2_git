#ifndef SORTABLEACTIVESTEPID_H_INCLUDED
#define SORTABLEACTIVESTEPID_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/SortableActiveStepId.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

namespace TA_Base_Bus
{
	class SortableActiveStepId
	{
	public:
        SortableActiveStepId( const unsigned long nodeId, const unsigned long planInstance, const unsigned long stepNumber );
        SortableActiveStepId( const SortableActiveStepId& );
		bool operator<( const SortableActiveStepId& rhs ) const;

		void setNodeId( const unsigned long nodeId );
		void setPlanInstance( const unsigned long planInstance );
		void setStepNumber( const unsigned long stepNumber );

		unsigned long getNodeId() const;
		unsigned long getPlanInstance() const;
		unsigned long getStepNumber() const;

    private:
        SortableActiveStepId();
        SortableActiveStepId& operator=( const SortableActiveStepId& );

        unsigned long m_nodeId;
        unsigned long m_planInstance;
        unsigned long m_stepPosition;
	};

} // TA_Base_Bus

#endif //SORTABLEACTIVESTEPID_H_INCLUDED
