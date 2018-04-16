/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/incident_manager/src/ColumnType.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  ColumnType.h
//  Implementation of the Class ColumnType
//  Created on:      16-May-2005 12:18:52 PM
//  Original author: San Teo
///////////////////////////////////////////////////////////

#if !defined(ColumnType_F6C3B9E7_0EE8_4f08_A05A_EA29A86F58DA__INCLUDED_)
#define ColumnType_F6C3B9E7_0EE8_4f08_A05A_EA29A86F58DA__INCLUDED_

/**
 * Suggested enumerated type to represent the columns that could possibly appear
 * in the incident list control.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:18:53 PM
 */
namespace TA_IRS_App
{
	enum ColumnType
	{
	/**
	* The incident ID
		*/	
		//TD15204 - reverting back the changes of TD15041
		ID,
		/**
		* The creation date
		*/
		CREATED,
		/**
		* The title of the incident
		*/
		TITLE,
		/**
		* The location where the incident occurred.
		*/
		LOCATION,
		/**
		* If it happened on the track, specifies whether it's inner or outer track.
		*/
		TRACK,
		/**
		* The train involved in the incident
		*/
		TRAIN,
		/**
		* The alarm associated with this incident
		*/
		ALARM,
		/**
		* The asset associated with this incident
		*/
		ASSET,
		/**
		* The subsystem invoved
		*/
		SUBSYSTEM,
		/**
		* The name of the operator who creates this incident
		*/
		CREATOR,
		/**
		* The profile used to create this incident
		*/
		PROFILE,
		/**
		* The date that this incident was closed
		*/
		CLOSED
	};
}// end TA_IRS_App
#endif // !defined(ColumnType_F6C3B9E7_0EE8_4f08_A05A_EA29A86F58DA__INCLUDED_)