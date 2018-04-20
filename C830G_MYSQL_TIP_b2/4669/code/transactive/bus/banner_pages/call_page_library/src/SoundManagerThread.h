#include "SoundEntity.h"
#include "core\threads\src\QueueProcessor.h"
#include "boost\shared_ptr.hpp"
#include "bus\sound\sound_client\src\SoundManagerProxy.h"
// SoundManagerThread.h: interface for the SoundManagerThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDMANAGERTHREAD_H__CEC344BB_0328_46DA_BCFC_2FE2856D3DD8__INCLUDED_)
#define AFX_SOUNDMANAGERTHREAD_H__CEC344BB_0328_46DA_BCFC_2FE2856D3DD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
namespace TA_IRS_Bus 
{
	class SoundManagerThread : public TA_Base_Core::QueueProcessor < SoundEntity >
	{
	public:
		SoundManagerThread();
		virtual ~SoundManagerThread();
		virtual void processEvent( boost::shared_ptr< SoundEntity > newEvent ) ;
			
	private:
	
        void init() ; 
		 boost::shared_ptr< TA_Base_Bus::SoundManagerProxy >  m_soundManagerProxy ;

		bool m_bSound  ;
	

	};
}

#endif // !defined(AFX_SOUNDMANAGERTHREAD_H__CEC344BB_0328_46DA_BCFC_2FE2856D3DD8__INCLUDED_)
