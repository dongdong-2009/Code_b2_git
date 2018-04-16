#ifndef SDH_SIMULATOR_H_
#define SDH_SIMULATOR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioSimulator.h $
  * @author:  LIU YU
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  * 
  * Declaration file for the RadioSimulator class
  */

#include <vector>

#include "bus/generic_simulator/src/ISimulationModule.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/RadioCommsObserver.h"
//#include "SimulatorUseCase.h"
#include "app/radio/radio_simulator/TrainSimulator/SimulatorUseCase.h"

#include "core/threads/src/Thread.h"
#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"

#include "app/radio/radio_simulator/MotorolaProtocolLib/src/MotorolaSDSProtocolHelper.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/RadioDispatch.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/PACommandProcessor.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/PECCommandProcessor.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TTISCommandProcessor.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/EndOfTtisDownload.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/ChangeoverStatus.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TimsCommand.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/RequestOccCall.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/ChangeModeCommandProcessor.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TestCallCommand.h"

#include "app/radio/radio_simulator/CommandProcessorLib/src/HeartBeatCommandProcessor.h"

#include "app/radio/radio_simulator/TrainSimulator/IModelLogger.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "core/threads/src/IWorkItem.h"
#include "app/radio/radio_simulator/TrainSimulator/IRegStatus.h"

namespace TA_Base_Core
{
	class ThreadPoolSingletonManager;
}

namespace TA_IRS_App
{

	struct sTimsCmdData
	{
	public:
		CommandType CmdType;
		int Status;
		int TrainID;
		bool isPrimaryTsi;
	};
	
	/**
     * This is an interface called to process the incoming TimsCommand
     */
    class ITIMSTrainsProcessor
    {
    public:
        virtual void processTimsCommand( sTimsCmdData data ) = 0;
    };


