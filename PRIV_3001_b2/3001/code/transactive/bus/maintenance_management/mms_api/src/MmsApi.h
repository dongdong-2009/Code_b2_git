/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/maintenance_management/mms_api/src/MmsApi.h $
  * @author:  Wu Min Tao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * The class is the API wrapper for the BEA message Q, the assumption here is 
  * the server queue name must be 'MMS' and client queue name must be 'ISCS'
  */

#if !defined _MmsAgentApi
#define _MmsAgentApi

#ifdef _WINDOWS
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#else // not Windows NT
#include <string.h>
#endif // _WINDOWS

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <time.h>
#include <iostream>
#include <string>

//BEA message queue stuff
#include "p_entry.h" 
#include "p_return.h"
#include "p_symbol.h"

namespace TA_IRS_Bus
{
    #define MSTR(x,y) case (x) : (void) sprintf(symbuf, (y)); mstrc = symbuf; break

    //BEA msg format use for the agent
    typedef struct _Msg
    {
       char  *buffer;
       short size;
       char  mpriority;
       char  delivery;
       char  uma;
    } Msg;


    //BEA msg type format use for the agent
    typedef struct _MsgType
    {
       short spos;
       char  String[10];
       char  priority;
       char  delivery;
       char  uma;
    } MsgType;

    //BEA msg queue configuration
    typedef struct _QueueConfig
    {
	    std::string  name;
	    int queueid;
	    int groupid;
    }QueueConfig;

    class MmsApi
    {
    public:

	    /**
	      * getInstance
	      * 
	      * Return THE instance of the callback Singleton.
	      * 
	      * @return static MmsApi& 
	      * 
	      * @exception <exceptions> Optional
	      */
	    static MmsApi& getInstance();

	    /**
	      * removeInstance
	      * 
	      * Remove THE instance of the callback Singleton.
	      * 
	      * 
	      * @return static void 
	      * 
	      * @exception <exceptions> Optional
	      */
	    static void removeInstance();

	    /**
	      * exit_Q
	      * 
	      * method to exit the BEA message Q
	      * 
	      * @return int 
	      * 
	      * @exception <exceptions> Optional
	      */
	    int exit_Q();

	    //method to put the BEA message to a particular message queue
	    int put_Q(char *q_name, char *target_q_name, char *msg, char *msgType, short msg_len);

	    //method to get the BEA message from a particular message queue
	    int get_Q(char *q_name, char * msg, short len, int32 itimeout);

	    //method to get the error msg, which is returned from the message queue
	    char* getErrorMsg();

	    //method to get the error code, which is returned from the message queue
	    int32 getErrorCode();

	    //method to get the pending msg from the message queue
	    int getPendingMsgs(char *q_name);

	    //method to ping the server and get the status
	    //use this to get the network status of the server
    	bool pingServer(double maxpingsecs);

	    //update the server address
	    //must call this before ping server
	    void updateServerAddress(std::string& address) {m_serverAddress = address;}

	    //update the queue configuration
	    //must call this before put_Q, or get_Q
	    void updateQueueConfig(QueueConfig& config, bool isserver);

	    //getIniJournal status
	    bool isJournalOn();
    
	    //get the iniQ status
	    bool isInitQ();

    private:
        // constructor
	    MmsApi (); 
        // destructor
	    ~MmsApi(); 
	    /**
	      * initQ
	      * 
	      * initialize the queue
	      * 
	      * @return int 
	      * @param : char *qname
	      * 
	      * @exception <exceptions> Optional
	      */
	    int initQ(char *qname); 

	    /**
	      * locate_Q
	      * 
	      * locate a particular queue with the name
	      * 
	      * @return int 
	      * @param : char* q_name
	      * 
	      * @exception <exceptions> Optional
	      */
	    int locateQ(const char* q_name); 
	    /**
	      * ackQ
	      * 
	      * ack the queue
	      * 
	      * @return int 
	      * 
	      * @exception <exceptions> Optional
	      */
	    int ackQ(); 

	    int32 attach(char *q_name,q_address   *q_addr); // attach to a queue
	    int32 confirm(uint32 *msg_seq_number); // for confirming the last message received
	    int32 exit(); // detach from the queue
	    int32 get(q_address *q_addr, q_address *msg_source, char *msg_area, 
			    short msgLength,int32 timeout); // get message from the queue
	    int32 pendingMsgs(q_address   *q_addr, int32 *); // checking the no of msg in a queue
	    int32 put(q_address   *q_addr); // send message to target queue
	    void getDateTime(char* pRetBuf); // return the current date time in string
	    void get_sym_str(int32 sym); // convert error code to string message

		//This datamember is used to identify received datagrams as ones 
		//that we sent
		unsigned short m_pid;
        //the singleton object
	    static	MmsApi*	 m_singleton;
        //server address
	    std::string m_serverAddress;
        //server configuration
	    QueueConfig m_serverConfig;
        //client configuration
	    QueueConfig m_clientConfig;
        //member for clearing queue resource in the destructor
	    int			m_exit;	
        //member for message number count
        int			m_msgCount; 
        // Init Q status
        bool		m_initQ; 
        //Locate Q status
        int         m_locateQ; 
        //journal on of status
	    bool		m_JournalOn; 
        // for defining sending msg characteristic
        Msg			m_msgBag; 
        // client own queue address
        q_address   m_queueClient; 
        // server own queue address
        q_address   m_queueServer; 
        // for storing return address
        q_address   m_queueTemp; 
        // for storing last error msg
        char		m_errMessage[100]; 
        // for storing last error code
        int32		m_errCode;
        // acknowledging message and destroy
	    uint32		m_confirmSeq[2]; 

    public:
        static const std::string MMS_SERVER_QUEUE_NAME;
        static const std::string ISCS_CLIENT_QUEUE_NAME;
        static const int BEA_TRUE;
        static const int BEA_FALSE;
        static const int NUM_QUEUES;	
        static const int BUFSIZE;// use in for error message and error routine
        static const int MSGBUFSIZE;// use in put_q funtion

        static const int FIRST_PRIORITY;     
        static const int SECOND_PRIORITY;    
        static const int THIRD_PRIORITY;
        static const double MAX_PING_INTERVAL;
        static const int MAX_SEQUENCE_NUMBER; //TD15245 (Mintao++)
    };
}// TA_IRS_Bus

#endif



