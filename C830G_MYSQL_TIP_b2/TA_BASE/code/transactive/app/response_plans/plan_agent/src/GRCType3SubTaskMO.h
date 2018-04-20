// GRCType3SubTaskMO.h: interface for the GRCType3SubTaskMO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRCTYPE3SUBTASKMO_H__079D7AFF_3EE2_40AF_B7D7_D0E0C41090A1__INCLUDED_)
#define AFX_GRCTYPE3SUBTASKMO_H__079D7AFF_3EE2_40AF_B7D7_D0E0C41090A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
	class GRCType3SubTask;

	//class GRCType3SubTaskMO  : public ACE_Method_Object
	class GRCType3SubTaskMO  : public ACE_Method_Request
	{
	public:
		GRCType3SubTaskMO( GRCType3SubTask * pSubTask, ACE_Future<bool> & execResult );
		virtual int call();
		
	protected:
		virtual void execute();
		
	private:
		GRCType3SubTaskMO& operator=( const GRCType3SubTaskMO& );
		GRCType3SubTaskMO( const GRCType3SubTaskMO& );
		
	protected:
		ACE_Future<bool> m_execResult;
		GRCType3SubTask * m_pSubTask;
	};
	
}
#endif // !defined(AFX_GRCTYPE3SUBTASKMO_H__079D7AFF_3EE2_40AF_B7D7_D0E0C41090A1__INCLUDED_)
