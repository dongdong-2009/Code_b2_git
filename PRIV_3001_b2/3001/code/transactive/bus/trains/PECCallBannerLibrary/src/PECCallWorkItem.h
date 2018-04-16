/**
  * PECCallWorkItem.h
  */

#if !defined(PECCallWorkItem_H)
#define PECCallWorkItem_H

#include "core\threads\src\IWorkItem.h"
#include "PECCallEntry.h"

namespace TA_IRS_Bus
{
	struct PECCallActionType
	{
		enum EAction
		{
			NONE = 0,
			CCTV = 1,
			RESET = 2,
			RESETALL = 3,
			ONHOLD = 4
		};
	};

	class PECCallWorkItem : public TA_Base_Core::IWorkItem
    {

		public:

			PECCallWorkItem( PECCallEntry* CallEntry, PECCallActionType::EAction action);

			virtual ~PECCallWorkItem(){};
			/**
			 * executeWorkItem
			 *
			 * This method is used as a callback mechanism by the ThreadPoolSingleManager to notify
			 * the user that it is the time to execute its asynchronous operation.
			 *
			 */

			virtual void executeWorkItem();

	     private:
		   PECCallEntry* m_pCallEntry;

		   PECCallActionType::EAction m_action;
			
    };
}

#endif // !defined(PECCallWorkItem_H)

