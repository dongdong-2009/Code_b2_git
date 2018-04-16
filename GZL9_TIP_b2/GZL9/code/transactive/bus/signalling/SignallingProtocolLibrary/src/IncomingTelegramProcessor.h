/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/IncomingTelegramProcessor.h $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the Class IncomingTelegramProcessor
  * This is used to convert incoming data into the correct type of telegram object.
  */

#if !defined(IncomingTelegramProcessor_6350EE6A_7BA6_43f1_9062_DB89A2F20A88__INCLUDED_)
#define IncomingTelegramProcessor_6350EE6A_7BA6_43f1_9062_DB89A2F20A88__INCLUDED_

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/types/src/ta_types.h"
#include <boost/shared_ptr.hpp>

#include "bus/signalling/SignallingProtocolLibrary/src/Telegram.h"
#include "bus/signalling/SignallingProtocolLibrary/src/IDataPush.h"
#include "bus/signalling/SignallingProtocolLibrary/src/ITelegramFilter.h"
#include "core/threads/src/QueueProcessor.h"
#include <list>


namespace TA_Base_Core
{
	class ItaAlarms;
}

class ITelegramSender;
class ITrainLocationTelegramReceiver;

class IMessagePollReceiver;
class ITrainLocationReceiver;
class IDynamicTrainDataReceiver;
class IBlockInfoReceiver;
class ISignalStatusReceiver;
class ITrainPisInfoReceiver;

class IPlannedStartReceiver;
class IPlannedContentReceiver;
class IPlannedEndReceiver;

class IPracticalStartReceiver;
class IPracticalContentReceiver;
class IPracticalEndReceiver;

class IncomingTelegramProcessor : public TA_Base_Core::Thread
{

public:

	/**
	 * Constructor which takes the delegates for each type of incoming message.
	 * @param thirdRailReceiver
	 * @param radioReceiver
	 * @param pidsReceiver
	 * @param lessReceiver
	 * @param telegramSender    The object used to send error packets - ACKS with
	 * error flags set. This is only used if the packet cant be delivered to an
	 * interface.
	 * 
	 */
	//TD 15503
	//zhou yuan++
	//bug 731
	IncomingTelegramProcessor(
		TA_Base_Core::ItaAlarms& taAlarms,
 IMessagePollReceiver&   messagePollReceiver,
 ITrainLocationReceiver& trainLocation,
 IBlockInfoReceiver&             blockInfo,
 IDynamicTrainDataReceiver&      dynamicTrainData,
 ISignalStatusReceiver&          signalStatus,
 ITrainPisInfoReceiver&          trainPisInfo,
 IPlannedStartReceiver& plannedStart,
 IPlannedContentReceiver& plannedContent,
 IPlannedEndReceiver& plannedEnd,
 IPracticalStartReceiver& practicalStart,
 IPracticalContentReceiver& practicalContent,
 IPracticalEndReceiver& practicalEnd,
 ITelegramSender&            telegramSender);
	//++zhou yuan
	//bug 731

    /** 
      * Destructor
      */ 
	virtual ~IncomingTelegramProcessor();

	/**
	 * Given some data from a socket, attempt to create a telegram object, and
	 * delegate the telegram to the correct observer.
	 * Any number of full packets will be removed from the incoming data, the only
	 * data left in the incoming data will be for incomplete packets. This data will
	 * be re-attempted the next time some data is received.
	 * @param incomingData    The data from the socket. Used data will be removed,
	 * incomplete data will be left in the vector for next time around.
	 * 
	 */
	void processIncomingData(std::vector<unsigned char>& incomingData);

	/**
	 * the agent is in control mode
	 */
	void onAgentControl();

	/**
	 * The agent is in monitor mode
	 */
	void onAgentMonitor();

	/**
	 * Used to clear the Cache when there is a connection lost
	 */
	void connectionLost();
	
	///////////////////////  TA_Base_Core::Thread  ///////////////////////////////

	virtual void run();

	virtual void terminate();

private:

	/**
	 * Handles the Message Polling telegram, gives it to the Train Location observer.
	 * @param telegram
	 * 
	 */
	void processMessagePollingTelegram(const Telegram& telegram);
	
	/**
	 * Handles the Train Location telegram, gives it to the Train Location observer.
	 * @param telegram
	 * 
	 */
	void processTrainLocationTelegram(const Telegram& telegram);

