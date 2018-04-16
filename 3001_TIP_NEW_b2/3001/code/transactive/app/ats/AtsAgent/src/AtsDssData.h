/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/AtsDssData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSDSSDATA_H
#define ATSDSSDATA_H

#include <string>

namespace TA_IRS_App
{
    struct AtsDssData
    {
		time_t		timestamp;
		long		dssEvent;
		std::string	dssInput;

		/**
		  * Constructor
		  *
		  */
		AtsDssData()
			: timestamp(0), dssEvent(0), dssInput("")
		{};
		
		AtsDssData( const AtsDssData& atsDssData)
		{
			timestamp = atsDssData.timestamp;
			dssEvent	= atsDssData.dssEvent;
			dssInput	= atsDssData.dssInput;
		};

        AtsDssData& operator=(const AtsDssData & rhs)
		{
			timestamp = rhs.timestamp;
			dssEvent  = rhs.dssEvent;
			dssInput  = rhs.dssInput;

			return *this;
		};

		bool operator == (const AtsDssData & rhs)
		{
			return (timestamp == rhs.timestamp) && 
				   (dssEvent == rhs.dssEvent) && (dssInput == rhs.dssInput);
		};
	};
}

#endif // !defined(ATSDSSDATA_H)
