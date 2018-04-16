/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioPredefinedTextMessage.h $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#if !defined(AFX_CONFIGRADIOPREDEFINEDTEXTMESSAGE_H__C556E6E3_3919_4072_98E9_CDC90D3AEB06__INCLUDED_)
#define AFX_CONFIGRADIOPREDEFINEDTEXTMESSAGE_H__C556E6E3_3919_4072_98E9_CDC90D3AEB06__INCLUDED_


#include <string>
#include "core/data_access_interface/radio/src/IRadioPredefinedTextMessage.h"
#include "core/data_access_interface/radio/src/RadioPredefinedTextMessageHelper.h"


namespace TA_IRS_Core
{
    class ConfigRadioPredefinedTextMessage : public IRadioPredefinedTextMessage  
    {
    public:
        ConfigRadioPredefinedTextMessage();
	    ConfigRadioPredefinedTextMessage(const unsigned long key);
        ConfigRadioPredefinedTextMessage(const unsigned long row, TA_Base_Core::IData& data);
	    ConfigRadioPredefinedTextMessage(const unsigned long operatorid, const unsigned long messageKey, const std::string messageName, const std::string messageText);
	    virtual ~ConfigRadioPredefinedTextMessage();

        /** 
          * getType
          *
          * description
          * TD14312
          *
          * @return 
          */
		bool getType();
        /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey();

	    /**
	      * getOperator
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getOperator();

	    /**
	      * getShortName
	      * 
	      * @return std::string
	      */
	    virtual std::string getShortName();

	    /**
	      * getMessageText
	      * 
	      * @return std::string
	      */
	    virtual std::string getMessageText();

	    virtual void setOperator(unsigned long operatorId);

        virtual void setShortName(const std::string& shortName);

	    virtual void setMessageText(const std::string& messageText);
        /** 
          * getType
          *
          * description
          * TD14312
          *
          * @return 
          */
		void setType(bool bType);
        
        virtual void invalidate();
        virtual void save();
        
    private:
        // disabled methods
        ConfigRadioPredefinedTextMessage(const ConfigRadioPredefinedTextMessage&);
		ConfigRadioPredefinedTextMessage& operator=(const ConfigRadioPredefinedTextMessage&);

        RadioPredefinedTextMessageHelper m_radioHelper;
    };
}
#endif // !defined(AFX_CONFIGRADIOPREDEFINEDTEXTMESSAGE_H__C556E6E3_3919_4072_98E9_CDC90D3AEB06__INCLUDED_)
