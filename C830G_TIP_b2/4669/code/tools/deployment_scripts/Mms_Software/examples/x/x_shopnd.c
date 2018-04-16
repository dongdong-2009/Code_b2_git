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
**    FILE:          x_shopnd.c
**
**    DESCRIPTION:   This illustrates using putil_show_pending to
**                   display the number of pending messages currently
**                   in the queue.
**
**    CREATION DATE: 10 January 1994
**                                                             
**    REQUIREMENTS:  The queue named "EXAMPLE_Q_1" must be defined in
**                   your init file.
**
*/


/**  .......................  **/
/**  C library include files  **/
/**  .......................  **/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**  ......................  **/
/**  MessageQ include files  **/
/**  ......................  **/
#include "p_entry.h"
#include "p_return.h"
#include "p_symbol.h"


/**  ....................... **/
/**            Constants     **/
/**  ....................... **/
#define MAX_MESSAGE_SIZE  50
#define NUM_QUEUES 1

/**  ............................. **/
/**  Function Forward Declarations **/
/**  ............................. **/
long  ExampleAttach(char*, q_address*);
void  ExamplePut(q_address*);
long  ExampleGet(q_address*);
void  ExampleExit( );
long  ExampleConfirm( );
int32 ExamplePendingMsgs(q_address*);


/**  ........................................ **/
/**  MARCO definition for converting sysmbols **/
/**  into printable strings                   **/
/**  ........................................ **/
#define MSTR(x,y) case (x) : mstrc = y; break


