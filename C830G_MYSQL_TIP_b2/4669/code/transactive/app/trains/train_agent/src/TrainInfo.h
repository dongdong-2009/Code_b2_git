/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainInfo.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the TrainInformation commands 
  *
  */

///////////////////////////////////////////////////////////
//  TrainInfo.h
//  Implementation of the Class TrainInfo
//  Created on:      26-Feb-2004 02:15:47 PM
///////////////////////////////////////////////////////////

#if !defined(TrainInfo_INCLUDED_)
#define TrainInfo_INCLUDED_

#include <string>
#include <vector>

#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"

namespace TA_IRS_App
{
	class ITrainManager;

    class TrainInfo : public ITrainEventReaderDelegator
	{
    public: 

        // to decouple corba/c++
        struct TrainDetailsNotification
        {
            /// For convenience
            TrainDetailsNotification(unsigned char trainID,unsigned long currentLocation,std::string serviceNumber,bool isMute,char track,std::string ITSINumber, bool isLocalisationValid, char atsValid);

            unsigned char trainID;
            unsigned long currentLocation;
            std::string serviceNumber;
            bool isMute;
			TrainTypes::Track track;
            std::string ITSINumber;
            bool isLocalisationValid;
			TrainTypes::AtsValid atsValid; /// is localisation info valid
        };

        typedef std::vector<TrainDetailsNotification> TrainDetailsList;

        enum EChangeType 
        {
            SelectionListAddition,
            SelectionListUpdate,
            SelectionListDeletion
        };

		public:
			TrainInfo(ITrainManager& callback, ITrainNotificationFacade& notifier);
			virtual ~TrainInfo();

		public:
            bool delegate(TrainEvent& event);

            //*** Servant implementation methods
               
			/// Only return the 'valid' trains
            TrainDetailsList getTrainList();

            std::string getTrainTSI(unsigned char trainID);

            bool isTrain(std::string & ITSI);

            std::vector<std::string> getTrainSelectionList();
            
            std::vector<unsigned char> getTrainSelection(std::string selectionName);

            bool saveTrainSelection(const std::string & selectionName, std::vector<unsigned char> trainList, const std::string & sessionID);

            void deleteTrainSelection(const std::string & selectionName, const std::string & sessionID);

            //*** Asynchronous-unsolicited messages

//            void onPAAudioSWCabToCab(TrainEvent &event, paAudioSWCabToCabEventReader * reader);


        private:

			ITrainManager& m_trainManager;
            ITrainNotificationFacade& m_notifier;

//             unsigned int m_timerISCS1;
    };

};
#endif // !defined(TrainInfo_INCLUDED_)
