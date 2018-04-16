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
**    FILE:          x_putbig.c
**
**    DESCRIPTION:   This program illustrates the use of the additional
**                   length arguments to pams_put_msg and pams_get_msg(w) to
**                   send messages of sizes greater than 32000 bytes.
**                   
**                   The length used by this program is 50000.
**
**
**    CREATION DATE: 17 February 1997
**
**    REQUIREMENTS:  The queue named "EXAMPLE_Q_1" must be defined in your init
**                   file as a primary queue.
**
**                   GROUP_MAX_MESSAGE_SIZE must be set to some value between
**                   50000 and 4192304.
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

/**  .......................  **/
/**         Constants         **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  50000

/**  .......................  **/
/**   Function Forwards       **/
/**  .......................  **/
int32 ExampleAttach(q_address*);
int32 ExamplePut(q_address*);
int32 ExampleGet(void);
void  ExampleExit(void);



long
main( )
{
   q_address   my_primary_queue;

   (void)printf( "\nExample of DmQ Messaging for messages > 32000 bytes started...\n\n" );

   /*  Attach to MessageQ  */
   if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
         exit( 0 );

   /*  Put some messages in my own queue  */
   (void)ExamplePut( &my_primary_queue );

   /*  Get them back  */
   (void)ExampleGet();

   /*  Exit from MessageQ  */
   ExampleExit();

   (void)printf( "\nBasic example complete.\n" );

   return( PAMS__SUCCESS );

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
  
   dmq_status  = pams_attach_q(&attach_mode, q_addr, &q_type, q_name,
                     &q_name_len,
                     (int32 *) 0,    /*  Use default name space  */
                     (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "Attached to queue \"%s\".\n", q_name );
   else
      (void)printf( "Error attaching to queue \"%s\"; status returned is: %ld.\n", 
               q_name, dmq_status );

   return ( dmq_status );

} /*  end of ExampleAttach  */




int32
ExamplePut(q_address *q_addr)
{
   int         i;
   char        priority;
   char        delivery;
   char        uma;
   int32       large_msg_size = MAX_MESSAGE_SIZE;
   short       msg_class;
   short       msg_type;
   short       msg_size = PSYM_MSG_LARGE;
   int32       dmq_status;
   int32       timeout;
   struct PSB  put_psb;
   char        msg_area[MAX_MESSAGE_SIZE];
   static char *MessageList[] = {
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
   priority    = '\0';              /* Regular priority; use 0, NOT '0'     */
   msg_class   = msg_type = 0;      /* No class or type at the moment       */
   delivery    = PDEL_MODE_NN_MEM;  /* No Notification and nonrecoverable   */
   timeout     = 100;               /* Wait 10 seconds before giving up     */
   uma         = PDEL_UMA_DISCL;    /* If can't deliver it, DISCard and Log */

   for ( i = 0; i < 10; i++ )
   {
      (void)memset(msg_area,0,MAX_MESSAGE_SIZE);
      (void)strcpy(msg_area,MessageList[i]);
      dmq_status = pams_put_msg(
                     msg_area,
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
                     &large_msg_size,
                     (char *) 0,
                     (char *) 0 );
      
      if ( dmq_status == PAMS__SUCCESS )
         (void)printf( "Put message:\t\"%s\" in buffer of size %ld\n", 
                 MessageList[i], large_msg_size );
      else
         (void)printf( "Error putting message; status returned is: %ld.\n", 
                  dmq_status );
   }/*end for */

   return ( dmq_status );

} /*  end of ExamplePut  */

int32
ExampleGet( )
{
   char        priority;
   int32       large_msg_area_len = MAX_MESSAGE_SIZE;
   int32       large_msg_data_len;
   short       msg_class;
   short       msg_type;
   short       msg_area_len = PSYM_MSG_LARGE;
   short       msg_data_len;
   int32       dmq_status;
   int32       sel_filter;
   char        msg_area[MAX_MESSAGE_SIZE];
   q_address   msg_source;
   struct PSB  get_psb;


  /*
  **  Get all the messages currently in the queue and print them out
  */
  (void)printf( "\n" );   

  priority     = 0;   /*  get messages of both high and normal priorities */
  sel_filter   = 0;   /*  select all messages, i.e. don't filter any out  */
  dmq_status   = PAMS__SUCCESS;

  while ( dmq_status == PAMS__SUCCESS ) {
      /*  Initialize the message area to hold the message  */
      (void)memset((void*)&msg_area[0], '\0', MAX_MESSAGE_SIZE );

      /*  Get a message  */
      dmq_status = pams_get_msg( msg_area, &priority, &msg_source, &msg_class,
                 &msg_type, &msg_area_len, &msg_data_len, &sel_filter,
                 &get_psb, (struct show_buffer *) 0, (int32 *) 0,
                 &large_msg_area_len, &large_msg_data_len, (char *) 0 );

      switch ( dmq_status ) {
      case PAMS__SUCCESS :
        (void)printf( "Got message:\t\"%s\" in buffer of length %ld\n", 
                msg_area, large_msg_area_len );
        break;

      case PAMS__NOMOREMSG :
        /*  Status returned when queue is empty  */
        break;

      default :
        (void)printf( "Error getting message; status returned is %ld.\n",
            dmq_status );
        break;
    }
    }

  if ( dmq_status == PAMS__NOMOREMSG )
    dmq_status = PAMS__SUCCESS;

  return ( dmq_status );

} /* end of ExampleGet  */



void
ExampleExit( )
{
  int32    dmq_status;


  dmq_status    = pams_exit();

  if ( dmq_status == PAMS__SUCCESS )
    (void)printf( "\nExited from MessageQ.\n" );
  else
    (void)printf( "Error exiting; status returned is %ld.\n", dmq_status );

} /*  end of ExampleExit  */
