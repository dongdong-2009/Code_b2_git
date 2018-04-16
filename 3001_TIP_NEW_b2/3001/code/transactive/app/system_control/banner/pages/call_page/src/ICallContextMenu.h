#pragma once


namespace TA_IRS_App
{
	class ICallContextMenu
	{

		public:
			~ICallContextMenu(void) {}
			virtual void show () = 0;
	};
}