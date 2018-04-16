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
**    FILE:          x_exit.c
**
**    DESCRIPTION:   This trivial example illustrates exiting from MessageQ
**
**    CREATION DATE: 10 January 1994
**
**    REQUIREMENTS:  None
**
*/

/**  .......................  **/
/**  C library include files  **/
/**  .......................  **/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
   int32       q_type;
   q_address   my_primary_queue;

   /*
   **  Must be attached before you can exit
   */ 
   (void)printf("\n\nAttempting to ATTACH to a Temporary queue\n");
   attach_mode = PSYM_ATTACH_TEMPORARY;
   q_type      = PSYM_ATTACH_PQ;

   dmq_status  = pams_attach_q(
                     &attach_mode,
                     &my_primary_queue,
                     &q_type,
                     (char *) 0,    /*  q_name not needed  */
                     (int32 *) 0,   /*  q_name_len not needed  */
                     (int32 *) 0,   /*  Use default name space  */
                     (int32 *) 0,
                     (int32 *) 0,
                     (char *) 0,
                     (char *) 0 );

   if ( dmq_status != PAMS__SUCCESS )
   {
      (void)printf( "\nError attaching temporary; status returned is: %ld\n", 
                 dmq_status );
      exit(0);
   }

   (void)printf( "\nsuccessfully ATTACHed to temporary queue %d.\n", 
            my_primary_queue.au.queue );

   /*
   **  Attached, now exit
   */
   dmq_status = pams_exit();

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "\nExited from MessageQ.\n\n\n" );
   else
      (void)printf( "\nUnexpected error occurred when exiting: %ld\n\n\n", dmq_status );

   return( dmq_status );
} /* end of main */
