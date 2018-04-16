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
**    FILE:           x_select.c
**
**    DESCRIPTION:    Illustrates the selective reception of messages using
**                    pams_set_select() to build a complex message selection
**                    filter.
**
**    CREATION DATE:  10 January 1994
**
**    REQUIREMENTS:   The queue named "EXAMPLE_Q_1" must be defined in your
**                    init file as a primary queue.
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
/**          Constants        **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  50


/**  .......................  **/
/**      Message mclasses     **/
/**  .......................  **/
#define FIRST_CLASS     1
#define SECOND_CLASS    2
#define THIRD_CLASS     3

/**  .......................  **/
/**       Message types       **/
/**  .......................  **/
#define EVEN            0
#define ODD             1

/**  .......................  **/
/**           Structures      **/
/**  .......................  **/
typedef struct _Msg {
  char  buffer[MAX_MESSAGE_SIZE];
  short size;
  char  priority;
  short mclass;
  short type;
} Msg;

/**  .......................  **/
/**    Function Prototypes    **/
/**  .......................  **/
int32  ExampleAttach(q_address*);
int32  ExamplePut(q_address*);
int32  ExampleSetSelect(int32*);
int32  ExampleGetFilter(int32*);
void   ExampleExit( );
char   *ClassName(int);
char   *TypeName(int);
char   *PriorityName(char);
void    MsgPrint(char *header, Msg *msg);


long
main( )
{
    int32        selection_handle;
    q_address    my_primary_queue;

    (void)printf( "\nSelect example started...\n\n" );

    /*  Attach to MessageQ  */
    if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
        exit( -1 );

    /*  Put some messages in my own queue  */
    (void)ExamplePut( &my_primary_queue );

    /*  Set up a selection filter to selectively get some of the messages   */
    (void)ExampleSetSelect( &selection_handle );

    /*  Now get only those messsages which match the filter  */
    (void)ExampleGetFilter( &selection_handle );

    /*  Exit from MessageQ  */
    ExampleExit();

    (void)printf( "\nSelect example complete.\n" );

    return( PAMS__SUCCESS );

} /*  end of main  */


int32
ExampleAttach(q_address    *q_addr)
{
    int32        attach_mode;
    int32        dmq_status;
    int32        q_name_len;
    int32        q_type;
    static char  q_name[12];

   /*
   **  Attach by name to the primary queue "EXAMPLE_Q_1"
   */
   attach_mode = PSYM_ATTACH_BY_NAME;
   q_type      = PSYM_ATTACH_PQ;
   (void)strcpy( q_name, "EXAMPLE_Q_1" );
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
    int32       dmq_status;
    int32       timeout;
    struct PSB  put_psb;
    static Msg  MsgList[] = {
    /* buffer            size  priority  mclass        type
       ------            ----  --------  -----        ----  */
    { "first message",   0,   '\0',     FIRST_CLASS,  ODD },
    { "second message",  0,   '\0',     SECOND_CLASS, EVEN },
    { "third message",   0,   '\0',     THIRD_CLASS,  ODD },
    { "fourth message",  0,   '\0',     FIRST_CLASS,  EVEN },
    { "fifth message",   0,   '\0',     SECOND_CLASS, ODD },
    { "sixth message",   0,   '\1',     THIRD_CLASS,  EVEN },
    { "seventh message", 0,   '\1',     FIRST_CLASS,  ODD },
    { "eighth message",  0,   '\1',     SECOND_CLASS, EVEN },
    { "ninth message",   0,   '\1',     THIRD_CLASS,  ODD },
    { "tenth message",   0,   '\1',     FIRST_CLASS,  EVEN } };

    /*
    **  Put the messages in MessageList to the queue passed in
    */
    (void)printf( "\nPutting messages:\n" );
    delivery  = PDEL_MODE_NN_MEM;  /* No Notification and nonrecoverable   */
    timeout   = 100;               /* Wait 10 seconds before giving up     */
    uma       = PDEL_UMA_DISCL;    /* If can't deliver it, DISCard and Log */

    for ( i = 0; i < 10; i++ ) {
        MsgList[i].size = (short) strlen( MsgList[i].buffer );
        dmq_status      = pams_put_msg( MsgList[i].buffer,
                               &MsgList[i].priority,
                               q_addr,        /* passed in */
                               &MsgList[i].mclass, &MsgList[i].type,
                               &delivery, &MsgList[i].size, &timeout,
                               &put_psb, &uma, (q_address *) 0,
                               (int32 *) 0, (char *) 0, (char *) 0 );
      
        if ( dmq_status == PAMS__SUCCESS )
            MsgPrint( "\n\tPut Message:", &MsgList[i] );
        else
            (void)printf( "\nError putting message %d; status returned is: %ld.\n", i,
                     dmq_status );
    }

    (void)printf("\n");

    return ( dmq_status );

} /*  end of ExamplePut  */



