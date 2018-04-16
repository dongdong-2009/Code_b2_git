/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsDssData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
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
