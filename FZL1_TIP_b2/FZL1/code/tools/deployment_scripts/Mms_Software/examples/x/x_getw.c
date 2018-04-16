/*
** Copyright (c) 1994,1998 BEA Systems, Inc.
** All Rights Reserved.
**
** This software is furnished under a license and  may  be used
** and copied only in accordance with the terms of such license
** and with the inclusion of the above copyright notice.   This
** software or  any other copies thereof may not be provided or
** otherwise made available to any other person.  No  title  to
** and ownership of the software is hereby transferred.
**
** RESTRICTED RIGHTS LEGEND Use,  duplication, or disclosure by
** the U.S.  Government is subject to restrictions as set forth
** in Subparagraph (c)(1)(ii) of  DFARS 252.227-7013, or in FAR
** 52.227-19, as applicable.
**                                                                           
**    FILE:          x_getw.c
**
**    DESCRIPTION:   Illustrates using pams_get_msgw() to wait for a message
**                   to arrive. This will set an alarm to send messages to
**                   itself every 5 seconds and uses pams_get_msgw() to wait
**                   for the messages.
**
**    CREATION DATE: 10 January 1994
**
**    REQUIREMENTS:  The queue named "EXAMPLE_Q_1" must be defined in your
**                   init file as a primary queue.
*/

/**  .......................  **/
/**  C library include files  **/
/**  .......................  **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <unistd.h>
#endif

/**  ......................  **/
/**  MessageQ include files  **/
/**  ......................  **/
#include "p_entry.h"
#include "p_return.h"
#include "p_symbol.h"


/**  .......................  **/
/**    Local constants        **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  50


/**  .............................. **/
/**   Function Forward Declarations **/
/**  .............................. **/
int32   ExampleAttach(q_address*);
int32   ExampleGetWait(  );
void    ExampleAlarmHandler(int);
void    ExampleThread( );
int32   ExampleExit( );


/**  .......................  **/
/**         Globals           **/
/**  .......................  **/
q_address   my_primary_queue;


long
main( )
{
#ifdef WIN32
   DWORD tid;
#endif
   
   (void)printf( "Get Wait example starting...\n" );

   /*  Attach to MessageQ  */
   if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
       exit( 0 );

#ifndef WIN32
   /*  Set the alarm to go off in 5 seconds and send a message  */

#ifdef SIGALRM
   (void)signal( SIGALRM, ExampleAlarmHandler );
   (void)alarm( 5 );
#endif
#else
   /*
   ** Windows NT does not have the "alarm" function:
   ** Start a second thread to send a message to the main thread
   ** in 5 seconds.
   */
    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ExampleThread,0,0,&tid);
 
#endif

   /*  Wait to all the messages  */
   (void)ExampleGetWait();

   /*  Exit from MessageQ  */
   (void)ExampleExit();

   (void)printf( "Get Wait example complete.\n" );
   return( PAMS__SUCCESS );

} /*  end of main  */



