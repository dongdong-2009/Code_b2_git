#ifndef _H_ITCPMESSAGE_PROCESSOR_
#define _H_ITCPMESSAGE_PROCESSOR_

namespace TA_IRS_App
{
	class ITCPMessageProcessor
	{
	public:
		virtual void HandleTVSSTrainBorneEvent(long trainID,const char*eventStr) = 0;
	};
}
#endif;
