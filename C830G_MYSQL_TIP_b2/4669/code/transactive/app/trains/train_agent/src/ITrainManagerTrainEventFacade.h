/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/ITrainManagerTrainEventFacade.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This interface provides access to the sendTrainCommand
  *
  */

///////////////////////////////////////////////////////////
//  ITrainManager.h
//  Implementation of the Interface ITrainManager
//  Created on:      26-Nov-2003 02:54:15 PM
///////////////////////////////////////////////////////////

#if !defined(ITrainManagerTEF_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_)
#define ITrainManagerTEF_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_


namespace TA_IRS_App
{
	class TrainEvent;

	/**
	 * This interface provides access to onTrainEvent()
	 *
	 * Used to re-inject SDS Events for further processing.
	 */
	class ITrainManagerTrainEventFacade
	{
		public:
            virtual void onTrainEvent(TrainEvent& event) = 0;
    };
};
#endif // !defined(ITrainManagerTEF_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_)
