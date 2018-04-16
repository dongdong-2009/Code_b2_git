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
**    FILE:           x_timer.c
**
**    DESCRIPTION:    Illustrates using the MessageQ timer calls -
**                    pams_set_timer() and pams_cancel_timer().
**                    This example will set a timer to go off every 5
**                    seconds. When the timer expires, it will send
**                    messages to itself. While not handling the timer
**                    event, it sits and waits for other incoming
**                    messages.  If this is interrupted, it will 
**                    cancel any outstanding timers.
**    
**    CREATION DATE:  10 January 1994
**
**    REQUIREMENTS:   The queue named "EXAMPLE_Q_1" must be defined in
**                    your init file as a primary queue.
**
*/


/**  .......................  **/
/**  C library include files  **/
/**  .......................  **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


/**  ......................  **/
/**  MessageQ include files  **/
/**  ......................  **/
#include "p_entry.h"
#include "p_return.h"
#include "p_symbol.h"
#include "p_typecl.h"


/**  .......................  **/
/**      Local constants      **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  50


/**  .......................  **/
/**    Function Prototypes    **/
/**  .......................  **/
int32 ExampleAttach(q_address*);
int32 ExampleSetTimer(int);
int32 ExampleGetWait( );
void  ExampleTimerHandler( );
int32 ExampleExit( );
void  ExampleInterruptHandler(int);

/**  .......................  **/
/**          Globals          **/
/**  .......................  **/
q_address   my_primary_queue;
int32       timer_id;
static int  cur_msg;

