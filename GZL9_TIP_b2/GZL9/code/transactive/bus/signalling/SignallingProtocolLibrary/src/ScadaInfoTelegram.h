/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\ScandaInfoTelegram.h$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/11 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to send data to ATS for the Scada Info
  */

#ifndef _SCADA_INFO_TELEGRAM_H_
#define _SCADA_INFO_TELEGRAM_H_
#include "bus/signalling/SignallingProtocolLibrary/src/Telegram.h"

namespace TA_IRS_Bus
{
	class ScadaInfoTelegram : public Telegram
	{

	public:
	/**
	 * ScadaInfoTelegram
	 * 
	 * Constructor to build the packet from incoming data.
	 * 
	 */
		ScadaInfoTelegram(ta_uint16 numberOfSections);

	 /**
	  * Destructor
	  */
		virtual ~ScadaInfoTelegram(){};

	  /**
	   * setSectionNumber
	   *
	   * Use to set the Section Number of the Scada Info
	   */
		void setSectionNumber(ta_uint16 sectionNumber);

	   /**
	    * setSectionId
		*
		* Use to set the Section Id of the Scada Info.
		*/
		void setSectionId(ta_uint16 sectionId);

	   /**
		* setPowerStatus
		*
		* Use to set the Power Status of the Scada Info
		*/
		void setPowerStatus(ta_uint8 powerStatus);

	   /**
		* getSectionNumber
		*
		* Gets the Section Number of the Scada Info
		*/
		ta_uint16 getSectionNumber() const;

	   /**
		* getSectionId
		*
		* Gets the Section Id of the Scada Info
		*/
		ta_uint16 getSectionId() const;

	   /**
	    * getPowerStatus
		*
		* Gets the Power Status of the Scada Info
		*/
		ta_uint8 getPowerStatus() const;

		/**
		 * setDataContent
		 */
		void setDataContent(const std::vector<ta_uint8>& dataContent);

	private:

		/**
		 * Disable copy and assignment constructor
		 */
		ScadaInfoTelegram (const ScadaInfoTelegram&);
		ScadaInfoTelegram& operator= (const ScadaInfoTelegram&);

		static const ta_uint8 MSG_ID_POSITION;
		static const ta_uint8 SECTION_NUMBER_POSITION;
		static const ta_uint8 SECTION_ID_POSITION;
		static const ta_uint8 POWER_STATUS_POSITION;
		static const ta_uint8 SCADA_INFO_TELEGRAM_SIZE;
		static const ta_uint8 SCADA_MSG_LENGTH;

		static const ta_uint8 SCADA_DATA_CONTENT_SIZE;
		static const ta_uint8 LEN_MSG_SECTION_NUMBER_SIZE;

		
	
	};
}
#endif