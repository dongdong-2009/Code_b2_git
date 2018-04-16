/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/TelephoneManagerUserEvents.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Defines the set of user-defined messages for the TelephoneManager application.
  */

#if !defined(TelephoneManagerUserEvents_2BF70D10_3A47_4d51_8B39_DBC0A744C16C__INCLUDED_)
#define TelephoneManagerUserEvents_2BF70D10_3A47_4d51_8B39_DBC0A744C16C__INCLUDED_

enum TelephoneManagerUserEvents
{
	/**
	 * The message to send when a new telephone directory have been uploaded. When
	 * sending this message the WPARAM parameter will not be utilised.
	 */
	WM_DIRECTLINES_MODIFIED = WM_APP+1,
	/**
	 * The message to send when a new telephone directory have been uploaded. When
	 * sending this message the WPARAM parameter will not be utilised.
	 */
	WM_DIRECTORY_UPLOAD,
	/**
	 * The message to send when a new telephone directory entry have been added.  When
	 * sending this message the WPARAM parameter should contain a pointer to a
	 * TelephoneDirectoryEntryData object.
	 */
	WM_DIRECTORY_ENTRY_ADDED,
	/**
	 * The message to send when a telephone directory entry have been modified.  When
	 * sending this message the WPARAM parameter should contain a pointer to the
	 * TelephoneDirectoryEntryData object that will replace the existing object. The
	 * LPARAM parameter should contain a pointer to the exiting
	 * TelephoneDirectoryEntryData object being replaced.
	 */
	WM_DIRECTORY_ENTRY_MODIFIED,
	/**
	 * he message to send when a telephone directory entry have been deleted.  When
	 * sending this message the WPARAM parameter should contain a pointer to a
	 * TelephoneDirectoryEntryData object.
	 */
	WM_DIRECTORY_ENTRY_DELETED,
	/**
	 * The message to send when a new telephone subdirectory have been added.  When
	 * sending this message the WPARAM parameter should contain a pointer to a
	 * TelephoneDirectoryData object.
	 */
	WM_DIRECTORY_ADDED,
	/**
	 * The message to send when a telephone subdirectory have been modified.  When
	 * sending this message the WPARAM parameter should contain a pointer to a
	 * TelephoneDirectoryData object.
	 */
	WM_DIRECTORY_MODIFIED,
	/**
	 * The message to send when a telephone subdirectory have been deleted.  When
	 * sending this message the WPARAM parameter should contain a pointer to a
	 * TelephoneDirectoryData object.
	 */
	WM_DIRECTORY_DELETED
};
#endif // !defined(TelephoneManagerUserEvents_2BF70D10_3A47_4d51_8B39_DBC0A744C16C__INCLUDED_)
