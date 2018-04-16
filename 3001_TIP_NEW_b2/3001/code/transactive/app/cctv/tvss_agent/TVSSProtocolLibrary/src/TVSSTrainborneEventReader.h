#pragma once
#include<vector>

#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"


namespace TA_IRS_App
{

	class TVSSTrainborneEventReader : public TVSSEventReader
	{
	public:
		TVSSTrainborneEventReader(boost::shared_ptr<TVSSEvent> event);
		virtual ~TVSSTrainborneEventReader(void);
	public :

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

		/**
	    * Return the train ID according to the event data
		*/
		unsigned long getTrainID();
	   /**
	    * Return the data lenght of the Trainborne
		*/
		int getTrainborneDataLength();
	private:
		std::vector<int> TrainborneData;
	};
} ;