

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/spid_manager/src/PidMessage.h $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * This abstract class represents a PID message of any type. It contains fields
  * common to all messages.
  */

#ifndef PIDMESSAGE_H
#define PIDMESSAGE_H
#include "StdAfx.h"
#include <string>
#include <vector>
//#include "app/signs/spid_manager/src/MessageDetails.h"
//#include "app/signs/spid_manager/src/Appearance.h"

/* This abstract class represents a PID message of any tipe. It contains fields common to all messages */
using namespace std;
namespace TA_IRS_Core
{
	class PidMessage
	{
	public:
		PidMessage();
		virtual ~PidMessage();
		PidMessage(const PidMessage& msg);
		PidMessage(long id, wstring strDescription, 
			wstring strContent
			/*EFontSize englishFontsize, 
			EFontSize chinesefontsize, EMessageSet set*/);
		/**
		  * operator=
		  * 
		  * Assignment operator.
		  * 
		  * @return PidMessage& 
		  * @param : const PidMessage& message
		  */
		PidMessage& operator=(const PidMessage& msg);

		// Returns the message's ID.
		int getId();

		// Returns the message's English description.
		wstring getMsgDescription();

		// Returns the message's Chinese description.
		wstring getMsgContent();

		// Returns which set the message belongs to.
		//EMessageSet getSet();

		// returns appearance
		//Appearance getAppearance();

		// returns graphic
		//std::string getGraphic();

		// Sets msg id
		void setId(int id);
	    
		// Sets a new English description for the message.
		void setMsgDescription(wstring msgDescription);

		// Sets a new Chinese description for the message.
		void setMsgContent(wstring msgContent);

		// sets msg set
		//void setSet(EMessageSet msgSet);
	    
		// sets appearance
		//void setAppearance(Appearance apr);

		// sets graphic
		//void setGraphic(std::string str);

		//EFontSize getEnglishFontsize();
		//EFontSize getChineseFontsize();
		//void setEnglishFontsize(EFontSize fontsize);
		//void setChineseFontsize(EFontSize fontsize);

		//added by hongran
		// Returns the message's English description.
		//CString getEnglishDescriptionWithoutImage();

		// Returns the message's Chinese description.
		//std::string getChineseDescriptionWithoutImage();
		//void replaceImageWithHolder();

	private:
		// The message's unique ID.
		int m_id;

		// Stores the message's English description 
		wstring m_strMsgDescription;
		//add by hongran
		//CString m_englishDescriptionWithoutImage;

		// Stores the message's Chinese description 
		wstring m_strMsgContent;
		//added by hongran
		//std::string m_chineseDescriptionWithoutImage;

		// Stores which set the message belongs to (global set, or private set).
		//EMessageSet m_set;

		//EFontSize   m_englishFontsize;
		//EFontSize   m_chineseFontsize;
		//Appearance  m_appearance;
		//std::string m_graphic;
		//std::vector<CString> imageNames;
	};
}
#endif
