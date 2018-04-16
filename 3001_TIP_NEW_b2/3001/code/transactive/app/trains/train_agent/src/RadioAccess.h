/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/trains/train_agent/src/RadioAccess.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 15:10:25 $
 * Last modified by:  $Author: builder $
 * 
 * Handles all radio related functions like sending/receiving SDS and
 * making/ending calls.
 * 
 * Refer to the radio system design for how to actually implement this class.
 */
 
#if !defined(RADIOACCESS_H__A14117A0_F7EB_4cc2_9D61_2C57E66F0267__INCLUDED_)
#define RADIOACCESS_H__A14117A0_F7EB_4cc2_9D61_2C57E66F0267__INCLUDED_

#include <map>

#include "boost/shared_ptr.hpp"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/naming/src/NamedObject.h"

#include "bus/trains/TrainTransactionModel/src/ICommandSender.h"
#include "bus/trains/TrainTransactionModel/src/IResponseReceiver.h"
#include "bus/radio/radio_agent/IDL/src/RadioServerAgentIDL.h"
#include "bus/radio/radio_agent/IDL/src/RadioMFTAgentCommandIDL.h"





class RadioAccess : public TA_IRS_Bus::ICommandSender, 
                    public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
public:

    enum ERadioBroadcastType
    {
        ALL_TRAINS = 0,
        SINGLE_TRAIN = 1
    } ;

    /**
      * RadioAccess
      * 
      * Constructor
      * 
      * @return 
      * @param : unsigned long paLiveResourceId
      * @param : unsigned long radioInterval
      * 
      * @exception <exceptions> Optional
      */
	RadioAccess(std::map< int, unsigned long> paLiveResources, unsigned long radioInterval);


    /**
      * ~RadioAccess
      * 
      * Destructor
      * 
      * @return virtual 
      * 
      * @exception <exceptions> Optional
      */
	virtual ~RadioAccess();

    /**
      * sendData
      * 
      * Sends the SDS messages to the OCC Server Radio Agent using Radio Agent's sendSDS(...)
      * 
      * @return virtual void 
      * @param : const std::string& radioId
      * @param : std::vector<unsigned char>& data
      * 
      * @exception TrainTransmissionFailureException
      *            If the send fails for some reason
      */
	virtual void sendData(const std::string& radioId, const std::vector<unsigned char>& data);

    /**
      * receiveSpecialisedMessage
      * 
      * Receive a specialized corba message from OCC Server Radio Agent
      * 
      * @return virtual void 
      * @param : const TA_Base_Core::CommsMessageCorbaDef& message
      * 
      * @exception <exceptions> Optional
      */
    virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

    /**
      * registerForSDS
      * 
      * Sets a receiver to receive incoming SDS messages.
      * 
      * @return void 
      * @param : TA_IRS_Bus::IResponseReceiver& receiver
      * 
      * @exception <exceptions> Optional
      */
	void registerForSDS(TA_IRS_Bus::IResponseReceiver& receiver);

    /**
      * makePaLiveAnnouncementCall
      * 
      * Initiates the call for the PA Live announcement
      * Calls the assignResource(..) follow by selectResource(...) of the radio agent 
      * for the different MFT (operator MFT).
      * 
      * @return void 
      * @param : const std::string& operatorSessionId
      * @param : const std::string& talkgroupName
      * @param : const ERadioBroadcastType broadcastType
      * 
      * @exception TrainTransmissionFailureException if the call fails for some reason
      */
	void makePaLiveAnnouncementCall( const std::string& operatorSessionId, 
                                     //const unsigned long resourceId, 
									 const std::string& talkgroupName,
                                     const ERadioBroadcastType broadcastType = SINGLE_TRAIN);

    /**
      * endPaLiveAnnouncementCall
      * 
      * Ends the call for the PA Live announcement
      * Calls the deassignResource(..) follow by deselectResource(...) of the radio agent 
      * for the different MFT (operator MFT).
      * 
      * @return void 
      * @param : const std::string& operatorSessionId
      * @param : const std::string& talkgroupName
      * @param : const ERadioBroadcastType broadcastType
      * 
      * @exception TrainTransmissionFailureException if the call ending fails for some
      * reason
      */
	void endPaLiveAnnouncementCall( const std::string& operatorSessionId, 
                                    //const unsigned long resourceId, 
									const std::string& talkgroupName,
                                    const ERadioBroadcastType broadcastType = SINGLE_TRAIN);




    /**
      * getAllTrainsLivePaResourceId
      * 
      * Returns Resource ID to be used when doing LIVE PA Broadcast for all valid trains 
      * 
      * @return unsigned long 
      * 
      * @exception <exceptions> Optional
      */
    unsigned long getAllTrainsLivePaResourceId(int line);

    /**
      * receiveSDSData
      * 
      * pass SDS data to receiver
      * 
      * @return unsigned long 
      * 
      * @exception <exceptions> Optional
      */
    void receiveSDSData(const std::vector<unsigned char>& data, const std::string& radioId,const std::string& destRadio);

	//void makePecCall(const std::string& operatorSessionId,const unsigned long resourceId);
	void makePrivateCall(const std::string& operatorSessionId,const unsigned long resourceId);

	void endPecCall( const std::string& operatorSessionId,const unsigned long resourceId);
	
	void answerPrivateCall(const std::string& operatorSessionId,const unsigned long resourceId);

