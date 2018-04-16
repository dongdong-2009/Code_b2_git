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
**    FILE:          x_locate.c
**
**    DESCRIPTION:   Illustrates using pams_locate_q(). This will attach
**                   to "EXAMPLE_Q_1", the locate the queue "EXAMPLE_Q_2",
**                   where a message will be put.
**
**    CREATION DATE: 10 January 1994
**
**    REQUIREMENTS:  The queues named "EXAMPLE_Q_1" and "EXAMPLE_Q_2" must
**                   be defined in your init file; "EXAMPLE_Q_1" must be a
**                   primary queue.
**
*/

/**  .......................  **/
/**  C library include files  **/
/**  .......................  **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**  ......................  **/
/**  MessageQ include files  **/
/**  ......................  **/
#include "p_entry.h"
#include "p_return.h"
#include "p_symbol.h"

/**  .......................  **/
/**      Local constants      **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  50

/**  .............................. **/
/**   Function Forward Declarations **/
/**  .............................. **/
int32 ExampleAttach(q_address *);
int32 ExampleLocate(char *queue_name, q_address *q_addr);
int32 ExamplePut(q_address *);
void  ExampleExit( );


long
main( )
{
   q_address   my_primary_queue;
   q_address   target_queue;

   /*  Attach to MessageQ  */
   if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
      exit( 0 );

   /*  Find the address of "EXAMPLE_Q_1"  */
   if ( ExampleLocate( "EXAMPLE_Q_1", &target_queue ) != PAMS__SUCCESS )
   {
      ExampleExit();
      exit( 0 );
   }

   /*  Put a message into the target queue  */
   (void)ExamplePut( &target_queue );

   /*  Leave it there  */
   /*  Exit from MessageQ  */
   ExampleExit();

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
    **  Attach by name to the primary queue "EXAMPLE_Q_2"
    */
    (void)strcpy( q_name, "EXAMPLE_Q_2" );
    attach_mode = PSYM_ATTACH_BY_NAME;
    q_type      = PSYM_ATTACH_PQ;
    q_name_len  = (int32) strlen( q_name );
  
    dmq_status  = pams_attach_q( &attach_mode, q_addr, &q_type, q_name,
                        &q_name_len,
                        (int32 *) 0,    /*  Use default name space  */
                        (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\nAttached to queue:  \"%s\"\n", q_name );
    else
        (void)printf( "\nError attaching to queue: \"%s\"; status returned is: %ld.\n", 
                 q_name, dmq_status );

    return ( dmq_status );

} /*  end of ExampleAttach  */



int32
ExampleLocate(char *queue_name, q_address *q_addr)
{
    int32    dmq_status;
    int32    queue_name_len;
    int32    wait_mode;
    int32    req_id;

    /*
    **  Attempt to locate the queue_name in the process and group name spaces
    */
    queue_name_len = strlen( queue_name );
    wait_mode      = PSYM_WF_RESP;
    req_id         = 1;

    dmq_status     = pams_locate_q(
                           queue_name,
                           &queue_name_len,
                           q_addr,
                           &wait_mode,
                           &req_id,
                           (int32 *) 0,  /* No response queue  */
                           (int32 *) 0,  /* Use default name space list of
                           process and group  */
                           (int32 *) 0,  /* name space list len not needed */
                           (int32 *) 0 );

    switch ( dmq_status )
    {
       case PAMS__SUCCESS :
          (void)printf( "\nLocated queue named: \"%s\" at %d.%d\n", queue_name, 
                  q_addr->au.group, q_addr->au.queue );
       break;
      
       case PAMS__NOOBJECT :
          (void)printf( "\nQueue: \"%s\" not found.\n", queue_name );
       break;

       default :
          (void)printf( "\nUnexpected error returned from pams_locate_q: %ld\n", 
                   dmq_status );
       break;
    }/*end case */

    return ( dmq_status );

} /*  end of ExampleLocate  */


  
int32
ExamplePut(q_address  *q_addr)
{
    char        priority;
    char        delivery;
    char        uma;
    short       msg_class;
    short       msg_type;
    short       msg_size;
    static char msg[25];
    int32       dmq_status;
    int32       timeout;

    priority    = 0;                 /* Regular priority; use 0, NOT '0'     */
    msg_class   = msg_type = 0;      /* No class or type at the moment       */
    delivery    = PDEL_MODE_NN_MEM;  /* No Notification and nonrecoverable   */
    timeout     = 100;               /* Wait 10 seconds before giving up     */
    uma         = PDEL_UMA_DISC;    /* If can't deliver it, DISCard and Log */

    (void)strcpy( msg, "this message was sent..." );
    msg_size = (short) strlen( msg );

    dmq_status = pams_put_msg(msg, &priority,
                       q_addr,        /* target passed in */
                       &msg_class, &msg_type, &delivery, &msg_size, &timeout,
                       (struct PSB *) 0, &uma, (q_address *) 0,
                       (int32 *) 0, (char *) 0, (char *) 0 );
      
    switch ( dmq_status ) {
       case PAMS__SUCCESS :      /* queue is presently attached */
          (void)printf( "\nPUT returned: PAMS__SUCCESS "); 
          (void)printf( "\nPut message \"%s\" to %d.%d\n", msg, q_addr->au.group,
                   q_addr->au.queue );
       break;
      
       case PAMS__UNATTACHEDQ :  /* queue is not presently attached */
          (void)printf( "\nPUT returned: PAMS__UNATTACHEDQ "); 
          (void)printf( "\nPut message \"%s\" to %d.%d\n", msg, q_addr->au.group,
                   q_addr->au.queue );
       break;


       case PAMS__NOTACTIVE :  /* queue is not presently attached */
          (void)printf( "\nPUT returned: PAMS__NOTACTIVE "); 
          (void)printf( "\nPut message \"%s\" to %d.%d\n", msg, q_addr->au.group,
                   q_addr->au.queue );
       break;

       default :
          (void)printf( "\nError putting message; status returned is: %ld.\n", 
                   dmq_status );
       break;
    }/*end case */

    return ( dmq_status );

} /*  end of ExamplePut  */



void
ExampleExit( )
{
   int32    dmq_status;

   dmq_status = pams_exit();

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "\nExited from MessageQ.\n" );
   else
      (void)printf( "\nError exiting; status returned is %ld.\n", dmq_status );

} /*  end of ExampleExit  */
