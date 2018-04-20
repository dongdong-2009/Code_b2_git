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
**    FILE:          x_bind.c
**
**    DESCRIPTION:   Illustrates using pams_bind_q and pams_locate_q() 
**                   to bind/locate bus-wide queue references
**                   This will attach to "EXAMPLE_Q_1", bind to the 
**                   reference "GLOBAL_Q_1", then locate "GLOBAL_Q_1"
**                   and check the bound address is correct
**
**    CREATION DATE: 06-February-1997
**
**    REQUIREMENTS:  The queue named "EXAMPLE_Q_1" must be defined in 
**                   the QCT section of your init file
**                   The reference "GLOBAL_Q_1" must be defined in the
**                   GNT section of your init file as follows :
**                      GLOBAL_Q_1    0.0    G
**
**		     A Naming Agent Server must be configured in order
**		     for global naming to work correctly.  
**
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

/**  .............................. **/
/**   Function Forward Declarations **/
/**  .............................. **/
int32 ExampleAttach(q_address   *q_addr);
int32 ExampleLocate(char *queue_name, q_address *q_addr);
int32 ExampleBind(char *queue_name, q_address *q_addr);
void  ExampleExit(void);


long
main( )
{
   q_address   my_primary_queue;
   q_address   target_queue;

   /*  Attach to MessageQ  */
   if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
      exit( 0 );

   /*  Bind "GLOBAL_Q_1" to "EXAMPLE_Q_1"  */
   if ( ExampleBind( "GLOBAL_Q_1", &my_primary_queue ) != PAMS__SUCCESS )
   {
      ExampleExit();
      exit( 0 );
   }

   /*  Find the address of "GLOBAL_Q_1"  */
   if ( ExampleLocate( "GLOBAL_Q_1", &target_queue ) != PAMS__SUCCESS ) {
      ExampleExit();
      exit( 0 );
   }

   /*  If located address is not attached queue exit */
   if (target_queue.all != my_primary_queue.all)
   {
      (void)printf("GLOBAL_Q_1 is not bound to my primary queue - Abort\n");
      exit( 0 );
   }

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
    **  Attach by name to the primary queue "EXAMPLE_Q_1"
    */
    (void)strcpy( q_name, "EXAMPLE_Q_1" );
    attach_mode = PSYM_ATTACH_BY_NAME;
    q_type      = PSYM_ATTACH_PQ;
    q_name_len  = (int32) strlen( q_name );
  
    dmq_status  = pams_attach_q( &attach_mode, q_addr, &q_type,
                        q_name, &q_name_len,
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
    int32    namespace_list[] = { PSEL_TBL_PROC, PSEL_TBL_GRP, PSEL_TBL_BUS };
    int32    namespace_list_len = 3;
    int32    timeout = 300;   /* wait for 30 seconds max for binding status */

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
                           namespace_list,
                           &namespace_list_len,
                           &timeout);

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
ExampleBind(char *queue_name, q_address *q_addr)
{
    int32    dmq_status;
    int32    queue_name_len;
    int32    timeout = 300;   /* wait for 30 seconds max for binding status */
    int32    namespace_list = PSEL_TBL_BUS;
    int32    namespace_list_len = 1;

    /*
    **  Attempt to bind the queue_name in the bus name space
    */
    queue_name_len = strlen( queue_name );
    dmq_status     = pams_bind_q(
                           q_addr,
                           queue_name,
                           &queue_name_len,
                           &namespace_list,
                           &namespace_list_len,
                           &timeout,
                           (char *) 0 );

    switch ( dmq_status )
    {
       case PAMS__SUCCESS :
          (void)printf( "\nBound succesfully to queue named: \"%s\" \n", queue_name);
       break;
      
       case PAMS__TIMEOUT :
          (void)printf( "\nTimeout waiting for a response from binding\n");
       break;

       default :
          (void)printf( "\nUnexpected error returned from pams_bind_q: %ld\n", 
                   dmq_status );
       break;
    }/*end case */

    return ( dmq_status );

} /*  end of ExampleBind  */



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
