
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Data.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CMDATA_H
#define CMDATA_H

#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h"

namespace TA_IRS_App {

	class CM_Data : public CMCommon3  
	{
		public:
			CM_Data();
			virtual ~CM_Data();

			int getAreaSelection();
			void setAreaSelection(int area);

			void createCM_Data( const std::vector<unsigned char>& rawData);
			bool parseData( const std::vector<unsigned char>& rawData);

		protected:
			virtual void buildMsg();


		private:
			// disabled methods
			CM_Data(const CM_Data&);
			void operator=(const CM_Data&);


		/* attributes */
		private: 
			
			unsigned char areaSelection;


		public:

			static const int CM_DATA_AREA_INDEX;
			static const int CM_DATA_TL_LENGTH_INDEX;

	};
};

#endif