int32
ExampleSetSelect(int32            *selection_handle)
{
    short                      num_masks;
    int32                      dmq_status;
    selection_array_component *sel_filter;
    selection_array_component *sel_mask;

    /*
    **  Setup the selection filter to select only:
    **    messages of priority 1 OR
    **    messages of priority 0 and of mclass FIRST_CLASS  OR
    **    messages of any priority and of mclass SECOND_CLASS and of type EVEN
    */
    (void)printf ( "Setting select filter to select:\n" );
    (void)printf ( "\tmessages of priority 1 OR\n" );
    (void)printf ( "\tmessages of priority 0 and class = FIRST_CLASS OR\n" );
    (void)printf (
      "\tmessages of any priority and class = SECOND_CLASS and type = EVEN\n" );

    /*  Allocate and initialize space for 3 masks  */
    num_masks  = 3;
    sel_filter = (selection_array_component *)calloc((size_t)num_masks, sizeof( selection_array_component ) );
    sel_mask   = sel_filter;

    /*  Set up the first mask to select only messages of priority 1  */
    sel_mask->priority            = PSEL_PRI_P1;
    sel_mask++;

    /*  Set up the second mask to select normal priority FIRST_CLASS messages  */
    sel_mask->priority            = PSEL_PRI_P0;
    sel_mask->key_1_offset        = PSEL_CLASS;
    sel_mask->key_1_size          = 2;
    sel_mask->key_1_value         = FIRST_CLASS;
    sel_mask->key_1_oper          = PSEL_OPER_EQ;
    sel_mask++;

    /*  Set up the third mask to select EVEN, SECOND CLASS messages  */
    sel_mask->priority = PSEL_PRI_ANY;

    sel_mask->key_1_offset        = PSEL_CLASS;
    sel_mask->key_1_size          = 2;
    sel_mask->key_1_value         = SECOND_CLASS;
    sel_mask->key_1_oper          = PSEL_OPER_EQ;

    sel_mask->key_2_offset        = PSEL_TYPE;
    sel_mask->key_2_size          = 2;
    sel_mask->key_2_value         = EVEN;
    sel_mask->key_2_oper          = PSEL_OPER_EQ;

    dmq_status = pams_set_select(
                      sel_filter,
                      &num_masks,
                      selection_handle );

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "Selection filter set.\n" );
    else
        (void)printf( "Unexpected error in ExampleSetSelect: %ld.\n", dmq_status );

    return ( dmq_status );

} /*  end of ExampleSetSelect  */



int32
ExampleGetFilter(int32              *selection_handle)
{
    char               priority;
    short              msg_area_len;
    int32              dmq_status;
    int32              show_buf_len;
    q_address          msg_source;
    struct PSB         get_psb;
    struct show_buffer show_buf;
    Msg                msg;

    /*
    **  Get all the messages currently in the queue and print them out
    */
    (void)printf( "\n" );   
    priority     = 0;  /*  get messages of both high and normal priorities */
    msg_area_len = MAX_MESSAGE_SIZE;
    show_buf_len = sizeof( struct show_buffer );
    dmq_status   = PAMS__SUCCESS;

    while ( dmq_status == PAMS__SUCCESS )
    {
        /*  Initialize the message area to hold the message  */
        (void)memset((void*)msg.buffer, '\0', MAX_MESSAGE_SIZE );

        /*  Get a message  */
        dmq_status = pams_get_msg(msg.buffer, &priority, &msg_source,
                          &msg.mclass, &msg.type, &msg_area_len, &msg.size,
                          selection_handle, &get_psb, &show_buf,
                          &show_buf_len, (int32 *) 0, (int32 *) 0, (char *) 0);

        switch ( dmq_status ) {
            case PAMS__SUCCESS :
                msg.priority = (char) show_buf.priority;
                MsgPrint( "\nGot message:", &msg );
            break;

            case PAMS__NOMOREMSG :
                /*  Status returned when queue is empty  */
            break;

            default :
                (void)printf( "\nError getting message; status returned is %ld.\n",
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
        (void)printf( "\nError exiting; status returned is %ld.\n", dmq_status );

} /*  end of ExampleExit  */


char *
ClassName(int      val)
{
    switch ( val )
    {
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
TypeName(int     val)
{
    switch ( val )
    {
        case EVEN :
            return ( "EVEN" );
      
        case ODD :
            return ( "ODD" );

        default :
            return ( "UNKNOWN" );
    }

} /*  end of TypeName  */



char *
PriorityName(char       the_priority)
{
    static char *pnames[] = { "NORMAL", "HIGH" };

    return ( pnames[the_priority] );

} /*  end of PriorityName  */



void
MsgPrint(char *header, Msg *msg)
{
    if ( header )
        (void)printf( "%s\n", header );

    (void)printf( "\tText:\t\"%s\"\n", msg->buffer );
    (void)printf( "\tPriority:\t%s\n", PriorityName(msg->priority) );
    (void)printf( "\tClass:\t%s\n", ClassName((int)msg->mclass) );
    (void)printf( "\tType:\t%s\n", TypeName((int)msg->type) );

} /*  end of MsgPrint  */
