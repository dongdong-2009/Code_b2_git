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
**    FILE:          x_attnum.c
**
**    DESCRIPTION:   Attach to queue by nunmber example program.
**                   This example illustrates attaching by number.
**                   The example depends on having the queue numbered
**                   1 defined in your init file as a primary queue.
**
**    CREATION DATE: 10 January 1994
**
**    REQUIREMENTS:  The queue number "1" must be defined in your
**                   init file as a primary queue.
**
*/

/**  .........................  **/
/**  C library include files    **/
/**  .........................  **/
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
   int32       attach_mode;
   int32       dmq_status;
   int32       q_num_len;
   int32       q_type;
   int         q_number;
   char        q_num_str[2];
   q_address   q_addr;


   (void)printf("\n\nExample program to illustrate how DMQ can\n" );
   (void)printf("attach to a primary queue '1' by number...\n" );

   attach_mode = PSYM_ATTACH_BY_NUMBER;
   q_type      = PSYM_ATTACH_PQ;
   q_number    = 1;
   (void)sprintf( q_num_str, "%d", q_number ); 
   q_num_len   = (int32) strlen( q_num_str );
  
   dmq_status = pams_attach_q( &attach_mode, &q_addr, &q_type,
                     q_num_str, &q_num_len,
                     (int32 *) 0,   /*  Use default name space */
                     (int32 *) 0,   /*  No name space list len */
                     (int32 *) 0,   /*  No timeout value       */
                     (char *) 0,    /*  Reserved               */
                     (char *) 0);   /*  Reserved               */

   if ( dmq_status == PAMS__SUCCESS )
         (void)printf("Attached successfully to queue numbered %s.\n", q_num_str);
   else
         (void)printf("Error attaching to queue \"%s\"; status returned is: %ld\n", 
               q_num_str, dmq_status );

   return(dmq_status);

} /* end of main */