/**  ........................................ **/
/**  Funstion for converting symbol to string **/
/**  There are other methods for achieving    **/
/**  the same result, but this method is      **/
/**  portable.  It is provided as a futher    **/
/**  example and may be useful to you !!!     **/ 
/**  ........................................ **/
static char *get_sym_str(long sym)
{
	char *mstrc;                   
	char symbuf[80];

	switch (sym) {
	MSTR(PAMS__SUCCESS, "PAMS__SUCCESS");
	MSTR(PAMS__DISC_SUCCESS, "PAMS__DISC_SUCCESS");
	MSTR(PAMS__DISCL_SUCCESS, "PAMS__DISCL_SUCCESS");
	MSTR(PAMS__RESRCFAIL, "PAMS__RESRCFAIL");
	MSTR(PAMS__DLJ_SUCCESS, "PAMS__DLJ_SUCCESS");
	MSTR(PAMS__DLQ_SUCCESS, "PAMS__DLQ_SUCCESS");
	MSTR(PAMS__RTS_SUCCESS, "PAMS__RTS_SUCCESS");
	MSTR(PAMS__SAF_SUCCESS, "PAMS__SAF_SUCCESS");

	MSTR(PAMS__NOMOREMSG, "PAMS__NOMOREMSG");
	MSTR(PAMS__NOSEND, "PAMS__NOSEND");
	MSTR(PAMS__WAKEFAIL, "PAMS__WAKEFAIL");
	MSTR(PAMS__TIMERACT, "PAMS__TIMERACT");
	MSTR(PAMS__MSGACT, "PAMS__MSGACT");
	MSTR(PAMS__NO_UMA, "PAMS__NO_UMA");
	MSTR(PAMS__UMA_NA, "PAMS__UMA_NA");
	MSTR(PAMS__TRACEBACK, "PAMS__TRACEBACK");
	MSTR(PAMS__STORED, "PAMS__STORED");
	MSTR(PAMS__ENQUEUED, "PAMS__ENQUEUED");
	MSTR(PAMS__UNATTACHEDQ, "PAMS__UNATTACHEDQ");
	MSTR(PAMS__CONFIRMREQ, "PAMS__CONFIRMREQ");
	MSTR(PAMS__PROPAGATE, "PAMS__PROPAGATE");
	MSTR(PAMS__ABORT, "PAMS__ABORT");
	MSTR(PAMS__BADDECLARE, "PAMS__BADDECLARE");
	MSTR(PAMS__BADFREE, "PAMS__BADFREE");
	MSTR(PAMS__TIMEOUT, "PAMS__TIMEOUT");
	MSTR(PAMS__ACKTMO, "PAMS__ACKTMO");
	MSTR(PAMS__MSGUNDEL, "PAMS__MSGUNDEL");
	MSTR(PAMS__EX_Q_LEN, "PAMS__EX_Q_LEN");
	MSTR(PAMS__POSSDUPL, "PAMS__POSSDUPL");
	MSTR(PAMS__STUB, "PAMS__STUB");
	MSTR(PAMS__SENDER_TMO_EXPIRED, "PAMS__SENDER_TMO_EXPIRED");
	MSTR(PAMS__MRQTBLFULL, "PAMS__MRQTBLFULL");
	MSTR(PAMS__NOTALLOCATE, "PAMS__NOTALLOCATE");
	MSTR(PAMS__BIGBLKSIZE, "PAMS__BIGBLKSIZE");
	MSTR(PAMS__MSGTOBIG, "PAMS__MSGTOBIG");
	MSTR(PAMS__INVALIDID, "PAMS__INVALIDID");
	MSTR(PAMS__INVFORMAT, "PAMS__INVFORMAT");
	MSTR(PAMS__INVBUFFPTR, "PAMS__INVBUFFPTR");
	MSTR(PAMS__INVALIDNUM, "PAMS__INVALIDNUM");
	MSTR(PAMS__BIGMSG, "PAMS__BIGMSG");
	MSTR(PAMS__MSGTOSMALL, "PAMS__MSGTOSMALL");
	MSTR(PAMS__AREATOSMALL, "PAMS__AREATOSMALL");
	MSTR(PAMS__NOCANSEND, "PAMS__NOCANSEND");
	MSTR(PAMS__NOTACTIVE, "PAMS__NOTACTIVE");
	MSTR(PAMS__EXCEEDQUOTA, "PAMS__EXCEEDQUOTA");
	MSTR(PAMS__BADPRIORITY, "PAMS__BADPRIORITY");
	MSTR(PAMS__BADDELIVERY, "PAMS__BADDELIVERY");
	MSTR(PAMS__BADJOURNAL, "PAMS__BADJOURNAL");
	MSTR(PAMS__BADPROCNUM, "PAMS__BADPROCNUM");
	MSTR(PAMS__BADTMPPROC, "PAMS__BADTMPPROC");
	MSTR(PAMS__BADSYNCHNUM, "PAMS__BADSYNCHNUM");
	MSTR(PAMS__BADTMPSYNCH, "PAMS__BADTMPSYNCH");
	MSTR(PAMS__BADRECEIVE, "PAMS__BADRECEIVE");
	MSTR(PAMS__BADTIME, "PAMS__BADTIME");
	MSTR(PAMS__NOTDCL, "PAMS__NOTDCL");
	MSTR(PAMS__STATECHANGE, "PAMS__STATECHANGE");
	MSTR(PAMS__INVUCBCNTRL, "PAMS__INVUCBCNTRL");
	MSTR(PAMS__NOLINK, "PAMS__NOLINK");
	MSTR(PAMS__CIRACT, "PAMS__CIRACT");
	MSTR(PAMS__PROTOCOL, "PAMS__PROTOCOL");
	MSTR(PAMS__COMMERR, "PAMS__COMMERR");
	MSTR(PAMS__BADSELIDX, "PAMS__BADSELIDX");
	MSTR(PAMS__IDXTBLFULL, "PAMS__IDXTBLFULL");
	MSTR(PAMS__BADPARAM, "PAMS__BADPARAM");
	MSTR(PAMS__NOMRS, "PAMS__NOMRS");
	MSTR(PAMS__DISC_FAILED, "PAMS__DISC_FAILED");
	MSTR(PAMS__DISCL_FAILED, "PAMS__DISCL_FAILED");
	MSTR(PAMS__DLJ_FAILED, "PAMS__DLJ_FAILED");
	MSTR(PAMS__DLQ_FAILED, "PAMS__DLQ_FAILED");
	MSTR(PAMS__DQF_DEVICE_FAIL, "PAMS__DQF_DEVICE_FAIL");
	MSTR(PAMS__INVUMA, "PAMS__INVUMA");
	MSTR(PAMS__DQF_FULL, "PAMS__DQF_FULL");
	MSTR(PAMS__INVJH, "PAMS__INVJH");
	MSTR(PAMS__LINK_DOWN, "PAMS__LINK_DOWN");
	MSTR(PAMS__BADSEQ, "PAMS__BADSEQ");
	MSTR(PAMS__NOTJRN, "PAMS__NOTJRN");
	MSTR(PAMS__MRS_RES_EXH, "PAMS__MRS_RES_EXH");
	MSTR(PAMS__NOMOREJH, "PAMS__NOMOREJH");
	MSTR(PAMS__REJECTED, "PAMS__REJECTED");
	MSTR(PAMS__NOSUCHPCJ, "PAMS__NOSUCHPCJ");
	MSTR(PAMS__UCBERROR, "PAMS__UCBERROR");
	MSTR(PAMS__BADUMA, "PAMS__BADUMA");
	MSTR(PAMS__BADRESPQ, "PAMS__BADRESPQ");
	MSTR(PAMS__BADARGLIST, "PAMS__BADARGLIST");
	MSTR(PAMS__NO_DQF, "PAMS__NO_DQF");
	MSTR(PAMS__NO_SAF, "PAMS__NO_SAF");
	MSTR(PAMS__RTS_FAILED, "PAMS__RTS_FAILED");
	MSTR(PAMS__SAF_DEVICE_FAIL, "PAMS__SAF_DEVICE_FAIL");
	MSTR(PAMS__SAF_FAILED, "PAMS__SAF_FAILED");
	MSTR(PAMS__BADLOGIC, "PAMS__BADLOGIC");
	MSTR(PAMS__SELRCVACT, "PAMS__SELRCVACT");
	MSTR(PAMS__NOMRQRESRC, "PAMS__NOMRQRESRC");
	MSTR(PAMS__BADASSIGN, "PAMS__BADASSIGN");
	MSTR(PAMS__LOGNAME, "PAMS__LOGNAME");
	MSTR(PAMS__LOGNAME2, "PAMS__LOGNAME2");
	MSTR(PAMS__LOGNAME3, "PAMS__LOGNAME3");
	MSTR(PAMS__LOGNAME4, "PAMS__LOGNAME4");
	MSTR(PAMS__LOGNAME5, "PAMS__LOGNAME5");
	MSTR(PAMS__NOOPEN, "PAMS__NOOPEN");
	MSTR(PAMS__BADSCRIPT, "PAMS__BADSCRIPT");
	MSTR(PAMS__DECLARED, "PAMS__DECLARED");
	MSTR(PAMS__EXHAUSTBLKS, "PAMS__EXHAUSTBLKS");
	MSTR(PAMS__INTERNAL, "PAMS__INTERNAL");
	MSTR(PAMS__QUECORRUPT, "PAMS__QUECORRUPT");
	MSTR(PAMS__REMQUEFAIL, "PAMS__REMQUEFAIL");
	MSTR(PAMS__INSQUEFAIL, "PAMS__INSQUEFAIL");
	MSTR(PAMS__CREATEFAIL, "PAMS__CREATEFAIL");
	MSTR(PAMS__DCLTMPFAIL, "PAMS__DCLTMPFAIL");
	MSTR(PAMS__PAMSDOWN, "PAMS__PAMSDOWN");
	MSTR(PAMS__BADASTPARM, "PAMS__BADASTPARM");
	MSTR(PAMS__NOQUOTA, "PAMS__NOQUOTA");
	MSTR(PAMS__NOTPRIMARYQ, "PAMS__NOTPRIMARYQ");
             
	default:
	   (void) sprintf(symbuf, "PAMS__UNEXPECTED - %d", sym);
	   mstrc = symbuf;

	}                 

	return (mstrc);
}