	/**
	 * Handles the Block Info telegram, gives it to the Block Info observer.
	 * @param telegram
	 * 
	 */
	void processBlockInfoTelegram(const Telegram& telegram);

	/**
	 * Handles the Dynamic Train Data telegram, gives it to the Dynamic Train Data observer.
	 * @param telegram
	 * 
	 */
	void processDynamicTrainDataTelegram(const Telegram& telegram);

	/**
	 * Handles the Signal Status telegram, gives it to the Signal Status observer.
	 * @param telegram
	 * 
	 */
	void processSignalStatusTelegram(const Telegram& telegram);

	/**
	 * Handles the Train Pis Info telegram, gives it to the Train Pis Info observer.
	 * @param telegram
	 * 
	 */
	void processTrainPisInfoTelegram(const Telegram& telegram);


	void processPlannedTelegram(const Telegram& telegram);
	void processPracticalTelegram(const Telegram& telegram);

    
    //Disabled Methods
    /** 
      * Default Constructor
      */
	IncomingTelegramProcessor();

    /**
      * Copy Constructor
      */
    IncomingTelegramProcessor(const IncomingTelegramProcessor&);

    /**
      * Assignment Overloading
      */
    IncomingTelegramProcessor& operator=(const IncomingTelegramProcessor&);

private:
	//TD 15503
	//zhou yuan++
	//helper function
	void raiseInvalidMessageAlarm();

	std::string m_invalidAlarmId;
	//zhou yuan++

	void parseIncomingData();

	/**
	 * This is used to raise alarm, when the incoming data is invalid
	 */
	TA_Base_Core::ItaAlarms& m_taAlarms;

	/**
	 * This handles incoming Message Poll Telegram
	 */
	IMessagePollReceiver& m_messagePollReceiver;

	/**
	 * This handles incoming Train Location telegram
	 */
	ITrainLocationReceiver& m_trainLocationReceiver;

	/**
	 * This handles incoming Block Info telegram
	 */
	IBlockInfoReceiver& m_blockInfoReceiver;

	/**
	 * This handles incoming Dynamic Train Data telegram
	 */
	IDynamicTrainDataReceiver& m_dynamicTrainDataReceiver;

	/**
	 * This handles incoming Signal Status telegram
	 */
	ISignalStatusReceiver& m_signalStatusReceiver;

	/**
	 * This handles incoming Train Pis Info telegram
	 */
	ITrainPisInfoReceiver& m_trainPisInfoReceiver;

	/**
	 * This handles incoming Planned telegram start
	 */
	IPlannedStartReceiver& m_plannedStartReceiver;

	/**
	 * This handles incoming Planned telegram content
	 */
	IPlannedContentReceiver& m_plannedContentReceiver;

	/**
	 * This handles incoming Planned telegram end
	 */
	IPlannedEndReceiver& m_plannedEndReceiver;

	/**
	 * This handles incoming Planned telegram start
	 */
	IPracticalStartReceiver& m_practicalStartReceiver;

	/**
	 * This handles incoming Planned telegram content
	 */
	IPracticalContentReceiver& m_practicalContentReceiver;

	/**
	 * This handles incoming Planned telegram end
	 */
	IPracticalEndReceiver& m_practicalEndReceiver;
	
	/**
	 * Used to queue a telegram to send. This class only uses this to send back
	 * erroneous packets, ie packets that cant be delivered to an interface - e.g.
	 * InvalidInterface packets.
	 */
	ITelegramSender& m_telegramSender;

    std::map<Telegram::EInterfaceType, std::vector<boost::shared_ptr<Telegram> > > m_telegramMap;
	
	bool m_isRunning;

	TA_Base_Core::Semaphore m_semaphore;

	std::vector<unsigned char> m_incomingData;
	std::vector<unsigned char> m_incomingDataCache;

	TA_Base_Core::ReEntrantThreadLockable m_incomingDataLock;
	
	TA_IRS_Bus::ITelegramFilter * m_telegramFilter;

	/**
     * Flag to specify if agent is in control or monitor mode
     */ 
	volatile bool m_agentInControl;

	void proessSpecificTelegram (Telegram::ETelegramIdentification identification,const Telegram & initalTelegram);
	void processTelegram( std::list<MessageData>& messageData );
	void notifyTelegramProcessors();

	std::list<MessageData> MessageDataContainer;
	
};
#endif // !defined(IncomingTelegramProcessor_6350EE6A_7BA6_43f1_9062_DB89A2F20A88__INCLUDED_)