/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/bus/trains/TrainSelectionComponent/src/TrainList.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is used by MFCTrainSelectorGUI to maintain a list of trains.
  * it is used so that when new windows register they get the current list given
  * to them.
  *
  * This can be used as an in or outbound list.
  *
  */

#if !defined(TRAIN_LIST_H)
#define TRAIN_LIST_H

#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <map>


namespace TA_IRS_Bus
{

    class TrainList
    {

    public:


        /** 
          * TrainList
          *
          * Constructor.
          */
        TrainList();

        
        virtual ~TrainList(){};
        

		/**
		* Updates the list of trains
		* 
		* @param action    the action to take (see EAction definition).
		* @param trainDetails    the new details of the train if this is an Add or Update.
		*/
        virtual void updateList( ITrainSelectorGUI::EAction action, const TrainInformationTypes::TrainDetails& trainDetails );


        /**
		* gets the map of current trains
		* 
		* @return the trains in this object
		*/
        inline virtual const TrainInformationTypes::TrainDetailsMap getCurrentTrains()
		{
			return  m_trains;
        }
        
    private:


		/**
		 * the current list of trains
		 */
        TrainInformationTypes::TrainDetailsMap m_trains;


        // the lock for the train list
        TA_Base_Core::NonReEntrantThreadLockable m_listLock;

    };

} // TA_IRS_Bus

#endif // !defined(TRAIN_LIST_H)

