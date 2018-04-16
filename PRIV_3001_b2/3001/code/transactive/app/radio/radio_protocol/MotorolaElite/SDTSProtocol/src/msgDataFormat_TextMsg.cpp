/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TextMsg.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TextMsg.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App {

    msgDataFormat_TextMsg::msgDataFormat_TextMsg() //: textCodingScheme(TEXTCODING_ISO_88591)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	msgDataFormat_TextMsg::~msgDataFormat_TextMsg()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	//unsigned short msgDataFormat_TextMsg::getTextCodingScheme()
	//{
	//	return textCodingScheme;
	//}

	//void msgDataFormat_TextMsg::setTextCodingScheme(unsigned short txtCode)
	//{
	//	textCodingScheme = txtCode;
	//}

	std::string msgDataFormat_TextMsg::getText()
	{
		return text;
	}

	void msgDataFormat_TextMsg::setText(const std::string& txtmsg )
	{
		text = txtmsg;
	}
}