
#ifndef _I_VIEW_H_
#define _I_VIEW_H_
#include <string>
#include "IController.h"
#include "GlobalDataTypes.h"
namespace TA_IRS_App
{
	class IView
	{
	public:
		virtual void addListener(IController* controller) = 0;
		virtual void updateUI(MessageInfo) = 0;
	};

}
#endif