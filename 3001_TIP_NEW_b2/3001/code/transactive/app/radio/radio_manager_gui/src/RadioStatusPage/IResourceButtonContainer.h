/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/IResourceButtonContainer.h $
 * @author:  Chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Interface to facilitate resource button callbacks to parent container.
 */

#ifndef INTERFACE_RESOURCE_BUTTON_CONTAINER_H_INCLUDED
#define INTERFACE_RESOURCE_BUTTON_CONTAINER_H_INCLUDED

class IResourceButtonContainer
{

public:
	virtual void addTimerObserver(CWnd* observer) =0;
	virtual void removeTimerObserver(CWnd* observer) =0;

};
#endif