    /**
      * RadioSimulator implements ISimulationModule, for use with the generic simulator.
      */
    class RadioSimulator
    : public virtual TA_Base_Core::Thread
    , public TA_Base_Bus::ISimulationModule
	, public ITIMSTrainsProcessor
	, public IRegStatus
    {

    public:

        //Construction & Destruction
        RadioSimulator(const std::string &name,const char* host,const char* port, TA_IRS_App::IModelLogger* modelLogger);
        virtual ~RadioSimulator();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                            //
        //                                Thread methods                                //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * run
          * 
          * The run method must be implemented in any class that inherits from TA_Core::Thread.
            *
          */
        void run();


        /**
          * terminate
          * 
          * The terminate method must be implemented in any class that inherits from TA_Core::Thread.
          *
          */
        void terminate();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                            //
        //                            ISimulationModule methods                        //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * startSim
          * 
          * startSim is called whenever the "start" command is issued by the simulater user.
          * Any initialisation that needs to be taken care of before the simulation starts 
          * for the first time is done in this method.
          *
          */
        void startSim();


        /**
          * stoptSim
          * 
          * stopSim is called whenever the "stop" command is issued by the simulater user.
          * Any processing that needs to be taken care of as the simulation finishes 
          * is done in this method.
          *
          */
        void stopSim();


        /**
          * restartSim
          * 
          * restartSim is called whenever the "start" command is issued by the simulater user.
          * Any initialisation that needs to be taken care of before the simulation restarts 
          * is done in this method.
          *
          */
        void restartSim();


        /**
          * destroySim
          * 
          * startSim is called whenever the "quit" command is issued by the simulater user.
          * Any processing that needs to be taken care of before the simulator is destroyed
          * should be completed in this method.
          *
          */
        void destroySim();    


        /**
          * getName
          * 
          * returns the name that the simulator module was instantiated with.
          *
          * @return std::string Name
          *
          */
        std::string getName();


        /**
          * getSimDescription
          * 
          * returns the description of the simulation module, for help messages.
          *
          * @return std::string simulatorDescription
          *
          */
        std::string getSimDescription();


        /**
          * getCommandProcessor
          * 
          * returns a pointer to the derived PASimulatorCommandProcessor
          *
          */
        TA_Base_Bus::ISimulationCommandProcessor& getCommandProcessor();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                                Execute methods                           //
        //   TODO replace with radio methods                                                                       //
        //////////////////////////////////////////////////////////////////////////////

		bool execPassnumber_check(bool bEnable);
		bool execAddISSI(const std::string& cmdValue);
		bool execListISSIPassNum();
		bool execListDeliveryStatusOptions();
		bool execDeliveryStatus(const std::string& cmdValue);
		bool execSetCmdata(const std::string& cmdValue);
		bool execSendCmdata();
		
        bool execLoadTable(std::string parameters);
        bool execClearTable();
        bool execShowTable(std::string params);
        bool execRun(std::string parameters);
		void simulatorRunUseCase(TA_IRS_Bus::SimulatorUseCase& uc);
		void execSendData(std::vector<unsigned char> &v);
		bool execShowMessage(const std::string& params);
		bool execSetTtsiVer(const std::string& params);

        /**
          * execStatus
          * 
          * outputs the current status of the VirtualVideoSwitch.  This method is 
          * executed when 
          *
          * @return bool successful True if the command is executed sucessfully.
          *
          */
        bool execStatus();

	

		MotorolaSDSProtocolHelper* getInstanceMotorolaHelper();

		TTISCommandProcessor* getTTISCommandProcessor();
		
		int getNumberOfConnection() const;
	
		typedef std::vector<TA_IRS_App::HeartBeatCommandProcessor* > HeartBeatProcessorData;



        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                        Simulator public methods                          //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

		 HeartBeatInfo getTrainHeartBeatInfo();

		 void addTrainHeartBeat(int trainID, int delay);

		 int getCommsObserverStatus();

		 void sendStatusTrainHeartBeat(TrainStatus stat, int trainID);

		 std::vector<TA_IRS_App::HeartBeatCommandProcessor*>  getHeartBeatProcessor() const;

		 void addTrainResponseStrategy(const TrainStrategyInfo& info);

		 std::map<int, TrainStrategyInfo> getTrainStrategyMap();

		 void removeTrainResponseStrategy(int trainID);

		 bool sendTIMSCommand(CommandType cmdType, int status, const std::vector<int>& trains,bool bIsPrimary);

		 TimsCommand* getTimsCommandHandler(CommandType cmdType);

		 void processTimsCommand( sTimsCmdData data );

		 //For IRegStatus
		 void updateRegStatus(bool status);

		 

    
    private:

        //////////////////////////////////////////////////////////////////////////////
        //                                                                            //
        //                         Simulator private methods                            //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * getCommsObserverVector
          * 
          * returns the vector of CommsObserver objects held by this class.  Each CommsObserver
          * monitors a connected client socket.
          *
          * @return std::vector< CommsObserver* >& commsObservers The vector of CommsObserver objects
          *                    held by the Simulator
          *
          */
        std::vector< TA_IRS_App::RadioCommsObserver* >& getCommsObserverVector();


        /**
          * addCommsObserver
          * 
          * adds the passed in CommsObserver to the vector of CommsObserver objects held by the 
          * simulator
          *
          * @param CommsObserver* com A pointer to the CommsObserver object to be added
          *
          */
        void addCommsObserver( TA_IRS_App::RadioCommsObserver* com); 


        /**
          * removeCommsObserver
          * 
          * removes the passed in CommsObserver from the vector of CommsObserver objects held by the 
          * simulator
          *
          * @param com A pointer to the CommsObserver object to be removed
          *
          */
        void removeCommsObserver( TA_IRS_App::RadioCommsObserver* com );

		/**
		 * initialize
		 *
		 * This method is used for initialization
		 */
		void initialize();

		void removeAllCommsObservers();

		void removeDisconnectedCommsObserver();

		void addHeartBeatProcessor(TA_IRS_App::HeartBeatCommandProcessor* cmdProcessor);
		void removeHeartBeatProcessor(TA_IRS_App::HeartBeatCommandProcessor* cmdProcessor);

		
	
		void initializeTrainHeartBeat();

		// Check if there is atleast one instance of RadioCommsObserver
		bool checkCommObserverStatus();

        // The name of this SimulationModule
        std::string m_name;

        // The SdhSimulationCommandProcessor
        TA_Base_Bus::ISimulationCommandProcessor *m_commandProcessor;

        bool m_threadIsRunning;
        bool m_doThreadTerminate;

        // indicates whether the socket is currently connected
        bool m_isConnected;

        // The port used by the socket. Defaults to 3001
        std::string m_socketPort;

        // The host used by the socket. Defaults to "localhost"
        std::string m_socketHost;

        // a vector which holds an observed socket for each connection that
        // is accepted.
        std::vector< TA_IRS_App::RadioCommsObserver* > m_commsObservers;

		MotorolaSDSProtocolHelper* m_motorolaSDSProtocolHelper;
		RadioDispatcher*           m_radioDispatcher;
		PECCommandProcessor*       m_pecCommandProcessor;
		PACommandProcessor*        m_paCommandProcessor;
		TTISCommandProcessor*      m_ttisCommandProcessor;
		ChangeModeCommandProcessor* m_changeCommandProcessor;

		EndOfttisDownload*         m_endOfTtisDownload;
		ChangeoverStatus*          m_changeoverStatus;
		RequestOccCall*            m_requestOccCall;
		TestCallCommand*           m_testCallCommand;

		IModelLogger* m_modelLogger;

		//HeartBeatProtocol
		std::vector<TA_IRS_App::HeartBeatCommandProcessor*>  m_HeartBeatProcessors;

		void stopSendingHeartBeat();
		void startSendingHeartBeat();
		void startSendingHeartBeat(TA_IRS_App::HeartBeatCommandProcessor* cmdProcessor);

		void checkHeartBeatStatus();
		void removeHeartBeatInfo(int trainID);

		HeartBeatInfo m_heartBeatInfo;

		TA_Base_Core::ReEntrantThreadLockable m_heartBeatStatusLock;

		TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;

		bool m_regStatus;


    };

	/**
     * Task to process incoming events simultaneously
     */
    class RadioSimTask : public TA_Base_Core::IWorkItem
    {
    public:

        RadioSimTask( ITIMSTrainsProcessor& callback,
                      sTimsCmdData data  );

        
        virtual ~RadioSimTask();


        virtual void executeWorkItem();

    private:

        RadioSimTask();

        ITIMSTrainsProcessor& m_callback;
        sTimsCmdData m_data;
		
    };

}//close namespace

#endif // SDH_SIMULATOR_H
