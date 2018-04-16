// ActiveNullStep.h: interface for the ActiveNullStep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVENULLSTEP_H__D6FD31B8_5452_431B_AB84_EF6E28746487__INCLUDED_)
#define AFX_ACTIVENULLSTEP_H__D6FD31B8_5452_431B_AB84_EF6E28746487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ActiveStep.h"

namespace TA_Base_App
{
	class ActiveNullStep : public ActiveStep  
	{
		// Public methods
		public:
				ActiveNullStep( const TA_Base_Core::StepDetail& stepDetail, 
							 IActivePlanFlow& flowControl );
			
		protected:
				virtual TA_Base_Core::EActiveStepState executeImpl( void );

		private:

			ActiveNullStep& operator=( const ActiveNullStep& );
			ActiveNullStep( const ActiveNullStep& );

			IActivePlanFlow& m_flowControl;
	};
}

#endif // !defined(AFX_ACTIVENULLSTEP_H__D6FD31B8_5452_431B_AB84_EF6E28746487__INCLUDED_)
