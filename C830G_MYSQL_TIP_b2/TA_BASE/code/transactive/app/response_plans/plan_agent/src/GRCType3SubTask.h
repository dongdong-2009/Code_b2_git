// GRCType3SubTask.h: interface for the GRCType3SubTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRCTYPE3SUBTASK_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_)
#define AFX_GRCTYPE3SUBTASK_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
	class GRCType3SubTask : public ACE_Task<ACE_MT_SYNCH>
	{
	public:
		GRCType3SubTask(unsigned long stepNumber, ActiveDataPointSetStep * pStep);
		virtual ~GRCType3SubTask();
	public:
		int open(void*);
		int close(u_long flags = 0 );
		int svc(void);
		ACE_Future<bool> doSetDataPoint( );
		ActiveDataPointSetStep * getDataPointSetStep();
	public:
		void doSetDataPointImpl( );
	
	private:
		ACE_Activation_Queue m_activationQueue;

		static const ACE_Time_Value m_msgQueueTimeout;
		static const ACE_Time_Value m_activationQueueTimeout;
		static const unsigned long THREAD_COUNT;

		unsigned long m_stepNumber;
		ActiveDataPointSetStep * m_pStep;
		
		
	};
}

#endif // !defined(AFX_GRCTYPE3SUBTASK_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_)
