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
**    FILE:          x_getall.c
**
**    DESCRIPTION:   Example program that highlights the use of
**                   pams_get_msg().
**
**    CREATION DATE: 10 January 1994
**
**    REQUIREMENTS:  The queue named "EXAMPLE_Q_1" must be defined in your init
**                   file as a primary queue.
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
/**         Constants         **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  50


/**  ..............................  **/
/**    Function Forward Declarations **/
/**  ..............................  **/
int32   ExampleGet(void);
int32   ExampleAttach(q_address *q_addr);
int32   ExamplePut(q_address *q_addr);
int32   ExampleExit(void);



long
main(  )
{
   q_address   my_primary_queue;

   (void)printf( "\nGet All Example starting...\n" );

   /*  Attach to MessageQ  */
   if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
       exit( 0 );

   /*  Put some messages in my own queue  */
   if ( ExamplePut( &my_primary_queue ) != PAMS__SUCCESS ) {
       (void)ExampleExit();
       exit( 0 );
   }

   /*  Get them back  */
   (void)ExampleGet();

   /*  Exit from MessageQ  */
   (void)ExampleExit();
   return ( PAMS__SUCCESS );

} /*  end of main  */



int32
ExampleAttach(q_address *q_addr)
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
  
    dmq_status = pams_attach_q( &attach_mode, q_addr, &q_type, q_name,
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


  
int32
ExamplePut(q_address *q_addr)
{
   int         msg_counter;
   char        priority;
   char        delivery;
   char        uma;
   short       msg_class;
   short       msg_type;
   short       msg_size;
   int32       dmq_status;
   int32       timeout;
   struct PSB  put_psb;
   char        **msg_ptr;

   static char  *MessageList[] = {
                        "first message",
                        "second message",
                        "third message",
                        "fourth message",
                        "fifth message",
                        "sixth message",
                        "seventh message",
                        "eighth message",
                        "ninth message",
                        "tenth message",
                        NULL };


   /*
   **  Send (pams_Put_msg) the messages in MessageList to the queue
   **  passed in
   */
   (void)printf( "\n" );
   msg_counter = 0;
   priority    = 0;                 /* Regular priority; use 0, NOT '0'    */ 
   msg_class   = msg_type = 0;      /* No class or type at the moment      */
   delivery    = PDEL_MODE_NN_MEM;  /* No Notification and nonrecoverable  */
   timeout     = 100;               /* Wait 10 seconds before giving up    */
   uma         = PDEL_UMA_DISCL;    /* If can't deliver it, DISCard and Log*/

   for ( msg_ptr = MessageList; *msg_ptr != (char *)NULL; msg_ptr++ )
   {
      msg_size        = (short) strlen( *msg_ptr );
      dmq_status      = pams_put_msg(
                              *msg_ptr,
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
         (void)printf( "\nPut message %d:\t%s\n", ++msg_counter, *msg_ptr );    
      else
         (void)printf( "\nError putting message %d; status returned is: %ld.\n", 
                  msg_counter, dmq_status );
   }/* end for */

   (void)printf("\n");
   return ( dmq_status );

} /*  end of ExamplePut  */



int32
ExampleGet( )
{
   int         msg_counter;
   int32       dmq_status;
   char        msg_area[MAX_MESSAGE_SIZE];
   char        priority;
   q_address   msg_source;
   short       msg_class;
   short       msg_type;
   short       msg_area_len;
   short       msg_data_len;

   /*
   **  Get all the messages currently in the queue and print
   **  to the console.
   */
   (void)printf( "\n" );  
   msg_counter    = 0;
   dmq_status     = PAMS__SUCCESS;

   priority       = 0;                 /*  get all messages           */
   msg_area_len   = MAX_MESSAGE_SIZE;  /*  size of the message buffer */

   while ( dmq_status == PAMS__SUCCESS ) {
      /*  Initialize the message area to hold the message  */
      (void)memset((void*)&msg_area[0], '\0', MAX_MESSAGE_SIZE );

      /*  Get a message  */
      dmq_status = pams_get_msg( 
                         msg_area,       /*  where msg is written   */
                         &priority,      /*  get all messages       */
                         &msg_source,    /*  q_address of sender    */
                         &msg_class,     /*  class of received msg  */
                         &msg_type,      /*  type of received msg   */
                         &msg_area_len,  /*  size of message buffer */
                         &msg_data_len,  /*  actual size of the msg */
                         (int32 *) 0,      
                         (struct PSB *) 0, 
                         (struct show_buffer *) 0,
                         (int32 *) 0,
                         (int32 *) 0,
                         (int32 *) 0,
                         (char *) 0 );

      switch ( dmq_status )
      {
         case PAMS__SUCCESS :   
            (void)printf( "\nGot message %d:\t%s\n", ++msg_counter, msg_area );
         break;

         case PAMS__NOMOREMSG :
            /*  Status returned when queue is empty  */
         break;

         default :
            (void)printf( "\nError getting message %d; status returned is %ld.\n",
                     msg_counter, dmq_status );
         break;

      }/* end case */

   }/* end while */

   (void)printf("\n");
   return ( dmq_status );

} /* end of ExampleGet  */


int32
ExampleExit( )
{
   int32    dmq_status;

   dmq_status = pams_exit();

   if ( dmq_status == PAMS__SUCCESS )
       (void)printf( "\nExited from MessageQ.\n" );
   else
       (void)printf( "Error exiting; status returned is %ld.\n", dmq_status );

   return ( dmq_status );

} /*  end of ExampleExit  */
