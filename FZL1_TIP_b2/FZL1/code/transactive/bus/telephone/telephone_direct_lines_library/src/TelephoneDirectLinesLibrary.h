#if !defined(TelephoneDirectLinesLibrary_231E9088_BC20_4256_99E5_5D88427D47CB__INCLUDED_)
#define TelephoneDirectLinesLibrary_231E9088_BC20_4256_99E5_5D88427D47CB__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibrary.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Responsible for managing the retrieval and updates of the Telephone Speed Dial
  * Button details. It will call on the TelephoneDirectLineAccessFactory for
  * interfacing to the database.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <vector>

#include "bus/telephone/telephone_direct_lines_library/src/TelephoneSpeedDialButtonDetails.h"

class TelephoneDirectLinesLibrary
{

public:
	TelephoneDirectLinesLibrary();
	virtual ~TelephoneDirectLinesLibrary();

public:
	/**
	  * getButtonDetails
	  * Returns the TelephoneSpeedDialButtonDetails for the specified button
	  * <description>
	  * 
	  * @return static TelephoneSpeedDialButtonDetails 
	  * @param : const std::string& sessionId
	  * @param : const std::string& SetName
	  * @param : const int buttonNo
	  * 
	  * @exception <exceptions> Optional
	  */
	static TelephoneSpeedDialButtonDetails getButtonDetails(const std::string& sessionId, const std::string& SetName, const int buttonNo);

	/**
	  * getButtonSet
	  * 
	  * Returns all TelephoneSpeedDialButtonDetails for the specified Speed Dial set
	  * 
	  * @return static std::vector<TelephoneSpeedDialButtonDetails> 
	  * @param : const std::string& sessionId
	  * @param : const std::string& SetName
	  * 
	  * @exception <exceptions> Optional
	  */
	//static std::vector<TelephoneSpeedDialButtonDetails> getButtonSet(const std::string& sessionId, const std::string& SetName);
    static void getButtonSet(std::vector<TelephoneSpeedDialButtonDetails>& btnDetails, const std::string& sessionId, const std::string& SetName);

	/**
	  * setButton
	  * 
      * Modify the data of the specified Telephone Speed Dial button in the
	  * database. Returns true if successfull and false if unsuccesful. This will also
	  * utilise the CommsMessageSender class to send a sendBroadcastCommsMessage with a
	  * message type of TelephoneSpeedDialButtonModifiedNotification.
	  * 
	  * @return static bool 
	  * @param : const std::string& sessionId
	  * @param : TelephoneSpeedDialButtonDetails& details
	  * 
	  * @throws TelephoneDirectLinesLibraryException 
      *         - if there are error connectingt to database
      *         - error occurs in executing SQL statement
	  */
	static bool setButton(const std::string& sessionId, TelephoneSpeedDialButtonDetails& details);

    static const std::string SETNAME_HOTLINES;
    static const std::string SETNAME_USER;
    static const std::string SETNAME_STATIONS;
    static const std::string SETNAME_OCR;
    static const std::string SETNAME_DCR;
    static const std::string SETNAME_OTHERS;
    static const std::string SETNAME_PCP;

    static const int  MAX_HOTLINES_BTN;
    static const int  MAX_TABS_BTN;

private:
    static long getOperatorKey(const std::string& sessionId, const std::string& SetName);
    static long getProfileKey(const std::string& sessionId, const std::string& SetName);

private:
    //static TelephoneSpeedDialButtonDetails btnDetails;
    static const std::string USER_SET_NAME;
    static unsigned long UNUSED_KEY_VALUE;
};
#endif // !defined(TelephoneDirectLinesLibrary_231E9088_BC20_4256_99E5_5D88427D47CB__INCLUDED_)
