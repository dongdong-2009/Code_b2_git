/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TextMsg.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef MSGDATAFORMAT_TEXTMSG_H
#define MSGDATAFORMAT_TEXTMSG_H

#include <string>

namespace TA_IRS_App {


	class msgDataFormat_TextMsg  
	{
		public:
			msgDataFormat_TextMsg();
			virtual ~msgDataFormat_TextMsg();

			// peter.wong, DTL-480, update format
			//unsigned short getTextCodingScheme();
			//void setTextCodingScheme(unsigned short txtCode);

			std::string getText();
			void setText(const std::string& txtmsg );

		private:
			// disable methods
			msgDataFormat_TextMsg(const msgDataFormat_TextMsg&);
			msgDataFormat_TextMsg& operator=(const msgDataFormat_TextMsg&);


		/* attributes */
 		private:
			//unsigned short textCodingScheme;
			std::string text;
	};

};
#endif
