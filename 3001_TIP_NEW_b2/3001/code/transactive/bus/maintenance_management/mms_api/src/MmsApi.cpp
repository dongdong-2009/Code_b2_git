/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/maintenance_management/mms_api/src/MmsApi.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/03/26 11:00:59 $
  * Last modified by:  $Author: qi.huang $
  *
  */

////////////////////////
//////// Class MmsApi ///
////////////////////////

#include "MmsApi.h"

#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "core/sockets/src/RawDatagramSocket.h"
#include "core/sockets/src/ICMPMessageCodec.h"

#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

using namespace std;

namespace TA_IRS_Bus
{
    MmsApi* MmsApi::m_singleton = NULL;
    const std::string MmsApi::MMS_SERVER_QUEUE_NAME = "MMS";
    const std::string MmsApi::ISCS_CLIENT_QUEUE_NAME = "ISCS";
    const int MmsApi::BEA_TRUE = 1;
    const int MmsApi::BEA_FALSE = 2;
    const int MmsApi::NUM_QUEUES = 1;	
    const int MmsApi::BUFSIZE = 100; 
    const int MmsApi::MSGBUFSIZE = 32767; 

    const int MmsApi::FIRST_PRIORITY = 9;
    const int MmsApi::SECOND_PRIORITY = 5;
    const int MmsApi::THIRD_PRIORITY = 1;
    //Mintao++
    //TD14344
    const double MmsApi::MAX_PING_INTERVAL = 3;
    //Mintao++
    //TD14344

    const int MmsApi::MAX_SEQUENCE_NUMBER = 1000; //TD15245 (Mintao++)

    const int SLEEP_AFTER_SEND_PING = 50;

    MmsApi& MmsApi::getInstance()
    {
	    FUNCTION_ENTRY("getInstance");
        if (m_singleton == NULL)
        {
            m_singleton = new MmsApi();
        }
        return *m_singleton;
	    FUNCTION_EXIT;

    }

    void MmsApi::removeInstance()
    {
	    FUNCTION_ENTRY("removeInstance");
        if (m_singleton != NULL)
        {
            delete m_singleton;
            m_singleton = NULL;
        }
	    FUNCTION_EXIT;
    }

    bool MmsApi::isJournalOn()
    {
	    FUNCTION_ENTRY("isJournalOn");

	    return m_JournalOn;

	    FUNCTION_EXIT;
    }

    bool MmsApi::isInitQ()
    {
	    return m_initQ;
    }

    bool MmsApi::pingServer(double maxpingsecs)
    {
        bool toReturn = false;

	    bool canRead = false;
	    bool canWrite = false;
	    //The socket that we use to send pings on
	    TA_Base_Core::RawDatagramSocket m_socket("icmp", true);

        if(!m_socket.open())
	    {
		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		    "ping socket can not open");
            return false;
        }

        //TD15841 Mintao++
        if (maxpingsecs <= 0)
        {
            maxpingsecs = MAX_PING_INTERVAL;
        }

        double current = 0;
	    double sentTime = 0;
        int count = 0;
        //Mintao++
        //TD14344
        //get the start ping time
	    time_t timeSent = 0;
	    time(&timeSent);
        sentTime = timeSent;
        //Mintao++
        //TD14344
    
