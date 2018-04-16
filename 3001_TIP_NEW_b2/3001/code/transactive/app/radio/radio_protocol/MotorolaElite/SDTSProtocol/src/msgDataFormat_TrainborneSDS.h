/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TrainborneSDS.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef MSGDATAFORMAT_TRAINBORNESDS_H
#define MSGDATAFORMAT_TRAINBORNESDS_H

#include <vector>

namespace TA_IRS_App {

	class msgDataFormat_TrainborneSDS  
	{
		public:
			msgDataFormat_TrainborneSDS();
			virtual ~msgDataFormat_TrainborneSDS();

			std::vector<unsigned char> getSDSMessage();
			
			void setSDSMessage(const std::vector<unsigned char>& data);
			
		private:
			// disable methods
			msgDataFormat_TrainborneSDS(const msgDataFormat_TrainborneSDS&);
			msgDataFormat_TrainborneSDS& operator=(const msgDataFormat_TrainborneSDS&);
			

		/* attributes */
		private:
			std::vector<unsigned char> sdsMsg;
	};
};

#endif