private:
    /**
      * RadioAccess
      * 
      * Disable Default constructor
      * 
      * @return 
      * 
      * @exception <exceptions> Optional
      */
	RadioAccess();

    /**
      * RadioAccess
      * 
      * Disable Copy Constructor
      * 
      * @return 
      * @param : const RadioAccess&
      * 
      * @exception <exceptions> Optional
      */
    RadioAccess(const RadioAccess&);

    /**
      * operator=
      * 
      * Disable Assignment operator
      * 
      * @return RadioAccess& 
      * @param : const RadioAccess&
      * 
      * @exception <exceptions> Optional
      */
    RadioAccess& operator=(const RadioAccess&);


    /**
      * getRadioMFTAgentEntityName
      * 
      * Retrieves the name of the radio agent at the console (MFT)
      * 
      * @return std::string 
      * @param : std::string & sessionID
      * 
      * @exception <exceptions> Optional
      */
    std::string getRadioMFTAgentEntityName(const std::string & sessionID);


private:
	/**
	 * This is the resource ID used to make PA Live Announcements - Announcement group ID
	 */
	std::map< int, unsigned long> m_paLiveResources;

    /**
      * Interval to wait before train agent calls on radio agent again
      * Used mainly for making LIVE PA Announcement
      */
	unsigned long m_radioInterval;

    /**
      * Observer for the SDS events
      */
	TA_IRS_Bus::IResponseReceiver* m_sdsReceiver;

    /**
     * This is the radio agent object
     * this is typedef'd like
     */
    typedef TA_Base_Core::NamedObject<TA_IRS_Bus::RadioServerAgentIDL,
	                                  TA_IRS_Bus::RadioServerAgentIDL_ptr,
	                                  TA_IRS_Bus::RadioServerAgentIDL_var> RadioServerAgentNamedObject;
	 

    /**
     * This is the radio agent object
     * this is typedef'd like
     */
    typedef TA_Base_Core::NamedObject<TA_IRS_Bus::RadioMFTAgentCommandIDL,
	                                  TA_IRS_Bus::RadioMFTAgentCommandIDL_ptr,
	                                  TA_IRS_Bus::RadioMFTAgentCommandIDL_var> RadioMFTAgentNamedObject;
    /**
      * Server Radio Agent Object
      */
    RadioServerAgentNamedObject m_radioServerAgent;
    
};
#endif // !defined(RADIOACCESS_H__A14117A0_F7EB_4cc2_9D61_2C57E66F0267__INCLUDED_)
