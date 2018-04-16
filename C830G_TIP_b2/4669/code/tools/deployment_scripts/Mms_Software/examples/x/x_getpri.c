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
**    FILE:          x_getpri.c
**
**    DESCRIPTION:   Illustrates using the normal and high priority
**                   messages with pams_get_msg().
**
**    CREATION DATE: 10 January 1994
**
**    REQUIREMENTS:  The queue named "EXAMPLE_Q_1" must be defined in your
**                   init file as a primary queue.
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
#define MAX_MESSAGE_SIZE   50
#define ALL                 0
#define HIGH                1
#define NORMAL              2

#ifndef TRUE   
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif
  

/**  .......................  **/
/**         Structures        **/
/**  .......................  **/
typedef struct _Msg {
   char  buffer[MAX_MESSAGE_SIZE];
   char  priority;
} Msg;


/**  ..............................  **/
/**    Function Forward Declarations **/
/**  ..............................  **/
int32 ExampleAttach(q_address *q_addr);
int32 ExamplePut(q_address *);
int32 ExampleGetPriority(int, int );
void  ExampleExit(void);
char  *PriorityName(int);



long
main( )
{
   q_address   my_primary_queue;

   (void)printf( "\nGet by priority example started...\n\n" );

   /*  Attach to MessageQ  */
   if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
       exit( 0 );

   /*  Put messages of normal and high priority into my own queue  */
   (void)ExamplePut( &my_primary_queue );

   /* 
   **  Get a few messages regardless of priority. Since we have
   **  both high and normal priority messages in the queue we
   **  should get the high priority messages first.
   */
   (void)ExampleGetPriority( ALL, 2 );

   /*  Get a few of the normal priority messages  */
   (void)ExampleGetPriority( NORMAL, 2 );

   /*  Get the rest of the high priority messages  */
   (void)ExampleGetPriority( HIGH, 2 );

   /*  Get the rest of the messages  */
   (void)ExampleGetPriority( ALL, 4 );

   /*  Exit from MessageQ  */
   ExampleExit();

   (void)printf( "\nGet by priority example complete.\n" );

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
   (void)strcpy(q_name, "EXAMPLE_Q_1" );
   attach_mode = PSYM_ATTACH_BY_NAME;
   q_type      = PSYM_ATTACH_PQ;
   q_name_len  = (int32) strlen( q_name );
  
   dmq_status  = pams_attach_q(&attach_mode, q_addr, &q_type, q_name,
                        &q_name_len,
                        (int32 *) 0,    /*  Use default name space  */
                        (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "Attached to queue: \"%s\".\n", q_name );
   else
      (void)printf( "Error attaching to queue: \"%s\"; status returned is: %ld.\n", 
               q_name, dmq_status );

   return ( dmq_status );

} /*  end of ExampleAttach  */


  
int32
ExamplePut(q_address   *q_addr)
{
   int         i;
   char        delivery;
   char        uma;
   short       msg_class;
   short       msg_type;
   short       msg_size;
   int32       dmq_status;
   int32       timeout;
   struct PSB  put_psb;

   static Msg  MsgList[] = {
            /* message text      priority
               ------------      --------  */
            { "first message",   0 },
            { "second message",  1 },
            { "third message",   0 },
            { "fourth message",  1 },
            { "fifth message",   0 },
            { "sixth message",   1 },
            { "seventh message", 0 },
            { "eighth message",  1 },
            { "ninth message",   0 },
            { "tenth message",   1 } };


   /*
   **  Put the messages in MessageList to the queue passed in
   */
   (void)printf( "\nPutting messages...\n" );

   msg_class   = msg_type = 0;      /* No class or type at the moment       */
   delivery    = PDEL_MODE_NN_MEM;  /* No Notification and nonrecoverable   */
   timeout     = 100;               /* Wait 10 seconds before giving up     */
   uma         = PDEL_UMA_DISCL;    /* If can't deliver it, DISCard and Log */

   for ( i = 0; i < 10; i++ )
   {
      msg_size      = (short) strlen( MsgList[i].buffer );
      dmq_status    = pams_put_msg(
                            MsgList[i].buffer,
                            &MsgList[i].priority,
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
         (void)printf( "\n\tPut priority %d message:\t%s\n", MsgList[i].priority,
                  MsgList[i].buffer );
      else
         (void)printf( "\nError putting message; status returned is: %ld.\n", 
                  dmq_status );
   }/* end for */

   (void)printf("\n");
   return ( dmq_status );

} /*  end of ExamplePut  */



int32
ExampleGetPriority(int the_priority, int num_msgs)
{
   char                 priority;
   short                msg_class;
   short                msg_type;
   short                msg_area_len;
   short                msg_data_len;
   int                  i;
   int                  done;
   int32                dmq_status;
   int32                show_buffer_len;
   char                 msg_area[MAX_MESSAGE_SIZE];
   int32                sel_filter[2];
   q_address            msg_source;
   struct PSB           get_psb;
   struct show_buffer   show_buffer;

   /*
   **  Attempt to get the number of message of the priority and print them out.
   */
   (void)printf( "\nGetting %d %s priority messages...\n", num_msgs, 
            PriorityName( the_priority ) );   

   switch ( the_priority ) {
      case HIGH :
         priority       = 1;
         sel_filter[0]  = 0;
         sel_filter[1]  = 0;
      break;

      case NORMAL :
         priority       = 0;
         sel_filter[0]  = PSEL_PRI_P0;
         sel_filter[1]  = PSEL_PQ_PRI;
      break;
      
      case ALL:
         priority       = 0;
         sel_filter[0]  = 0;
         sel_filter[1]  = 0;
      break;

      default :
         (void)printf( "\nUnexpected priority encountered - defaulting to ALL\n" );
         priority       = 0;
         sel_filter[0]  = 0;
         sel_filter[1]  = 0;
      break;
   }/* end case */

   msg_area_len      = MAX_MESSAGE_SIZE;
   dmq_status        = PAMS__SUCCESS;
   show_buffer_len   = sizeof( struct show_buffer );

   for ( done = FALSE, i = 0; (i < num_msgs) && ( ! done ); i++ ) {
      /*  Initialize the message area to hold the message  */
      (void)memset((void*)&msg_area[0], '\0', MAX_MESSAGE_SIZE );

      /*  Get a message  */
      dmq_status    = pams_get_msg(msg_area, &priority, &msg_source, &msg_class,
                           &msg_type, &msg_area_len, &msg_data_len,
                           (int32 *)sel_filter, &get_psb, &show_buffer,
                           &show_buffer_len, (int32 *) 0, (int32 *) 0,
                           (char *) 0 );

      switch ( dmq_status ) {
         case PAMS__SUCCESS :
            (void)printf( "\n\tGot message of priority %ld:\t\"%s\"\n", 
                     show_buffer.priority, msg_area );
         break;

         case PAMS__NOMOREMSG :
            /*  Status returned when queue is empty  */
            done = TRUE;
         break;

         default :
            (void)printf( "\nError getting message; status returned is %ld.\n",
                     dmq_status );
            done = TRUE;
         break;

      }/* end case dmq_status */

   }/* end for */

   (void)printf("\n");
   return ( dmq_status );

} /* end of ExampleGetPriority  */



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



char *
PriorityName(int     the_priority)
{
    static char *pnames[] = { "ALL", "HIGH", "NORMAL" };

    return ( pnames[the_priority] );

} /*  end of PriorityName  */
