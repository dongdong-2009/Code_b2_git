/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/spid_manager/src/PidMessage.cpp $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * This abstract class represents a PID message of any type. It contains fields
  * common to all messages.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "PidMessage.h"
#include "core/utilities/src/FunctionEntry.h"
//#include "app/signs/spid_manager/src/PidMessage.h"
///@#include "core/utilities/src/DebugUtil.h"

const CString PIPE("|");
//enum
//{
//	INVALID_INDEX = -1,
//	HOLDER = 0x25BC
//};
namespace TA_IRS_Core
{
	PidMessage::PidMessage()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	PidMessage::PidMessage(const PidMessage& msg):
		m_id(msg.m_id),
		m_strMsgDescription(msg.m_strMsgDescription),
		m_strMsgContent(msg.m_strMsgContent)

	{
		FUNCTION_ENTRY("copy constructor");
		FUNCTION_EXIT;
	}

	PidMessage::~PidMessage()
	{
	//    FUNCTION_ENTRY("destructor");
		//FUNCTION_EXIT;
	}

	PidMessage& PidMessage::operator=(const PidMessage& msg)
	{
	  FUNCTION_ENTRY("operator=");
	    
		if (this != &msg)
		{    
			m_id = msg.m_id;
			m_strMsgDescription = msg.m_strMsgDescription;
			m_strMsgContent = msg.m_strMsgContent;
		}

		return *this;

		FUNCTION_EXIT;
	}

	PidMessage::PidMessage(long id, wstring description, wstring content):
		m_id(id),
		m_strMsgDescription(description),
		m_strMsgContent(content)
	    
	{
		FUNCTION_ENTRY("constructor");

		FUNCTION_EXIT;
	}

	int PidMessage::getId()
	{
		FUNCTION_ENTRY("getId");
		FUNCTION_EXIT;
		return m_id;
	}

	wstring PidMessage::getMsgContent()
	{   
		FUNCTION_ENTRY("getMsgContent");
		FUNCTION_EXIT;
		return m_strMsgContent;
	}

	wstring PidMessage::getMsgDescription()
	{
		FUNCTION_ENTRY("getMsgDescription");
		FUNCTION_EXIT;
		return m_strMsgDescription;
	}

	void PidMessage::setId(int id)
	{
		FUNCTION_ENTRY("setId");
		m_id = id;
		FUNCTION_EXIT;
	}

	void PidMessage::setMsgDescription(wstring description)
	{
		FUNCTION_ENTRY("setMsgDescription");
		m_strMsgDescription = description;
		//replaceImageWithHolder();    
		FUNCTION_EXIT;
	}

	void PidMessage::setMsgContent(wstring content)
	{
		FUNCTION_ENTRY("setMsgContent");
		m_strMsgContent = content;
		FUNCTION_EXIT;
	}
}