long
main( )
{
    long        dmq_status;
    q_address   the_queue;
    int32       num_pending;

    (void)printf( "\n\n\nShow pending example...\n\n" );

    /*  Attach to EXAMPLE_Q_1  */
    if ( ExampleAttach( "EXAMPLE_Q_1", &the_queue ) != PAMS__SUCCESS )
        exit( 0 );

    /*
    **    Show the number of messages pending at start
    */

    num_pending = ExamplePendingMsgs( &the_queue );

    (void)printf( "\n\tUpon startup, there are %d messages pending.\n",
            num_pending );

    /*
    **    Put some messages into my queue, show pending messages
    **    after each put
    */
    ExamplePut( &the_queue );


    /*
    **    Get the messages, show pending messages after each get
    */
    dmq_status = ExampleGet( &the_queue );


    /*  Exit from MessageQ  */
    ExampleExit();

    (void)printf( "\n\n\nShow pending example complete......\n\n\n\n" );
    return( dmq_status );

} /*  end of main  */



long
ExampleAttach(char *q_name, q_address *q_addr)
{
    int32       attach_mode;
    int32       dmq_status;
    int32       q_name_len;
    int32       q_type;

    /*
    **  Attach by name to the primary queue passed in
    */
    attach_mode   = PSYM_ATTACH_BY_NAME;
    q_type        = PSYM_ATTACH_PQ;
    q_name_len    = (long) strlen( q_name );
  
    dmq_status    = pams_attach_q(
                            &attach_mode,
                            q_addr,
                            &q_type,
                            q_name,
                            &q_name_len,
                            (int32 *) 0,    /*  Use default name space  */
                            (int32 *) 0,
                            (int32 *) 0,
                            (char *) 0,
                            (char *) 0 );
                  
    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\tAttached to queue: \"%s\".\n", q_name );
    else
        (void)printf( "\tError attaching to queue: \"%s\"; status returned is:  %s (%ld)\n\n",
                 q_name, get_sym_str(dmq_status), dmq_status);      

    return ( dmq_status );

} /*  end of ExampleAttach  */


  
void
ExamplePut(q_address     *q_addr)
{
    char           priority;
    char           delivery;
    char           uma;
    short          msg_class;
    short          msg_type;
    short          msg_size;
    int            i;
    int32          dmq_status;
    int32          timeout;
    struct PSB     put_psb;
    static char    *MessageList[] = {
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
    **  Put the messages in MessageList to the queue passed in
    */
    (void)printf( "\n" );
    priority  = '\0';             /* Regular priority; use 0, NOT '0'      */
    msg_class = msg_type = 0;     /* No class or type at the moment        */
    delivery  = PDEL_MODE_NN_MEM; /* Don't wait for it                     */
    timeout   = 100;              /* Wait 10 seconds before giving up      */
    uma       = PDEL_UMA_DISC;    /* If it can't be delivered, just log it */

    for ( i = 0; i < 10; i++ )
    {
        msg_size = (short) strlen( MessageList[i] );
        dmq_status = pams_put_msg(
                           MessageList[i],
                           &priority,
                           q_addr,        /* passed in */
                           &msg_class,
                           &msg_type,
                           &delivery,
                           &msg_size,
                           &timeout,
                           &put_psb,
                           &uma,
                           (q_address *) 0,
                           (int32 *) 0,
                           (char *) 0,
                           (char *) 0 );

        if ( dmq_status == PAMS__SUCCESS )
        {
            (void)printf( "\tPut message:\t%s\n", MessageList[i] );
            (void)printf( "\t\tThere are %d messages pending.\n\n", 
                        ExamplePendingMsgs( q_addr ) );
        }
        else
            (void)printf( "\tError putting message; status returned is: %s (%ld)\n\n",
                     get_sym_str(dmq_status), dmq_status);      
    }/* end for */

} /*  end of ExamplePut  */



long
ExampleGet(q_address      *q_addr)
{
    char            priority;
    short           msg_class;
    short           msg_type;
    short           msg_area_len;
    short           msg_data_len;
    int32           dmq_status;
    int32           sel_filter;
    int32           timeout;
    char            msg_area[MAX_MESSAGE_SIZE];
    q_address       msg_source;
    struct PSB      get_psb;


    /*
    **  Get all the messages currently in the queue and print them out
    */

    (void)printf( "\n" );   
    priority      = 0;    /*  get messages of both high and normal priorities */
    sel_filter    = 0;    /*  select all messages, i.e. don't filter any out  */
    timeout       = 50;   /*  5 second timeout                                */
    msg_area_len  = MAX_MESSAGE_SIZE;
    dmq_status    = PAMS__SUCCESS;

    while ( dmq_status == PAMS__SUCCESS ) {
        /*  Initialize the message area to hold the message  */
        (void)memset((void*)&msg_area[0], '\0', MAX_MESSAGE_SIZE );

        /*  Get a message  */
        dmq_status    = pams_get_msgw(msg_area, &priority, &msg_source,
                                &msg_class, &msg_type, &msg_area_len,
                                &msg_data_len, &timeout, &sel_filter,
                                &get_psb, (struct show_buffer *) 0,
                                (int32 *) 0, (int32 *) 0, (int32 *) 0,
                                (char *) 0 );
        switch ( dmq_status ) {
            case PAMS__SUCCESS :
                (void)printf( "\n\tGot message:\t%s\n", msg_area );
                (void)printf( "\t\tThere are now %d messages pending.\n",
                            ExamplePendingMsgs( q_addr ) );
            break;

            case PAMS__TIMEOUT :
                (void)printf( "\n\tTimed out waiting for message.\n" );
                (void)printf( "\t\tThere are now %d messages pending.\n",
                            ExamplePendingMsgs( q_addr ) );
            break;

            default :
                (void)printf( "\n\tError getting message; status returned is: %s (%ld)\n\n",
                     get_sym_str(dmq_status), dmq_status);         
            break;
        }/* end case dmq_status */

    }/* end while */

    return ( dmq_status );

} /* end of ExampleGet  */


void
ExampleExit( )
{
    int32   dmq_status;

    dmq_status = pams_exit();

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\nExited from MessageQ.\n" );
    else
        (void)printf( "\nError exiting; status returned is: %s (%ld)\n\n",
                     get_sym_str(dmq_status), dmq_status);

} /*  end of ExampleExit  */



int32
ExamplePendingMsgs(q_address   *q_addr)
{
    int32       dmq_status;

    int32       num_queues;
    short       q_list[NUM_QUEUES];
    int32       pend_list[NUM_QUEUES];

    /*
    **  Return the number of messages pending for the queue passed in
    */

    num_queues    = NUM_QUEUES;
    q_list[0] = q_addr->au.queue;

    dmq_status    = putil_show_pending(
                               &num_queues, 
                               q_list,
                               pend_list );

    if ( dmq_status != PAMS__SUCCESS ) {
        (void)printf( "\n\tError in calling putil_show_pending: %s (%ld)\n\n",
                     get_sym_str(dmq_status), dmq_status);         

        pend_list[0] = -1;
    }

    return ( pend_list[0] );

} /*  end of ExamplePendingMsgs  */