long
main( )
{
    /*  Handle interrupts  */
#ifdef SIGINT
    (void)signal(SIGINT, ExampleInterruptHandler);
#endif

    (void)printf( "\n\n\nTimer example starting...\n\n" );
    cur_msg  = -1;

    /*  Attach to MessageQ  */
    if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
        exit( 0 );

    /*  Set the timer to go off in 5 seconds and send a message  */
    (void)ExampleSetTimer( 5 );

    /*  Wait to all the messages  */
    (void)ExampleGetWait();

    /*  Exit from MessageQ  */
    (void)ExampleExit();

    (void)printf( "Timer example complete.\n" );

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
    attach_mode = PSYM_ATTACH_BY_NAME;
    q_type      = PSYM_ATTACH_PQ;
    (void)strcpy( q_name, "EXAMPLE_Q_1" );
    q_name_len  = (int32) strlen( q_name );
  
    dmq_status  = pams_attach_q(&attach_mode, q_addr, &q_type, q_name,
                       &q_name_len,
                       (int32 *) 0,    /*  Use default name space  */
                       (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\nAttached to queue: \"%s\"\n", q_name );
    else
        (void)printf( "\nError attaching to queue: \"%s\"; status returned is: %ld.\n", 
                  q_name, dmq_status );

    return ( dmq_status );

} /*  end of ExampleAttach  */

  
void
ExampleTimerHandler( )
{
    char         priority;
    char         delivery;
    short        msg_class;
    short        msg_type;
    short        msg_size;
    int32        dmq_status;
    int32        timeout;
    static char  *MsgList[] = {
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
    **  This function is invoked each time a timer message is received. It
    **  puts two messages into the queue "my_primary_queue": the next message
    **  from the MsgList, and a timer message. Once all the messages in the list
    **  have been written, don't send any messages.
    */

    /*  Have all the messages been written? */
    if ( cur_msg >= 9 )
        return;

    /*  Put the current message in to the queue  */
    cur_msg++;
    priority   = 0;                 /* Regular priority; use 0, NOT '0'     */
    msg_class  = msg_type = 0;      /* No class or type at the moment       */
    delivery   = PDEL_MODE_NN_MEM;  /* No Notification and nonrecoverable   */
    timeout    = 30;                /* Wait 3 seconds before giving up      */

    msg_size   = (short) strlen( MsgList[cur_msg] );
  
    dmq_status = pams_put_msg(
                      MsgList[cur_msg],
                      &priority,
                      &my_primary_queue,        /* global */
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


    /*  Set the timer to go off again in 5 seconds  */
    dmq_status = ExampleSetTimer( 5 );


} /*  end of ExampleTimerHandler  */



int32
ExampleSetTimer(int        num_seconds)
{
    char       format;
    int32      p_timeout;
    int32      s_timeout;
    int32      dmq_status;

  
    /*  Set the timer to go off in <num_seconds>  */
    timer_id = 1;   /*  Don't use 0 or 1000 - 1999; reserved by MessageQ  */
    format     = 'P';
    p_timeout  = (int32) num_seconds * 10;  /*  in tenths of seconds  */
    s_timeout  = 0;

    dmq_status = pams_set_timer(
                      &timer_id,
                      &format,
                      &p_timeout,
                      &s_timeout );

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\n\tTimer set to go off in 5 seconds.\n" );
    else
        (void)printf( "\n\tUnexpected error setting timer: %ld.\n", dmq_status );

    return ( dmq_status );

} /*  end of ExampleSetTimer  */



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
    **  Wait for messages to arrive in the queue; get them and print them out.
    **  If we haven't received a message in 10 seconds, leave.
    */
    (void)printf( "\n" );
    dmq_status   = PAMS__SUCCESS;
    priority     = 0;   
    msg_area_len = MAX_MESSAGE_SIZE;
    timeout      = 100;  /* 10 seconds */

    while ( dmq_status == PAMS__SUCCESS ) {
        (void)printf( "\nWaiting for a message...\n" );

        /*  Initialize the message area to hold the message  */
        (void)memset((void*)&msg_area[0], '\0', MAX_MESSAGE_SIZE );

        /*  Get a message  */
        dmq_status = pams_get_msgw( msg_area, &priority, &msg_source,
                          &msg_class, &msg_type, &msg_area_len, &msg_data_len,
                          &timeout, (int32 *) 0, (struct PSB *) 0,
                          (struct show_buffer *) 0, (int32 *) 0,
                          (int32 *) 0, (int32 *) 0, (char *) 0 );

        switch ( dmq_status ) {
            case PAMS__SUCCESS :
                /*  Got a message; Is it a TIMER message or a regular message  */
                if ( (msg_class == MSG_CLAS_PAMS)
                   &&(msg_type == MSG_TYPE_TIMER_EXPIRED) )
                {
                    (void)printf( "\n\tGot TIMER message..........\n" );
                    ExampleTimerHandler();
                }
                else
                    (void)printf( "\n\tGot message:\t\"%s\"\n\n\n", msg_area );
            break;

            case PAMS__TIMEOUT :
                (void)printf( "\n\t*** Timed out waiting for messages.... ***\n" );
            break;

            case PAMS__NOMOREMSG :
                (void)printf( "*** How did I get here? ***\n" );
                /*  Status returned when queue is empty  */
            break;

            default :
                (void)printf( "*** Error getting message; status returned is: %ld. ***\n",
                         dmq_status );
            break;
        }
    }

    return ( dmq_status );

} /* end of ExampleGetWait  */




int32
ExampleExit( )
{
    int32      dmq_status;


    dmq_status    = pams_exit();

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\nExited from MessageQ.\n" );
    else
        (void)printf( "\nError exiting; status returned is %ld.\n", dmq_status );

    return ( dmq_status );

} /*  end of ExampleExit  */


void
ExampleInterruptHandler(int   the_signal)
{
    int32 dmq_status;

    the_signal = the_signal;
    (void)printf( "\nProcessing Interrupt...\n" );

    /*  Cancel outstanding timer(s)  */
    dmq_status = pams_cancel_timer( &timer_id );
  
    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\nCanceled timer.\n" );
    else
        (void)printf( "\nUnexpected error cancelling the timer: %ld.\n", dmq_status );

    /*  Exit from MessageQ  */
    (void)ExampleExit();

    /*  Exit from this program  */
    exit( 0 );

} /*  end of ExampleInterruptHandler  */
