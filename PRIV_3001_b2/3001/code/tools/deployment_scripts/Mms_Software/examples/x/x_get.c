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
**    FILE:          x_get.c
**
**    DESCRIPTION:   Example program that highlights using pams_get_msg()
**                   with selection by mclass and type of message.
**                   This example does only simple selections (i.e. ones
**                   that DON'T require the use of pams_set_select()).
**
**    CREATION DATE: 10 January 1994
**
**    REQUIREMENTS:  The queue named "EXAMPLE_Q_1" must be defined in your
**                   init file as a primary queue.
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

/**  ...............  **/
/**  Local constants  **/
/**  ...............  **/
#define MAX_MESSAGE_SIZE    50

/**  ...............  **/
/**  Message mclasses  **/
/**  ...............  **/
#define FIRST_CLASS     1
#define SECOND_CLASS    2
#define THIRD_CLASS     3

/**  ...............  **/
/**  Message types    **/
/**  ...............  **/
#define EVEN            0
#define ODD             1

/**  ...............  **/
/**    Structures     **/
/**  ...............  **/
typedef struct _Msg
{
   char  buffer[MAX_MESSAGE_SIZE];
   short size;
   short mclass;
   short type;
} Msg;

/**  ............................  **/
/**  Function Forward Declarations **/
/**  ............................  **/
int32 ExampleAttach(q_address   *q_addr);
int32 ExamplePut(q_address   *q_addr);
int32 ExampleGetByClass(void);
int32 ExampleGetByType(void);
void  ExampleExit(void);
char  *ClassName(int val);
char  *TypeName(int val);
void  MsgPrint(char *header, Msg *msg);


long
main( )
{
   int32       dmq_status;
   q_address   my_primary_queue;

   (void)printf( "\nX_Get Select Example starting...\n" );

   /*  Attach to MessageQ  */
   if (ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
       exit(0);

   /*  Put some messages into my own queue  */
   dmq_status = ExamplePut( &my_primary_queue );

   /*  Get them back by mclass  */
   dmq_status = ExampleGetByClass();

   /*  Put the messages back into my queue  */
   dmq_status = ExamplePut( &my_primary_queue );

   /*  Now get them back by type  */
   dmq_status = ExampleGetByType();

   /*  Exit from MessageQ  */
   ExampleExit();

   (void)printf("\n\nPams_Get_msg Example complete.....\n" );
   return( dmq_status );

} /*  end of main  */



int32
ExampleAttach(q_address   *q_addr)
{
   int32       attach_mode;
   int32       dmq_status;
   int32       q_name_len;
   int32       q_type;
   static char q_name[12];

   /*  Attach by name to the primary queue "EXAMPLE_Q_1" */
   attach_mode = PSYM_ATTACH_BY_NAME;
   q_type      = PSYM_ATTACH_PQ;
   (void)strcpy(q_name,"EXAMPLE_Q_1");
   q_name_len  = (int32) strlen( q_name );
  
   dmq_status  = pams_attach_q(&attach_mode, q_addr, &q_type, q_name,
                      &q_name_len,
                      (int32 *) 0,    /*  Use default name space  */
                      (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

    if ( dmq_status == PAMS__SUCCESS )
         (void)printf( "Attached to queue: \"%s\"\n", q_name );
    else
         (void)printf("Error attaching to queue: \"%s\"; status returned is: %ld.\n", 
                 q_name, dmq_status);

   return (dmq_status);

} /*  end of ExampleAttach  */


  
int32
ExamplePut(q_address   *q_addr)
{
   int         i;
   char        priority;
   char        delivery;
   char        uma;
   int32       dmq_status;
   int32       timeout;
   struct PSB  put_psb;
   static Msg  MsgList[] = {
   /*  buffer            size  mclass         type
       ------            ----  -----         ----  */
    { "first message",   0,   FIRST_CLASS,  ODD },
    { "second message",  0,   SECOND_CLASS, EVEN },
    { "third message",   0,   THIRD_CLASS,  ODD },
    { "fourth message",  0,   FIRST_CLASS,  EVEN },
    { "fifth message",   0,   SECOND_CLASS, ODD },
    { "sixth message",   0,   THIRD_CLASS,  EVEN },
    { "seventh message", 0,   FIRST_CLASS,  ODD },
    { "eighth message",  0,   SECOND_CLASS, EVEN },
    { "ninth message",   0,   THIRD_CLASS,  ODD },
    { "tenth message",   0,   FIRST_CLASS,  EVEN } };


    /*
    **    Put the messages in MessageList to the queue passed in
    */
    (void)printf("\n\nPutting messages:\n");
    priority   = 0;                 /* Regular priority; use 0, NOT '0'     */
    delivery   = PDEL_MODE_NN_MEM;  /* No Notification and nonrecoverable   */
    timeout    = 100;               /* Wait 10 seconds before giving up     */
    uma        = PDEL_UMA_DISCL;    /* If can't deliver it, DISCard and Log */

    for ( i = 0; i < 10; i++ )
    {
      MsgList[i].size = (short) strlen( MsgList[i].buffer );

      dmq_status = pams_put_msg(
                           MsgList[i].buffer,
                           &priority,
                           q_addr,                    /* passed in */
                           &MsgList[i].mclass,
                           &MsgList[i].type,
                           &delivery,
                           &MsgList[i].size,
                           &timeout,
                           &put_psb,
                           &uma,
                           (q_address *) 0, 
                           (int32 *) 0,
                           (char *) 0,
                           (char *) 0 );
      
      if ( dmq_status == PAMS__SUCCESS )
            MsgPrint( "\n\tPut Message:", &MsgList[i] );
      else
            (void)printf("\nError putting message %d; status returned is: %ld.\n",
                      i,dmq_status);
    }

    (void)printf("\n");
    return (dmq_status);

} /*  end of ExamplePut  */


int32
ExampleGetByClass( )
{
   char       priority;
   short      msg_area_len;
   short      sel_filter[2];
   int        i;
   int32      dmq_status;
   q_address  msg_source;
   Msg        msg;

   (void)memset((void*)&msg, 0, sizeof(Msg));
   /*
   **    Get the FIRST_CLASS, then SECOND_CLASS,
   **     then THIRD_CLASS messages and print them out.
   */
   for ( i = FIRST_CLASS; i <= THIRD_CLASS; i++ ) {
      (void)printf( "\nGetting %s messages\n", ClassName((int)i));

      /* Set up sel_filter to only get messages of the specified mclass */
      sel_filter[0] = i;
      sel_filter[1] = PSEL_PQ_CLASS;  

      /*  Get all priority messages  */
      priority = 0;    

      msg_area_len   = MAX_MESSAGE_SIZE;
      dmq_status     = PAMS__SUCCESS;

      while ( dmq_status == PAMS__SUCCESS ) {
         /*  Initialize the message area to hold the message  */
         (void)memset((void*)msg.buffer, '\0', MAX_MESSAGE_SIZE );

         /*  Get a message  */
         dmq_status = pams_get_msg(msg.buffer, &priority, &msg_source,
                              &msg.mclass, &msg.type, &msg_area_len, &msg.size,
                              (int32 *) sel_filter, (struct PSB *) 0,
                              (struct show_buffer *) 0, (int32 *) 0,
                              (int32 *) 0, (int32 *) 0, (char *) 0 );
         switch ( dmq_status ) {
            case PAMS__SUCCESS :
               MsgPrint( "\n\tGot message:", &msg );
            break;

            case PAMS__NOMOREMSG :
               /*  Status returned when queue is empty  */
            break;

            default :
               (void)printf( "\nError getting message; status returned is %ld.\n",
                        dmq_status );
            break;
         } /* end case */
      } /* end while */
   } /* end for */

   (void)printf("\n");
   if (dmq_status == PAMS__NOMOREMSG)
        return (PAMS__SUCCESS);
    else
        return (dmq_status);

} /* end of ExampleGetByClass  */


int32
ExampleGetByType( )
{
   char       priority;
   short      msg_area_len;
   short      sel_filter[2];
   int        i;
   int32      dmq_status;
   q_address  msg_source;
   Msg        msg;
   
   (void)memset((void*)&msg, 0, sizeof(Msg));
   /*
   **  Get the messages of type EVEN then messages of type ODD
   **    and print them out.
   */
   for ( i = EVEN; i <= ODD; i++ ) {
      (void)printf( "\nGetting %s messages\n", TypeName((int)i) );

      /*
      **  Set the selection filter to select only those messages which
      **  have the same type as we are looking for.
      */
      sel_filter[0]  = i;
      sel_filter[1]  = PSEL_PQ_TYPE;

      priority       = 0;   
      msg_area_len   = MAX_MESSAGE_SIZE;
      dmq_status     = PAMS__SUCCESS;

      while ( dmq_status == PAMS__SUCCESS ) {
         /*  Initialize the message area to hold the message  */
         (void)memset((void*)msg.buffer, '\0', MAX_MESSAGE_SIZE );

         /*  Get a message  */
         dmq_status = pams_get_msg(msg.buffer, &priority, &msg_source,
                              &msg.mclass, &msg.type, &msg_area_len, &msg.size,
                              (int32 *) sel_filter, (struct PSB *) 0,
                              (struct show_buffer *) 0, (int32 *) 0,
                              (int32 *) 0, (int32 *) 0, (char *) 0 );

         switch ( dmq_status ) {
            case PAMS__SUCCESS :
               MsgPrint( "\n\tGot message:", &msg );
            break;

            case PAMS__NOMOREMSG :
               /*  Status returned when queue is empty  */
            break;

            default :
               (void)printf("\nError getting message; status returned is %ld.\n",
                       dmq_status);
            break;
         }/* end case */
      } /* end while */
   } /* end for */

   (void)printf("\n");

   if (dmq_status == PAMS__NOMOREMSG)
        return (PAMS__SUCCESS);
    else
        return (dmq_status);

} /* end of ExampleGetByType  */



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



char *
ClassName(int val)
{
    switch ( val ) {
        case FIRST_CLASS :
            return ( "FIRST" );
      
        case SECOND_CLASS :
            return ( "SECOND" );

        case THIRD_CLASS :
            return ( "THIRD" );
    
        default :
            return ( "UNKNOWN" );
    }
} /*  end of ClassName  */



char *
TypeName(int val)
{
    switch ( val ) {
        case EVEN :
          return ( "EVEN" );
      
        case ODD :
            return ( "ODD" );

        default :
            return ( "UNKNOWN" );
    }
} /*  end of TypeName  */



void
MsgPrint(char *header, Msg *msg)
{
    if ( header )
        (void)printf( "%s\n", header );

    (void)printf( "\tText:\t\"%s\"\n", msg->buffer );
    (void)printf( "\tClass:\t%s\n", ClassName((int)msg->mclass) );
    (void)printf( "\tType:\t%s\n", TypeName((int)msg->type) );

} /*  end of MsgPrint  */