        do {
            count ++;

	        //API REF:
		        // wait for io on only one socket.  One of read or write must be
		        // true otherwise function will return immediately with false.
		        // static bool waitForOneIO(SocketType *socket, bool read, bool write,
		        //					 bool &canRead, bool &canWrite,
		        //					 unsigned int timeMillis);
		        //
	        // wait in 200ms blocks so we can bail out on
	        // m_tellThreadToStop in a reasonably responsive manner.
	        //this call sets the canRead and canWrite
	        if (TA_Base_Core::SocketSet<TA_Base_Core::IDatagramSocket>::waitForOneIO(&m_socket,
			        true, true, canRead, canWrite, 200))
	        {
		        if (canWrite)
		        {
					//note that the address obj is modified by the send method
                    TA_Base_Core::InetAddress ina(m_serverAddress); 
			        //construct a datagram
			        TA_Base_Core::Datagram datagram;

			        //fill out packet structure so that it is ECHO, i.e. a "ping"
			        TA_Base_Core::ICMPMessageCodec::getInstance()->constructECHO(datagram, m_pid, count);

			        //if the sending of this datagram fails...
			        //note that the address obj is modified by the send method
			        if (!m_socket.send(ina, datagram))
			        {
				        const std::string& name = ina.getHostName();
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "failed to send to ping for %s", name.c_str());
			        }
                    else
                    {
                        TA_Base_Core::Thread::sleep( SLEEP_AFTER_SEND_PING );
                    }

		        } //end if (canWrite)

		        //if a ping responce has arrived...
		        if (canRead)
		        {
			        //These variables are populated by the receive method
			        TA_Base_Core::InetAddress outa;
			        TA_Base_Core::Datagram datagram;

			        //if we can read the responce from the socket...
			        if (m_socket.receive(outa, datagram))
			        {
				        unsigned short id = 0, index = 0;

				        //if we can get the data out of the package...
				        if (TA_Base_Core::ICMPMessageCodec::getInstance()->decodeECHO(datagram, id, index))
				        {

					        //if this is a datagram we sent...
					        if (id == m_pid && index <= count)
					        {							        
							    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							    "Successfully obtained the ping reply from server");
                                //get out of the while loop
                                toReturn = true;
                                break;                           
					        }
					        else
					        {
						        // an old or someone else's ping
						        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							        "decodeECHO() got bad id=%u or bad seq=%u", id, index);
					        }
				        }
				        else
				        {
					        // this isn't necessarily bad. It could be someone else's packet.
					        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "response failed checksum.");
				        }
			        }
			        else
			        {
				        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive() failed");
			        }
		        } //end : if(canRead)
	        } //end: if socket is ready to read or write

	        time_t now;
	        time(&now);
            current = now;
            
        }while (current - sentTime <= maxpingsecs);

        return toReturn;
    }
    
    MmsApi::MmsApi ():
    m_exit(BEA_FALSE),
    m_msgCount(0),
    m_initQ(false), // false in the beginning //Mintao++//TD14326
    m_locateQ(BEA_FALSE), 
    m_JournalOn(BEA_FALSE)
    {
	    FUNCTION_ENTRY("constructor");
        //initialized the server address
        m_serverAddress = "0.0.0.0";
        //initialized the server configuration 
        m_serverConfig.name = MMS_SERVER_QUEUE_NAME;
        m_serverConfig.groupid = 1;
        m_serverConfig.queueid = 1;
        //initialized the client configuration
        m_clientConfig.name = ISCS_CLIENT_QUEUE_NAME;
        m_serverConfig.groupid = 1;
        m_serverConfig.queueid = 2;

		#ifdef WIN32
			m_pid = _getpid();
		#else
			m_pid = getpid();
		#endif
        //Mintao++
        //in order to use the ping method here under windows, must do the following
	    //Init for socket (pinging) stuff:
	    #ifdef WIN32
		    try
		    {
			    WSAData winsockData;
			    WSAStartup( WINSOCK_VERSION, &winsockData);
		    }
		    catch(...)
		    {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "WSAStartup threw");
		    }

	    #endif	
            
	    FUNCTION_EXIT;
    }

    MmsApi::~MmsApi()
    {
	    FUNCTION_ENTRY("destructor");
	    // user forget to call Exit()
	    if (m_exit == BEA_FALSE) 
		    exit();
        //clean the socket (pinging) stuff
	    #ifdef WIN32
	    WSACleanup();
	    #endif
	    FUNCTION_EXIT;
    }

    void MmsApi::getDateTime(char *pretbuf)
    {
	    FUNCTION_ENTRY("getDateTime");
	    char tembuf[5] = {NULL};
	    char retbuf[25] = {NULL};
        struct tm *rettime;
        time_t long_time;
        time( &long_time );                /* Get time as long integer. */
        rettime = localtime( &long_time ); /* Convert to local time. */

	    sprintf(tembuf,"%02d", rettime->tm_mday);
	    strcat(retbuf, tembuf); // get day
	    strcat(retbuf, "/");
	    sprintf(tembuf,"%02d", rettime->tm_mon + 1);
	    strcat(retbuf, tembuf);	// get month
	    strcat(retbuf, "/");
	    sprintf(tembuf,"%04d", rettime-> tm_year + 1900);
	    strcat(retbuf, tembuf); // get year
	    strcat(retbuf, "/");
	    sprintf(tembuf,"%02d", rettime->tm_hour);
	    strcat(retbuf, tembuf); // get hour
	    strcat(retbuf, ":");
	    sprintf(tembuf,"%02d", rettime->tm_min);
	    strcat(retbuf, tembuf); // get minute
	    strcat(retbuf, ":");
	    sprintf(tembuf,"%02d", rettime->tm_sec);
	    strcat(retbuf, tembuf); // get day

	    strcpy(pretbuf,retbuf);
	    FUNCTION_EXIT;
	    return;
    }
	    
    int32 MmsApi::confirm(uint32 *msg_seq_number)
    {
       FUNCTION_ENTRY("confirm");
       char     force_j;
       int32    dmq_status;
       int32    pcj_status;

       /*
       **  Confirm that the message <msg_seq_number> was received 
       */
       pcj_status = 0;             /* Arbitrary value  */
       force_j    = PDEL_NO_JRN;   /* Don't journal */
  
       dmq_status = pams_confirm_msg(
                         msg_seq_number,
                         &pcj_status,
                         &force_j );

	    switch ( dmq_status ) 
	    {
            case PAMS__SUCCESS :
			    ;
		    break;

            default :
			    get_sym_str(dmq_status);
		    break;
	    }/*end switch */
       FUNCTION_EXIT;
       return ( dmq_status );
    } 


    int32 MmsApi::exit()
    {
       FUNCTION_ENTRY("exit");
       int32    dmq_status;

       dmq_status = pams_exit();

	    switch ( dmq_status ) 
	    {
            case PAMS__SUCCESS :
				    ;
		    break;

            default :
			    get_sym_str(dmq_status);
		    break;
	    }/*end switch */

       FUNCTION_EXIT;
       return dmq_status;
    } /*  end of Exit  */


    int32 MmsApi::attach(char *q_name,q_address   *q_addr)
    {
       FUNCTION_ENTRY("attach");
       int32     attach_mode;
       int32     dmq_status;
       int32    q_name_len;
       int32     q_type;
       int32     attach_timeout=100;

	    // convert to integer
	    int iNum = atoi(q_name);

	    char buffer[20];
	    // convert back to character
        /*  Initialize the buffer area  */
        (void)memset((void*)&buffer[0], '\0', 20 );
	    sprintf(buffer,"%d", iNum);

    /* Attach our client process to a permanent, primary queue. */

       if (strcmp( q_name, buffer ) != 0) // Both string must be the same for number
       {
          attach_mode = PSYM_ATTACH_BY_NAME;      /* user passed us a name */
       }
		    
       else 
       {
          attach_mode = PSYM_ATTACH_BY_NUMBER;    /* user passed us an address */
       }

       q_type      = PSYM_ATTACH_PQ; // primary queue
       q_name_len  = (int32) strlen( q_name );

    // trim out spaces at the beginning of queue name
	    while ( ( q_name_len > 0 ) && (q_name[0] == ' ') )
	    {
		    q_name++;
		    q_name_len--;
	    }
	    
    // trim out spaces at the end of queue name
	    while ( ( q_name_len > 0 ) && (q_name[q_name_len-1] == ' ' ) )
	    {
    //	q_name[q_name_len-1] = '\0';
		    q_name_len--;
	    }

       dmq_status  = pams_attach_q(
                         &attach_mode,
                         q_addr,
                         &q_type,
                         q_name,
                         &q_name_len,
                         (int32 *) 0,    /*  Use default name space  */
                         (int32 *) 0,
                         &attach_timeout,
                         (char *) 0,
                         (char *) 0 );

	    switch ( dmq_status ) 
	    {
            case PAMS__SUCCESS :
			    ;
            break;

		    case PAMS__JOURNAL_ON :
			    get_sym_str(dmq_status);
		    break;
   
            default :
			    get_sym_str(dmq_status);
            break;
	    }/*end switch */

       FUNCTION_EXIT;
       return ( dmq_status );
    } 

    int32 MmsApi::get(q_address *q_addr, q_address *msg_source, char *msg_area, 
				    short msgLength, int32 timeout)
    {
       FUNCTION_ENTRY("get");
       char        priority;
       short       msg_class;
       short       msg_type;
       short       msg_area_len;
       short       msg_data_len;
       int32       dmq_status;
       int32       sel_filter;
       struct PSB  get_psb;

       /*
       **  Get the messages currently in the queue
       */
       priority     = 0;                 /* get messages of any priority */
       sel_filter   = 0;                 /* select all messages */
       msg_area_len = msgLength;        //MAX_MESSAGE_SIZE;  
       dmq_status   = PAMS__SUCCESS;


          /*  Initialize the message area to hold the message  */
          (void)memset((void*)&msg_area[0], '\0', msgLength );

	      /*  Get a message  */
          dmq_status = pams_get_msgw( msg_area, 
								    &priority, 
								    msg_source, 
								    &msg_class,
								    &msg_type, 
								    &msg_area_len, 
								    &msg_data_len, 
								    &timeout,
								    &sel_filter, 
								    &get_psb, 
								    (struct show_buffer *) 0,
								    (int32 *) 0, 
								    (int32 *) 0, 
								    (int32 *) 0, 
								    (char *) 0 );

          switch ( dmq_status ) {
             case PAMS__SUCCESS :
                /*  Need to check the PSB  */
                switch ( get_psb.del_psb_status ) {
                   case PAMS__CONFIRMREQ :
				      //acknowledging previously received message
				        m_confirmSeq[0] = get_psb.seq_number[0];
					    m_confirmSeq[1] = get_psb.seq_number[1];
                   break;
        
                   case PAMS__POSSDUPL :
				      //acknowledging previously received message
					    if ((m_confirmSeq[0] == get_psb.seq_number[0]) && 
						    (m_confirmSeq[1] == get_psb.seq_number[1]))
					    {
						    dmq_status = PAMS__POSSDUPL;
						    get_sym_str(dmq_status);
					    }
					    else
					    {
						    m_confirmSeq[0] = get_psb.seq_number[0];
						    m_confirmSeq[1] = get_psb.seq_number[1];
					    }
                   break;

                   case PAMS__SUCCESS :
					    ;
				    break;
         
                   default :
				       dmq_status = get_psb.del_psb_status;
				       get_sym_str(dmq_status);
                   break;
                }/*end inner switch */
             break;

             case PAMS__TIMEOUT :
			     get_sym_str(dmq_status);
             break;

             default :
			     get_sym_str(dmq_status);
		     break;
          }/*end outer switch */

       FUNCTION_EXIT;
       return ( dmq_status );
    } 
	    

    int32 MmsApi::pendingMsgs(q_address   *q_addr, int32 *status)
    {
       FUNCTION_ENTRY("pendingMsgs");
	    int32       dmq_status;
        int32       num_queues;
        short       q_list[NUM_QUEUES];
        int32       pend_list[NUM_QUEUES];

        /*
        **  Return the number of messages pending for the queue passed in
        */

        num_queues    = NUM_QUEUES;
        q_list[0] = q_addr->au.queue;

        dmq_status    = putil_show_pending(&num_queues, q_list, pend_list );
	    status = &dmq_status;

        if ( dmq_status != PAMS__SUCCESS ) {
		    get_sym_str(dmq_status);
            pend_list[0] = -1;
        }

       FUNCTION_EXIT;
       return ( pend_list[0] );

    } 


    int32 MmsApi::put(q_address   *q_addr)
    {
       FUNCTION_ENTRY("put");
       short       msg_class;
       short       msg_type;
       int32       dmq_status;
       int32       timeout;
       struct PSB  put_psb;

       /*
       **  Put the messages in MessageList to the queue passed in
       */
       msg_class = 1;
       msg_type = -100;     /* No class or type at the moment   */


       timeout   = 100;              /* Wait 10 seconds before giving up     */

        dmq_status  = pams_put_msg(
                       m_msgBag.buffer,
                       &m_msgBag.mpriority,
                       q_addr,        /* passed in */
                       &msg_class,
                       &msg_type,
                       &m_msgBag.delivery,
                       &m_msgBag.size,
                       &timeout,
                       &put_psb,
                       &m_msgBag.uma,
                       (q_address *) 0,
                       (int32 *) 0,
                       (char *) 0,
                       (char *) 0 );

          if ((dmq_status == PAMS__SUCCESS) || (dmq_status == PAMS__JOURNAL_ON))
          {
             /*  Check the psb for the real status  */
             switch ( put_psb.del_psb_status )
             {
                case PAMS__ENQUEUED :
                case PAMS__STORED :
                case PAMS__SUCCESS :
                case PAMS__UNATTACHEDQ :
                case PAMS__SAF_FORCED :
					    ;
			    break;
			    
                case PAMS__DQF_DEVICE_FAIL :
                case PAMS__DQF_FULL :
                case PAMS__MRS_RES_EXH :
                case PAMS__NO_DQF :
				    switch ( put_psb.uma_psb_status )
				    {
					    case PAMS__SAF_SUCCESS :
							    ;
					    break;

					    default :
						    get_sym_str(dmq_status);
						    dmq_status = put_psb.uma_psb_status;
				    }
			    break;

                default :
				    dmq_status = put_psb.del_psb_status;
				    get_sym_str(dmq_status);
                   break;
             }/*end Switch */
          }/*end if */
          else
	      {
		      get_sym_str(dmq_status);
	      }

        FUNCTION_EXIT;
	    return dmq_status;
    } 


    int MmsApi::exit_Q()
    {
       FUNCTION_ENTRY("exit_Q");
	    confirm((uint32*)m_confirmSeq);
	    if (exit() == PAMS__SUCCESS)
	    {
		    m_exit = BEA_TRUE;
		    m_initQ = false;
		    m_locateQ = BEA_FALSE;
		    m_JournalOn = false;
		    return BEA_TRUE;
	    }
	    else
	    {
		    return BEA_FALSE;
	    }
        FUNCTION_EXIT;
    }

    int MmsApi::ackQ()
    {
       FUNCTION_ENTRY("ackQ");
	    if (confirm((uint32*)m_confirmSeq) == PAMS__SUCCESS)
	    {
            FUNCTION_EXIT;
		    return BEA_TRUE;
	    }
	    else
	    {
            FUNCTION_EXIT;
		    return BEA_FALSE;
	    }
    }

    void MmsApi::updateQueueConfig(QueueConfig& config, bool isserver)
    {
       FUNCTION_ENTRY("updateQueueConfig");
	    if(isserver)
	    {
		    m_serverConfig = config;
            locateQ(MMS_SERVER_QUEUE_NAME.c_str());
	    }
	    else
	    {
		    m_clientConfig = config;
            locateQ(ISCS_CLIENT_QUEUE_NAME.c_str());
	    }
        FUNCTION_EXIT;
    }

    int MmsApi::locateQ(const char* q_name)
    {
       FUNCTION_ENTRY("locateQ");
       //this is a trick, we did not call the actual locateQ to locateQ, we get it from the configuration instead
	    int32 status=PAMS__NOOBJECT;

	    if (strcmp( q_name, m_serverConfig.name.c_str()) == 0)
	    {
		    m_queueServer.au.group = m_serverConfig.groupid;
		    m_queueServer.au.queue = m_serverConfig.queueid;
		    status = PAMS__SUCCESS;
	    }
	    else if (strcmp( q_name, m_clientConfig.name.c_str()) == 0)
	    {
			//Mintao++
			//TD15243
		    m_queueClient.au.group = m_clientConfig.groupid;
		    m_queueClient.au.queue = m_clientConfig.queueid;
			//Mintao++
			//TD15243
		    status = PAMS__SUCCESS;
	    }
	    else
	    {
		    get_sym_str(PAMS__NOOBJECT);
		    status = PAMS__NOOBJECT;
            FUNCTION_EXIT;
		    return BEA_FALSE;
	    }

	    if ( status == PAMS__SUCCESS)
	    {
		    m_locateQ = BEA_TRUE;
            FUNCTION_EXIT;
		    return BEA_TRUE;
	    }
	    else
	    {
            FUNCTION_EXIT;
		    return BEA_FALSE;
	    }
    }

    int MmsApi::initQ(char *qname)
    {
        FUNCTION_ENTRY("initQ");
	    int32 status = attach(qname, &m_queueClient);

	    // check journal is on
	    if ((status == PAMS__SUCCESS) || (status == PAMS__JOURNAL_ON))
	    {
		    if (status == PAMS__JOURNAL_ON)
		    {
			    m_JournalOn = true; //BEA_TRUE, is bool variable
		    }
		    else
		    {
			    m_JournalOn = false;
		    }		
		    m_initQ = true;//Mintao++//TD14326
            FUNCTION_EXIT;
		    return BEA_TRUE;
	    }
	    else 
	    {
		    m_JournalOn = false;
            FUNCTION_EXIT;
		    return BEA_FALSE;
	    }
    }
    

    int MmsApi::put_Q(char *q_name, char *target_q_name, char *msg, char *msgType, short msg_len)
    {
        FUNCTION_ENTRY("put_Q");
	    int len, header_length;
	    int32 status;
	    char StrCount[4]; // for holding the message id
	    char StrDateTime[25];
	    char strMsg[MSGBUFSIZE] = {NULL}; // holding msg to be send
	    char strTempMsgType[5] = {NULL};

	    if (m_initQ == false) // check if already attached to queue
	    {
		    if (initQ(q_name) == BEA_FALSE)
		    {
                FUNCTION_EXIT;
			    return BEA_FALSE;
		    }
	    }
	    
	    len = strlen(msgType);
	    if (len > 3)
	    {
		    get_sym_str(PAMS__BADARGLIST); // setting err code and msg
		    status = PAMS__BADARGLIST; // the return status
	    }
	    else
	    {
		    /* forming the header */
		    //the sender is ISCS, 4 characters
            strcpy(strMsg, ISCS_CLIENT_QUEUE_NAME.c_str());
            //the receiver is MMS, mus make it into 4 charaters
            //pad it with space
            std::string receiver = MMS_SERVER_QUEUE_NAME + " ";
		    strcat(strMsg, receiver.c_str());

		    // for test message type
		    if (strcmp( msgType, "TST") == 0) // Both string must be the same 
		    {												
			    strcat(strMsg, msgType );
			    m_msgBag.mpriority = FIRST_PRIORITY;
                // Block until the message is stored in the remote recovery journal.
			    m_msgBag.delivery = PDEL_MODE_WF_DQF;  
                // Discard - the message is deleted.
			    m_msgBag.uma = PDEL_UMA_DISC;
		    }
		    else // for alarm message type
			    if (strcmp( msgType, "ALA") == 0 )
				{
				    strcat(strMsg, msgType );
				    m_msgBag.mpriority = SECOND_PRIORITY;
                    // Block until the message is stored in the remote recovery journal.
				    m_msgBag.delivery = PDEL_MODE_WF_DQF;
                    // Store and forward - the message is written to the message recovery journal on the sender system.
				    m_msgBag.uma = PDEL_UMA_SAF;  
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"xxs test: already connected MQ server, begin to send alarm message");
			    }
			    else if (strcmp( msgType, "REQ") == 0 ) // Both string must be the same 
			    {
				    strcat(strMsg, msgType );
				    m_msgBag.mpriority = SECOND_PRIORITY;
                    // Block until the message is stored in the remote recovery journal. (Will use local journal file)
				    m_msgBag.delivery = PDEL_MODE_WF_DQF;
                    // Discard - the message is deleted.
					m_msgBag.uma = PDEL_UMA_DISC;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"xxs test: already connected MQ server, begin to send req message");
			    }
			    else // for other type of message
			    {
				    // pad the string with space if the string is less than 3
				    strcpy(strTempMsgType, msgType);

				    if (strTempMsgType[2] == '\0') // the str is less than 3 characters
				    {
					    strTempMsgType[2] = ' '; /*assign space*/
					    if (strTempMsgType[1] == '\0') // check this just in case the string is less than 2 char
						    strTempMsgType[1] = ' '; /*assign space*/
					    if (strTempMsgType[0] == '\0') // check this just in case the string is less than 1 char
						    strTempMsgType[0] = ' '; /*assign space worst case (impossible)*/
				    }
				    strcat(strMsg, strTempMsgType);
				    m_msgBag.mpriority = THIRD_PRIORITY;
                    // Block until the message is stored in the remote recovery journal.
				    m_msgBag.delivery = PDEL_MODE_WF_DQF;
                    // Store and forward - the message is written to the message recovery journal on the sender system.
				    m_msgBag.uma = PDEL_UMA_SAF;
			    }

			//TD15245 (Mintao++)
            std::ostringstream cntStr;
	        sprintf( StrCount,"%03d",m_msgCount); /*convert integer to string*/
            m_msgCount++; //auto increase the count number
	        
            if (m_msgCount >= MAX_SEQUENCE_NUMBER)	/*check if the count value is more MAX_SEQUENCE_NUMBER*/
		        m_msgCount = 0;		/*reset count to 0*/
			//TD15245 (Mintao++)

            cntStr << StrCount;
		    strcat(strMsg, cntStr.str().c_str());

		    getDateTime(StrDateTime);
		    strcat(strMsg, StrDateTime);
		    /* end of forming the header */

		    // header length
		    header_length = strlen( strMsg);

		    if (msg_len == 0)
			    strcat(strMsg, msg);
		    else  // append the whole msg even if there is 0 in the middle
			    memcpy( &strMsg[header_length], msg, msg_len);

		    m_msgBag.buffer = strMsg;

		    if (msg_len == 0)
			    m_msgBag.size = strlen(strMsg);
		    else
			    m_msgBag.size = msg_len + header_length ;

		    status = put(&m_queueServer);

		    if (status == PAMS__JOURNAL_ON)
		    {
			    m_JournalOn = true;
			    get_sym_str(status);
		    }
	    }
 	    
	    // determine the return value
	    if (status == PAMS__SUCCESS)
	    {
            FUNCTION_EXIT;
		    return BEA_TRUE;
	    }
	    else
	    {
            FUNCTION_EXIT;
		    return BEA_FALSE;
	    }
    }

    int MmsApi::get_Q(char *q_name, char * msg, short len, int32 itimeout)
    {
        FUNCTION_ENTRY("get_Q");
	    if (m_initQ == false) // if not attach to queue attach now
	    {
		    if (initQ(q_name) == BEA_FALSE)
		    {
                FUNCTION_EXIT;
			    return BEA_FALSE;
		    }
	    }


	    int32 status = get(&m_queueClient, &m_queueTemp, 
					    msg, len, itimeout); 

	    if (status == PAMS__SUCCESS)
	    {
		    char msgType[4];
		    int count = 0;

		    // retrieve message type
		    while ((count < 3) && (msg[8 + count] != ' '))
		    {
			    msgType[count] = msg[8 + count];
			    count ++;
		    }
		    msgType[count] = '\0';  // terminating string
		    
		    // check to see if it is "TST"
		    if (strcmp(msgType, "TST") == 0)
		    {
			    // response to test message
			    put_Q(q_name, "MMS", "TST", "ACK",3);
		    }
		    ackQ();
            FUNCTION_EXIT;
		    return BEA_TRUE;
	    }
	    else 
	    {
		    get_sym_str(status);
            FUNCTION_EXIT;
		    return BEA_FALSE;
	    }
    }

    int MmsApi::getPendingMsgs(char *q_name)
    {
        FUNCTION_ENTRY("getPendingMsgs");
	    int ret;
	    int32 status;

	    if (m_initQ == false)
	    {
		    ret = initQ(q_name);
		    if (ret == BEA_FALSE) {
			    return -1;
		    }
	    }

	    ret = pendingMsgs(&m_queueClient, &status);
	    if (ret == -1)
	    {
		    get_sym_str(status);
	    }

        FUNCTION_EXIT;
	    return ret;
    }

    char* MmsApi::getErrorMsg()
    {
        FUNCTION_ENTRY("getErrorMsg");
	    return m_errMessage;
        FUNCTION_EXIT;
    }

    int32 MmsApi::getErrorCode()
    {
        FUNCTION_ENTRY("getErrorCode");
	    return m_errCode;
        FUNCTION_EXIT;
    }

    void MmsApi::get_sym_str(int32 sym)
    {
        FUNCTION_ENTRY("get_sym_str");
	    char *mstrc;                   
	    char symbuf[BUFSIZE];

	    switch (sym) {
	    MSTR(PAMS__SUCCESS, "Successful completion of an action.");
	    MSTR(PAMS__DISC_SUCCESS, "Message not recoverable in DQF, message discarded.");
	    MSTR(PAMS__DISCL_SUCCESS, "Message not recoverable in DQF, msg discarded and logging ok.");
	    MSTR(PAMS__RESRCFAIL, "Failed to allocate resources.");
	    MSTR(PAMS__DLJ_SUCCESS, "Message not recoverable in DQF, message written to DLJ.");
	    MSTR(PAMS__DLQ_SUCCESS, "Message not recoverable in DQF, message queued to DLQ.");
	    MSTR(PAMS__RTS_SUCCESS, "Message not recoverable in DQF, message returned.");
	    MSTR(PAMS__SAF_SUCCESS, "Message not recoverable in DQF, msg recoverable from SAF file.");
	    MSTR(PAMS__NOMOREMSG, "No messages available.");
	    MSTR(PAMS__NOSEND, "PAMS__NOSEND");
	    MSTR(PAMS__WAKEFAIL, "PAMS__WAKEFAIL");
	    MSTR(PAMS__TIMERACT, "PAMS__TIMERACT");
	    MSTR(PAMS__MSGACT, "PAMS__MSGACT");
	    MSTR(PAMS__NO_UMA, "Message is recoverable; UMA not executed.");
	    MSTR(PAMS__UMA_NA, "UMA not applicable.");
	    MSTR(PAMS__TRACEBACK, "PAMS__TRACEBACK");
	    MSTR(PAMS__STORED, "Message is recoverable.");
	    MSTR(PAMS__ENQUEUED, "Message is recoverable.");
	    MSTR(PAMS__UNATTACHEDQ, "Message successfully sent to unattached queue.");
	    MSTR(PAMS__CONFIRMREQ, "Confirmation required for this message.");
	    MSTR(PAMS__PROPAGATE, "PAMS__PROPAGATE");
	    MSTR(PAMS__ABORT, "PAMS__ABORT");
	    MSTR(PAMS__BADDECLARE, "Queue has already been attached to this application.");
	    MSTR(PAMS__BADFREE, "PAMS__BADFREE");
	    MSTR(PAMS__TIMEOUT, "The timeout period specified has expired.");
	    MSTR(PAMS__ACKTMO, "PAMS__ACKTMO");
	    MSTR(PAMS__MSGUNDEL, "Message returned is undeliverable.");
	    MSTR(PAMS__EX_Q_LEN, "PAMS__EX_Q_LEN");
	    MSTR(PAMS__POSSDUPL, "Message is a possible duplicate.");
	    MSTR(PAMS__STUB, "PAMS__STUB");
	    MSTR(PAMS__SENDER_TMO_EXPIRED, "PAMS__SENDER_TMO_EXPIRED");
	    MSTR(PAMS__MRQTBLFULL, "PAMS__MRQTBLFULL");
	    MSTR(PAMS__NOTALLOCATE, "PAMS__NOTALLOCATE");
	    MSTR(PAMS__BIGBLKSIZE, "PAMS__BIGBLKSIZE");
	    MSTR(PAMS__MSGTOBIG, "Message exceeded the size of the largest link list section (LLS).");
	    MSTR(PAMS__INVALIDID, "PAMS__INVALIDID");
	    MSTR(PAMS__INVFORMAT, "PAMS__INVFORMAT");
	    MSTR(PAMS__INVBUFFPTR, "PAMS__INVBUFFPTR");
	    MSTR(PAMS__INVALIDNUM, "PAMS__INVALIDNUM");
	    MSTR(PAMS__BIGMSG, "PAMS__BIGMSG");
	    MSTR(PAMS__MSGTOSMALL, "The msg_area_len argument must be positive or zero.");
	    MSTR(PAMS__AREATOSMALL, "Received message is larger than the user's message area.");
	    MSTR(PAMS__NOCANSEND, "PAMS__NOCANSEND");
	    MSTR(PAMS__NOTACTIVE, "Target process is not currently active; message not sent.");
	    MSTR(PAMS__EXCEEDQUOTA, "Target process quota exceeded; message was not sent.");
	    MSTR(PAMS__BADPRIORITY, "Invalid priority value on send/receive operation.");
	    MSTR(PAMS__BADDELIVERY, "Invalid delivery mode.");
	    MSTR(PAMS__BADJOURNAL, "PAMS__BADJOURNAL");
	    MSTR(PAMS__BADPROCNUM, "Queue number out of range.");
	    MSTR(PAMS__BADTMPPROC, "Invalid temporary queue number.");
	    MSTR(PAMS__BADSYNCHNUM, "PAMS__BADSYNCHNUM");
	    MSTR(PAMS__BADTMPSYNCH, "PAMS__BADTMPSYNCH");
	    MSTR(PAMS__BADRECEIVE, "PAMS__BADRECEIVE");
	    MSTR(PAMS__BADTIME, "PAMS__BADTIME");
	    MSTR(PAMS__NOTDCL, "Process has not been attached to MessageQ");
	    MSTR(PAMS__STATECHANGE, "PAMS__STATECHANGE");
	    MSTR(PAMS__INVUCBCNTRL, "PAMS__INVUCBCNTRL");
	    MSTR(PAMS__NOLINK, "PAMS__NOLINK");
	    MSTR(PAMS__CIRACT, "PAMS__CIRACT");
	    MSTR(PAMS__PROTOCOL, "PAMS__PROTOCOL");
	    MSTR(PAMS__COMMERR, "PAMS__COMMERR");
	    MSTR(PAMS__BADSELIDX, "Invalid or undefined selective receive index.");
	    MSTR(PAMS__IDXTBLFULL, "PAMS__IDXTBLFULL");
	    MSTR(PAMS__BADPARAM, "Invalid argument in the argument list.");
	    MSTR(PAMS__NOMRS, "MRS is not available.");
	    MSTR(PAMS__DISC_FAILED, "Message not recoverable in DQF, message could be not discarded.");
	    MSTR(PAMS__DISCL_FAILED, "Message not recoverable in DQF, log or 'msg discard' failure.");
	    MSTR(PAMS__DLJ_FAILED, "Message not recoverable in DQF, DLJ write operation failed.");
	    MSTR(PAMS__DLQ_FAILED, "Message not recoverable in DQF, msg could not be queued to DLQ.");
	    MSTR(PAMS__DQF_DEVICE_FAIL, "Message not recoverable; DQF I/O failed.");
	    MSTR(PAMS__INVUMA, "PAMS__INVUMA");
	    MSTR(PAMS__DQF_FULL, "Message not recoverable; DQF full.");
	    MSTR(PAMS__INVJH, "PAMS__INVJH");
	    MSTR(PAMS__LINK_DOWN, "PAMS__LINK_DOWN");
	    MSTR(PAMS__BADSEQ, "Journal sequence number is not known to the MRS.");
	    MSTR(PAMS__NOTJRN, "PAMS__NOTJRN");
	    MSTR(PAMS__MRS_RES_EXH, "Message not recoverable; MRS resource exhaustion.");
	    MSTR(PAMS__NOMOREJH, "PAMS__NOMOREJH");
	    MSTR(PAMS__REJECTED, "PAMS__REJECTED");
	    MSTR(PAMS__NOSUCHPCJ, "PAMS__NOSUCHPCJ");
	    MSTR(PAMS__UCBERROR, "PAMS__UCBERROR");
	    MSTR(PAMS__BADUMA, "Undeliverable message action (UMA) is invalid.");
	    MSTR(PAMS__BADRESPQ, "Response queue not owned by process.");
	    MSTR(PAMS__BADARGLIST, "Wrong number of call arguments.");
	    MSTR(PAMS__NO_DQF, "Message not recoverable; no DQF for target queue.");
	    MSTR(PAMS__NO_SAF, "PAMS__NO_SAF");
	    MSTR(PAMS__RTS_FAILED, "Msg not recoverable in DQF, msg could not be returned to sender.");
	    MSTR(PAMS__SAF_DEVICE_FAIL, "Message not recoverable; store and forward (SAF) I/O failed.");
	    MSTR(PAMS__SAF_FAILED, "Message not recoverable in DQF, SAF write operation failed.");
	    MSTR(PAMS__BADLOGIC, "PAMS__BADLOGIC");
	    MSTR(PAMS__SELRCVACT, "PAMS__SELRCVACT");
	    MSTR(PAMS__NOMRQRESRC, "Insufficient multireader queue resources to allow access.");
	    MSTR(PAMS__BADASSIGN, "PAMS__BADASSIGN");
	    MSTR(PAMS__LOGNAME, "PAMS__LOGNAME");
	    MSTR(PAMS__LOGNAME2, "PAMS__LOGNAME2");
	    MSTR(PAMS__LOGNAME3, "PAMS__LOGNAME3");
	    MSTR(PAMS__LOGNAME4, "PAMS__LOGNAME4");
	    MSTR(PAMS__LOGNAME5, "PAMS__LOGNAME5");
	    MSTR(PAMS__NOOPEN, "PAMS__NOOPEN");
	    MSTR(PAMS__BADSCRIPT, "PAMS__BADSCRIPT");
	    MSTR(PAMS__DECLARED, "The queue number is already attached to another application.");
	    MSTR(PAMS__EXHAUSTBLKS, "PAMS__EXHAUSTBLKS");
	    MSTR(PAMS__INTERNAL, "PAMS__INTERNAL");
	    MSTR(PAMS__QUECORRUPT, "PAMS__QUECORRUPT");
	    MSTR(PAMS__REMQUEFAIL, "Failed to properly dequeue a message buffer.");
	    MSTR(PAMS__INSQUEFAIL, "Failed to properly queue a message buffer.");
	    MSTR(PAMS__CREATEFAIL, "PAMS__CREATEFAIL");
	    MSTR(PAMS__DCLTMPFAIL, "PAMS__DCLTMPFAIL");
	    MSTR(PAMS__PAMSDOWN, "The specified MessageQ group is not running.");
	    MSTR(PAMS__BADASTPARM, "PAMS__BADASTPARM");
	    MSTR(PAMS__NOQUOTA, "PAMS__NOQUOTA");
	    MSTR(PAMS__NOTPRIMARYQ, "Queue name or number is not a primary queue.");
	    MSTR(PAMS__BADNAME, "The queue name contains illegal characters.");
	    MSTR(PAMS__NETERROR, "Network error resulted in a communications link abort.");
	    MSTR(PAMS__NOACCESS, "The address of the specified name is not accessible.");
	    MSTR(PAMS__NOOBJECT, "A bad default declaration in the group configuration file.");
	    MSTR(PAMS__PREVCALLBUSY , "Previous call to CLS has not been completed.");
	    MSTR(PAMS__BADQTYPE, "Invalid queue type.");
	    MSTR(PAMS__NOACL, "The queue access control file could not be found.");
	    MSTR(PAMS__NOTSECONDARYQ, "Queue name or number is not a secondary queue.");
	    MSTR(PAMS__BADHANDLE, "Invalid message handle.");
	    MSTR(PAMS__NOTSUPPORTED, "Combination of delivery mode and uma selected is not supported.");
	    MSTR(PAMS__STOPPED, "The requested queue has been stopped.");
	    MSTR(PAMS__BUFFEROVF, "The size of the show buffer specified is too small.");
	    MSTR(PAMS__STALE, "Resource is no longer valid and must be freed by the user.");
	    MSTR(PAMS__SAF_FORCED, "Message written to SAF file to maintain FIFO order.");
	    MSTR(PAMS__JOURNAL_ON, "Message is written to SAF file.");
	    MSTR(PAMS__NETNOLINK, "No Network Link.");
	    MSTR(PAMS__LINK_UP, "MRS has reestablished link.");
	 	    
	    default:
	       (void) sprintf(symbuf, "PAMS__UNEXPECTED - %ld", sym ); // PAMS RETURN VALUE
	       mstrc = symbuf;
	    }                 
	    m_errCode = sym;			// store the error code

	    strcpy(m_errMessage,mstrc); // store in a error buffer
        FUNCTION_EXIT;
	    return;
    } 

}//end of TA_IRS_Bus

