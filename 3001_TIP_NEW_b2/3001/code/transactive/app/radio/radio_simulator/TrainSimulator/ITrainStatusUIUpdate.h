
#ifndef _I_TRAIN_STATUS_UI_UPDATE_H_
#define _I_TRAIN_STATUS_UI_UPDATE_H_
#include <string>

namespace TA_IRS_App
{
	class ITrainStatusUIUpdate
	{
	public:
		enum eTrainStatus
		{
			UI_STATUS1 = 0,
			UI_STATU5,
			UI_STATUS6
		};

		virtual void UITrainStatusUdpate(eTrainStatus trainStatus, 
			                            int trainID, 
										const std::string& status ) = 0;

	};
}
#endif