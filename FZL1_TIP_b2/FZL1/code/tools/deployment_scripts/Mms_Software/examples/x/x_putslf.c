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
**    FILE:           x_putslf.c
**
**    DESCRIPTION:    This simple example will illustrate how to easy
**                    it is to setup and call pams_put_msg.
**                    The example simply attaches to "EXAMPLE_Q_1"
**                    puts a (sends) message to itself, and exits.
**
**    CREATION DATE:  10 January 1994
**
*/


/**  .......................  **/
/**  C library include files  **/
/**  .......................  **/
#include <stdio.h>
#include <string.h>


/**  ......................  **/
/**  MessageQ include files  **/
/**  ......................  **/
#include "p_entry.h"
#include "p_return.h"
#include "p_symbol.h"



long
main( )
{
    int32         attach_mode;
    int32         dmq_status;
    int32         q_name_len;
    int32         q_type;
    int32         timeout;
    short         mclass;
    short         type;
    short         msg_size;
    char          delivery;
    char          priority;
    char          uma;
    static char   msg_area[18];
    static char   q_name[12];
    q_address     my_queue;
    struct PSB    put_psb;


    /*
    **  Attach to MessageQ  
    */
    attach_mode = PSYM_ATTACH_BY_NAME;
    q_type      = PSYM_ATTACH_PQ;
    (void)strcpy( q_name, "EXAMPLE_Q_1" );
    q_name_len  = (int32) strlen( q_name );
    (void)strcpy( msg_area, "Talkin' to myself" );

    (void)printf( "\nAttaching to queue %s by name...\n", q_name);

    dmq_status = pams_attach_q(&attach_mode, &my_queue, &q_type, q_name,
                      &q_name_len,
                      (int32 *) 0,    /*  Use default name space  */
                      (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\n\tAttached successfully to queue \"%s\".\n", q_name );
    else
        (void)printf( "\nError attaching to queue \"%s\"; status returned is: %ld\n", 
                 q_name, dmq_status );

    /*
    **  Put a message into my own queue  
    */
    priority   = 0;
    mclass      = 0;
    type       = 0;
    delivery   = PDEL_MODE_NN_MEM;
    msg_size   = (short) strlen( msg_area );
    timeout    = 50;  /* 5 seconds  */
    uma        = PDEL_UMA_DISCL;

    dmq_status = pams_put_msg(
                      msg_area,
                      &priority,
                      &my_queue,
                      &mclass,
                      &type,
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
        (void)printf( "\n\tPut successfully to queue: \"%s\".\n", q_name );
    else
        (void)printf( "\nError putting to queue: \"%s\"; status returned is: %ld\n", 
                 q_name, dmq_status );


    /*
    **  Detach from MessageQ
    */
    dmq_status = pams_exit();
    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\n\tDetached successfully from MessageQ.\n" );
    else
        (void)printf( "\nError detaching from MessageQ; status returned is: %ld\n", 
                 dmq_status );

    return( PAMS__SUCCESS );

} /*  end of main  */
