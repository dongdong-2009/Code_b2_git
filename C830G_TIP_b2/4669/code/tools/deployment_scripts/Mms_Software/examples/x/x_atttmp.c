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
**    FILE:           x_atttmp.c
**
**    DESCRIPTION:    Illustrates attaching to a temporary queue
**                    and making a temporary primary queue.
**
**    CREATION DATE:  10 January 1994
**
**    REQUIREMENTS:   none.
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
    int32      attach_mode;
    int32      dmq_status;
    int32      q_type;
    q_address  my_primary_queue;


    (void)printf("\n\nExample program to illustrate how DMQ can\n" );
    (void)printf("attach to a temporary primary queue...\n");

    attach_mode   = PSYM_ATTACH_TEMPORARY;
    q_type        = PSYM_ATTACH_PQ; /* causes the tempory queue to be */
                                    /* a temporary primary queue      */
  
    dmq_status    = pams_attach_q(
                          &attach_mode,
                          &my_primary_queue,
                          &q_type,       /*  make a temp primary queue */
                          (char *) 0,    /*  q_name not needed         */
                          (int32 *) 0,   /*  q_name_len not needed     */
                          (int32 *) 0,   /*  Use default name space    */
                          (int32 *) 0,   /*  No name space list len    */
                          (int32 *) 0,   /*  No timeout value          */
                          (char *) 0,    /*  Reserved                  */
                          (char *) 0 );  /*  Reserved                  */

    if ( dmq_status == PAMS__SUCCESS )
            (void)printf("Attached successfully to temporary queue %d.\n", 
                        my_primary_queue.au.queue);
    else
            (void)printf("Error attaching temporary; status returned is: %ld\n", 
                    dmq_status );

    return( dmq_status );

} /* end of main */