int32
ExampleAttach(q_address   *q_addr)
{
   int32       attach_mode;
   int32       dmq_status;
   int32       q_name_len;
   int32       q_type;
   static char q_name[12];


   /*
   **  Attach by name to the primary queue "EXAMPLE_Q_1"
   */
   attach_mode    = PSYM_ATTACH_BY_NAME;
   q_type         = PSYM_ATTACH_PQ;
   (void)strcpy( q_name, "EXAMPLE_Q_1" );
   q_name_len     = (int32) strlen( q_name );
  
   dmq_status     = pams_attach_q(&attach_mode, q_addr, &q_type, q_name,
                           &q_name_len,
                           (int32 *) 0,    /*  Use default name space  */
                           (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "\nAttached to queue: \"%s\"\n", q_name );
   else
      (void)printf( "Error attaching to queue: \"%s\"; status returned is: %ld.\n", 
               q_name, dmq_status );

   return ( dmq_status );

} /*  end of ExampleAttach  */

#ifdef WIN32

void
ExampleThread()
{
   /*
   ** Each thread in a process has its own MessageQ
   ** context: to send a message to the main thread, this
   ** thread has to attach a queue of its own.
   */
 
   q_address   q_addr;
   int32       attach_mode;
   int32       dmq_status;
   int32       q_name_len;
   int32       q_type;
   char        q_name[2];
   int i;

   /*
   **  Attach to a temporay queue
   */
   attach_mode    = PSYM_ATTACH_TEMPORARY;
   q_type         = PSYM_ATTACH_PQ;
   (void)strcpy( q_name, "0" );
   q_name_len     = (int32) strlen( q_name );
  
   dmq_status     = pams_attach_q( &attach_mode, &q_addr, &q_type, q_name,
                           &q_name_len,
                           (int32 *) 0,    /*  Use default name space  */
                           (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "\nThread Attached to queue: \"%d\"\n", q_addr.au.queue );
   else {
      (void)printf( "Error attaching to queue: \"%s\"; status returned is: %ld.\n", 
               q_name, dmq_status );
      ExitThread(0);
   }
   for (i=0;i<10;i++) {
       ExampleAlarmHandler(0);    
       Sleep(5000);
   }
   pams_exit();
   ExitThread(0);
}
#endif
  
void
ExampleAlarmHandler(int the_signal)
{
   char        priority;
   char        delivery;
   short       msg_class;
   short       msg_type;
   short       msg_size;
   int32       dmq_status;
   int32       timeout;
   static int  cur_msg = -1;
   static char *MsgList[] = {
                        "first message",
                        "second message",
                        "third message",
                        "fourth message",
                        "fifth message",
                        "sixth message",
                        "seventh message",
                        "eighth message",
                        "ninth message",
                        "tenth message" };

   /* 
   **  Each time this function is invoked, a message is sent (pams_put_msg)
   **  from the MsgList into the queue "my_primary_queue".  The alarm is to
   **  call this function again.  When all of the messages in the list have
   **  been written, the alarm is NOT reset.
   */

   /*
   **  Check if all of the messages been written.
   **  If they have, then return from this function
   **  without sending another message and NOT
   **     setting the alarm.
   */

   the_signal = the_signal;
   if ( cur_msg >= 9 )
      return;

   /*  Put the current message in to the queue  */
   cur_msg++;              /* Count the messages being sent       */
   priority    = 0;        /* Regular priority; use 0, NOT '0'    */
   msg_class   = 0;        /* No class at the moment              */
   msg_type    = 0;        /* No type at the moment               */
   timeout     = 30;       /* Wait 3 seconds for a message        */

   /*
   **  Set the uma to:
   **  If can't deliver the message,
   **  DISCard and Log the message
   */

   /*
   **  Set the Delivery mode to:
   **  No Notification Memory and nonrecoverable
   */
   delivery        = PDEL_MODE_NN_MEM;  
   msg_size        = (short) strlen( MsgList[cur_msg] );
   dmq_status      = pams_put_msg(
                            MsgList[cur_msg],
                            &priority,
                            &my_primary_queue,        /* global variable */
                            &msg_class,
                            &msg_type,
                            &delivery,
                            &msg_size,
                            &timeout,
                            (struct PSB *) 0,
                            (char *) 0,
                            (q_address *) 0,
                            (int32 *) 0,
                            (char *) 0,
                            (char *) 0 );
    
   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "\n\tPut message:\t\"%s\"\n", MsgList[cur_msg] );
   else
      (void)printf( "\nError putting message:\t%s; status returned is: %ld.\n", 
               MsgList[cur_msg], dmq_status );

   /*  Reset the alarm to call this function */

#ifndef WIN32
#ifdef SIGALRM
   (void)signal( SIGALRM, ExampleAlarmHandler );
   (void)alarm( 5 );
#endif
#endif
} /*  end of ExampleAlarmHandler  */



int32
ExampleGetWait( )
{
   char        priority;
   int32       dmq_status;
   int32       timeout;
   char        msg_area[MAX_MESSAGE_SIZE];
   q_address   msg_source;
   short       msg_class;
   short       msg_type;
   short       msg_area_len;
   short       msg_data_len;

   /*
   **  Wait for messages to arrive in the queue, get them
   **  and then print them to the console.
   **  If we haven't received a message in 10 seconds (timeout),
   **  then return from this function.
   */

   (void)printf( "\n" );
   dmq_status     = PAMS__SUCCESS;
   priority       = 0;   
   msg_area_len   = MAX_MESSAGE_SIZE;
   timeout        = 100;  /* Set wait for 10 seconds for a message */

   while ( dmq_status == PAMS__SUCCESS ) {
      (void)printf( "\nWaiting for a message...\n" );

      /*  Initialize the message area to hold the message  */
      (void)memset((void*)&msg_area[0], '\0', MAX_MESSAGE_SIZE );

      /*  Get a message  */
      dmq_status = pams_get_msgw(msg_area, &priority, &msg_source, &msg_class,
                            &msg_type, &msg_area_len, &msg_data_len, &timeout,
                            (int32 *) 0, (struct PSB *) 0,
                            (struct show_buffer *) 0, (int32 *) 0,
                            (int32 *) 0, (int32 *) 0, (char *) 0 );

      switch ( dmq_status ) {
         case PAMS__SUCCESS :
            (void)printf( "\n\tGot message:\t\"%s\"\n", msg_area );
         break;

         case PAMS__TIMEOUT :
            (void)printf( "\nTimed out waiting for messages\n" );
         break;

         case PAMS__NOMOREMSG :
            (void)printf( "\nHow did I get here?\n" );
            /*  Status returned when queue is empty  */
         break;

         default :
            (void)printf( "\nError getting message; status returned is %ld.\n",
                     dmq_status );
         break;

      }/* end case */

   }/* end while */

   (void)printf("\n");
   return ( dmq_status );

} /* end of ExampleGetWait  */



int32
ExampleExit( )
{
   int32    dmq_status;

   dmq_status    = pams_exit();

   if ( dmq_status == PAMS__SUCCESS )
       (void)printf( "\nExited from MessageQ.\n" );
   else
       (void)printf( "\nError exiting; status returned is %ld.\n", dmq_status );

   return ( dmq_status );

} /*  end of ExampleExit  */
