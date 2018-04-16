#pragma once
#include<map>

#include "TVSSEventReader.h"
#include "TVSSProtocol.h"


namespace TA_IRS_App
{

	class TVSSTrainborneEventReader : public TVSSEventReader
	{
	public:
		TVSSTrainborneEventReader(boost::shared_ptr<TVSSEvent> event);
		virtual ~TVSSTrainborneEventReader(void);
	public :

	   /**
	    * Use to initialize Data mapping for Equipment Alarm Response
	    */
		void initializeTrainborneMap(void);

	   /**
        * Return the Data of the Trainborne Event Trigger based on the specified dataIndex
		* @param dataIndex
		* dataIndex: 1 , for Data1
		*            2 , for Data2
		*            3 , for Data3
		*           .. , for Data..
		*           nth , for Data_nth
		*/
		unsigned char getTrainborneData(int dataIndex);
	private:
		std::map<int,int> TrainborneData;
	};
} ;