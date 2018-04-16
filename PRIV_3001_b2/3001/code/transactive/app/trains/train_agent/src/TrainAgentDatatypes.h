/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/trains/train_agent/src/TrainAgentDatatypes.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 15:10:25 $
 * Last modified by:  $Author: builder $
 * 
 * Used to hold the internal datatypes for CORBA decoupling.
 */

#if !defined(TRAINAGENTDATATYPES_H__8A38D06A_36CB_4751_9975_94AABF6D80BB__INCLUDED_)
#define TRAINAGENTDATATYPES_H__8A38D06A_36CB_4751_9975_94AABF6D80BB__INCLUDED_

#include <string>

class TrainAgentDatatypes
{

public:
	TrainAgentDatatypes();
	virtual ~TrainAgentDatatypes();

public:
	enum ELine
	{
		/**
		 * Represent the Depot Line number
		 */
		DepotLine = 0,
		/**
		 * Represent the Red Line number
		 */
		RedLine = 1,
		/**
		 * Represent the Orange Line number
		 */
		OrangeLine = 2
	};

	enum ETrack
	{

		/**
		  * Represent the train is up track
		  */
		UpTrack = 0,
		/**
		  * Represent the train is down track
		  */
		DownTrack = 1,
		/**
		  * Represent the train is on track
		  */
		NoDirection = 3
	};
    
	struct TrainInformation
	{

	public:
		/**
		 * The logical train Id
		 */
		unsigned char trainId;

		/**
		 * This is the train service/serial number.
		 */
		unsigned short serviceNumber;

		/**
		 * The location key of the train.
		 */
		unsigned long currentLocation;
		/**
		 * The line the train is on.
		 */
		ELine currentLine;

		/**
		 * This is the track the train is on.
		 */
		ETrack track;

		/**
		 * The radio number of the leading car
		 */
		std::string leadingCarRadioNumber;

		/**
		 * The radio number of the trailing car
		 */
		std::string trailingCarRadioNumber;

		/**
		 * The crew number
		 */
		unsigned short crewNumber;

		/**
		 * TSI/Radio number of the train.
		 */
		std::string radioId;

		/**
		 * Element name that the train is travelling on.
		 */
		std::string elementName;

		/**
		 * Element name that the train is travelling on.
		 */
		unsigned long regionId;


	};

	enum ETrainTalkgroupState
	{
        Depot = 0,
		Unknown = 2,
		Mainline = 1
	};

	struct TrainCommsStateUpdate
	{

	public:
		unsigned char trainId;
        
        // TD18758
        // ss++
		ETrainTalkgroupState talkgroup;

		unsigned long talkgroupId;
        // ++ss 

		/**
		 * If this is part of a PA Live announcement, this is the call Id of the broadcast.
         * Variable removed (Query Notes: Ripple_TrainAgent_QN_001.doc)
		 * unsigned long paLiveResourceId;
		 */

		/**
		 * This is used to group this train into a PA live broadcast. If this is not 0 the
		 * train is part of an existing PA Live announcement.
		 */
		unsigned long paLiveTransactionId;

	};

    struct TrainDmCarNumber
    {
    public:

	    /**
	      * operator==
	      * 
	      * Compares the two numbers, ignoring the order of the pair.
	      * 
	      * @return bool 
	      * @param : const TrainDmCarNumber& rhs
	      */
	    bool operator==(const TrainDmCarNumber& rhs) const;

    public:

        std::pair<std::string, std::string> leadingCarNumber;

        std::pair<std::string, std::string> trailingCarNumber;
    };

	enum EFreeTextColour
	{
		ColourBlack = 0,
		ColourRed = 0x40
	};

	enum EFreeTextEffect
	{
		EffectImmediate = 0x10,
		EffectScrollLeft = 0x80,
		EffectScrollDown = 0x90,
		EffectScrollUp = 0xA0
	};

	enum EFreeTextFont
	{
		FontEnglish7x5 = 0x10,
		FontEnglish16x8Chinese16x16 = 0x20,
		FontEnglish24x12Chinese32x32 = 0x30

	};

	enum ETrainErrorState
	{
		NoError = 0,
		InvalidLibrary = 1,
		TrainBusy = 2,
		TrainAccessDeniedException = 3,
		TrainTimeout = 4,
		InvalidParameter = 5,
		TrainGeneralFailure = 6
	};
    
	struct TrainErrorState
	{

		unsigned char trainId;
		ETrainErrorState state;
	};

	enum ELiveAnnouncementState
	{
		InProgress = 0,
		Completed = 1,
		Failed = 2
	};


};
#endif // !defined(TRAINAGENTDATATYPES_H__8A38D06A_36CB_4751_9975_94AABF6D80BB__INCLUDED_)
