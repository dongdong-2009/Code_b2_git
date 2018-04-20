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
**    FILE:           x_getem.c
**
**    DESCRIPTION:    Example program that highlights the use of pams_get_msg().
**                    This will attach to "EXAMPLE_Q_1", put messages of different
**                    priorities, mclasses, and types into it's own queue, then
**                    gets them back in a variety of ways: simple FIFO, by
**                    priority, by mclass, and by type.
**
**    CREATION DATE:  10 January 1994
**
**    REQUIREMENTS:   The queue named "EXAMPLE_Q_1" must be defined in your init
**                    file as a primary queue.
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
/**        Constants          **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  25

typedef enum _MsgOrder 
   { FIFO, BY_PRIORITY, BY_CLASS, BY_TYPE } MsgOrder;

typedef enum _MsgClass 
   { FIRST=1, SECOND, THIRD, FOURTH, FIFTH, NO_CLASS } MsgClass;

typedef enum _MsgType  
   { EVEN, ODD, NO_TYPE } MsgType;



/**  .......................  **/
/**        Structures         **/
/**  .......................  **/
typedef struct _Msg {
  char      buffer[MAX_MESSAGE_SIZE];
  short     size;
  char      priority;
  short     mclass;
  short     type;
} Msg;

typedef struct _MsgSelection {
  MsgOrder  order;
  short     mclass;
  short     type;
  char      priority;
} MsgSelection;


/**  .............................. **/
/**   Function Forward Declarations **/
/**  .............................. **/
int32 ExampleAttach(q_address *q_addr);
int32 ExampleGet( );
int32 ExamplePut(q_address *q_addr);
void  ExampleExit(void);
int32 ExampleFIFO(q_address *q_addr);
int32 ExampleByPriority(q_address *q_addr);
int32 ExampleByClass(q_address *q_addr);
int32 ExampleByType(q_address *q_addr);
int32 MsgGet(MsgSelection *msg_sel, Msg *msg);
int32 MsgPut(Msg *msg, q_address *q_addr);
void  MsgPrint(char *header, Msg *msg);
char  *ClassName(short);
char  *TypeName(short);

  

long
main( )
{
    q_address    my_primary_queue;

    /*  Attach to MessageQ  */
    if ( ExampleAttach( &my_primary_queue ) != PAMS__SUCCESS )
        exit( 0 );

    /*  Put the messages into my own queue and get them back in FIFO order  */
    if ( ExampleFIFO( &my_primary_queue ) != PAMS__SUCCESS ) {
        ExampleExit();
        exit( 0 );
    }

    /*  Put the messages back into the queue and get the high priority
    **  messages then the normal priority messages.
    */
    if ( ExampleByPriority( &my_primary_queue ) != PAMS__SUCCESS ) {
        ExampleExit();
        exit( 0 );
    }


    /*  Put the messages into the queue again and get them back by mclass  */
    if ( ExampleByClass( &my_primary_queue ) != PAMS__SUCCESS )
    {
        ExampleExit();
        exit( 0 );
    }


    /*  One last time... put the messages into my queue and get them by type  */
    if ( ExampleByType( &my_primary_queue ) != PAMS__SUCCESS ) {
        ExampleExit();
        exit( 0 );
    }


    /*  Exit from MessageQ  */
    ExampleExit();
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
    attach_mode  = PSYM_ATTACH_BY_NAME;
    q_type       = PSYM_ATTACH_PQ;
    (void)strcpy( q_name, "EXAMPLE_Q_1" );
    q_name_len   = (int32) strlen( q_name );
  
    dmq_status   = pams_attach_q( &attach_mode, q_addr, &q_type, q_name,
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
    int         i;
    static Msg  MsgList[] = {
    /*   message text       size  priority  mclass   type 
         ------------       ----  --------  -----   ---- */
    { "first message",   0,    0,        (short)FIRST,  (short)ODD  },
    { "second message",  0,    0,        (short)SECOND, (short)EVEN },
    { "third message",   0,    0,        (short)THIRD,  (short)ODD  },
    { "fourth message",  0,    0,        (short)FOURTH, (short)EVEN },
    { "fifth message",   0,    0,        (short)FIFTH,  (short)ODD  },
    { "sixth message",   0,    1,        (short)FIRST,  (short)EVEN },
    { "seventh message", 0,    1,        (short)SECOND, (short)ODD  },
    { "eighth message",  0,    1,        (short)THIRD,  (short)EVEN },
    { "ninth message",   0,    1,        (short)FOURTH, (short)ODD  },
    { "tenth message",   0,    1,        (short)FIFTH,  (short)EVEN } };


    /*
    **  Put the messages in MsgList to the queue passed in
    */
    (void)printf( "\n" );

    for ( i = 0; i < 10; i++ ) {
        /*  Set the real size  */
        MsgList[i].size = (short) strlen( MsgList[i].buffer );

        /*  Put the message to the queue passed in  */
        if ( MsgPut( &MsgList[i], q_addr ) == PAMS__SUCCESS )
            MsgPrint( "Put message", &MsgList[i] );
        else
            break;
    }

    return(0);

} /*  end of ExamplePut  */



void
ExampleExit( )
{
    int32   dmq_status;

    /*
    **  Exit from MessageQ, that is, detach from the bus
    */
    dmq_status = pams_exit();

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\nExited from MessageQ.\n" );
    else
        (void)printf( "Error exiting; status returned is %ld.\n", dmq_status );

} /*  end of ExampleExit  */



int32
ExampleFIFO(q_address *q_addr)
{
    int32           dmq_status;
    MsgSelection    msg_sel;
    Msg             the_msg;


    /*  Get the messages back FIFO  */
    msg_sel.order    = FIFO;
    msg_sel.mclass    = msg_sel.type = 0;
    msg_sel.priority = 0;


    /*  First flush the queue of any existing messages  */
    dmq_status = PAMS__SUCCESS;
    while ( dmq_status == PAMS__SUCCESS ) {
        dmq_status = MsgGet( &msg_sel, &the_msg);
    }

    /*  Put the messages into the queue  */
    dmq_status = ExamplePut( q_addr );
    if ( dmq_status != PAMS__SUCCESS )
        return ( dmq_status );

    dmq_status = PAMS__SUCCESS;
    while ( dmq_status == PAMS__SUCCESS ) {
        dmq_status = MsgGet( &msg_sel, &the_msg );
        if ( dmq_status == PAMS__SUCCESS )
        MsgPrint( "Got Message:", &the_msg );
    }
  
    return ( PAMS__SUCCESS );

} /*  end of ExampleFIFO  */



int32
ExampleByPriority(q_address       *q_addr)
{
    int32           dmq_status;
    MsgSelection    msg_sel;
    Msg             the_msg;


    /*  Put the messages into the queue  */
    dmq_status = ExamplePut( q_addr );
    if ( dmq_status != PAMS__SUCCESS )
        return ( dmq_status );

    /*  Get the high priority messages back  */
    msg_sel.order    = BY_PRIORITY;
    msg_sel.priority = 1;
    msg_sel.mclass    = msg_sel.type = 0;

    dmq_status = PAMS__SUCCESS;
    while ( dmq_status == PAMS__SUCCESS ) {
        dmq_status = MsgGet( &msg_sel, &the_msg );
        if ( dmq_status == PAMS__SUCCESS )
        MsgPrint( "Got Message:", &the_msg );
    }

    /*  Now get the rest of the messages back  */
    msg_sel.order    = BY_PRIORITY;
    msg_sel.priority = 0;
    msg_sel.mclass    = msg_sel.type = 0;

    dmq_status = PAMS__SUCCESS;
    while ( dmq_status == PAMS__SUCCESS ) {
        dmq_status = MsgGet( &msg_sel, &the_msg );
        if ( dmq_status == PAMS__SUCCESS )
        MsgPrint( "Got Message:", &the_msg );
    }

    return ( PAMS__SUCCESS );

} /*  end of ExampleByPriority  */



int32
ExampleByClass(q_address      *q_addr)
{
    int32          dmq_status;
    MsgSelection   msg_sel;
    Msg            the_msg;
    MsgClass       msg_class;


    /*  Put the messages into the queue  */
    dmq_status = ExamplePut( q_addr );
    if ( dmq_status != PAMS__SUCCESS )
        return ( dmq_status );

    /*  Walk through all the mclasses getting all the messages in each mclass  */
    msg_class = FIRST;
    while ( msg_class != NO_CLASS )
    {
        msg_sel.order    = BY_CLASS;
        msg_sel.mclass    = (short) msg_class;
        msg_sel.type     = 0;
        msg_sel.priority = 0;
        dmq_status       = PAMS__SUCCESS;

        while ( dmq_status == PAMS__SUCCESS ) {
            dmq_status = MsgGet( &msg_sel, &the_msg );
            if ( dmq_status == PAMS__SUCCESS )
                MsgPrint( "Got Message:", &the_msg );
        } 
        msg_class = (MsgClass) ((int) msg_class + 1);
    }

    return ( PAMS__SUCCESS );

} /*  end of ExampleByClass  */



int32
ExampleByType(q_address       *q_addr)
{
    int32           dmq_status;
    MsgSelection    msg_sel;
    Msg             the_msg;
    MsgType         msg_type;


    /*  Put the messages into the queue  */
    dmq_status = ExamplePut( q_addr );
    if ( dmq_status != PAMS__SUCCESS )
        return ( dmq_status );


    /*  Walk through all the types getting all the messages of each type  */
    msg_type = EVEN; 
    while ( msg_type != NO_TYPE )
    {
        msg_sel.order    = BY_TYPE;
        msg_sel.type     = (short) msg_type;
        msg_sel.mclass    = 0;
        msg_sel.priority = 0;
        dmq_status       = PAMS__SUCCESS;

        while ( dmq_status == PAMS__SUCCESS ) {
            dmq_status = MsgGet( &msg_sel, &the_msg );
            if ( dmq_status == PAMS__SUCCESS )
                MsgPrint( "Got Message:", &the_msg );
        }
        msg_type = (MsgType) ( (int) msg_type + 1 );
    }

    return ( PAMS__SUCCESS );

} /*  end of ExampleByType  */



int32
MsgGet(MsgSelection *msg_sel, Msg *msg)
{
    int32     dmq_status;
    short     msg_area_len;
    short     sel_filter[2];
    char      priority;
    q_address source;


    /*
    **  Setup the get call according to the message selection passed in 
    */
    switch ( msg_sel->order )
    {
        case FIFO :
            /*  Get messages in FIFO order; disregard priority, mclass, or type  */
            priority      = 0;         
            sel_filter[0] = 0;    
            sel_filter[1] = 0;
        break;

        case BY_PRIORITY :
            /*  Get high priority messages only  */
            priority      = msg_sel->priority;
            sel_filter[0] = 0;    
            sel_filter[1] = 0;
        break;

        case BY_CLASS :
            /*  Get messages by mclass  */
            priority = 0;         
            sel_filter[0] = msg_sel->mclass;
            sel_filter[1] = PSEL_PQ_CLASS;
        break;

        case BY_TYPE :
            priority = 0;         
            sel_filter[0] = msg_sel->type;
            sel_filter[1] = PSEL_PQ_TYPE;
        break;

        default :
            return ( -1 );
    }

    msg_area_len = MAX_MESSAGE_SIZE;
    (void)memset((void*)msg->buffer, '\0', MAX_MESSAGE_SIZE );
    msg->mclass   = msg->type = 0;
    msg->size    = 0;

    dmq_status   = pams_get_msg(
                        msg->buffer,    /*  where msg is written   */
                        &priority,      /*  get msgs of this pri   */
                        &source,        /*  q_address of sender    */
                        &msg->mclass,    /*  mclass of received msg  */
                        &msg->type,     /*  type of received msg   */
                        &msg_area_len,  /*  size of message buffer */
                        &msg->size,     /*  actual size of the msg */
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
            (void)printf( "Get Message returned PAMS__SUCCESS\n\n" );
        break;

        case PAMS__NOMOREMSG : /*  Status returned when queue is empty  */
            (void)printf( "Get Message returned PAMS_NOMOREMSG\n\n" );
        break;

        default :
            (void)printf( "Error getting message; status returned is %ld.\n\n", dmq_status );
        break;
    }

    return ( dmq_status );

} /*  end of MsgGet  */



int32
MsgPut(Msg *msg, q_address *q_addr)
{
    int32    dmq_status;
    int32    timeout;
    char     delivery;


    delivery   = PDEL_MODE_NN_MEM;    /* No Notification and nonrecoverable */
    timeout    = 100;                 /* Wait 10 seconds before giving up   */
  
    dmq_status = pams_put_msg(
                      msg->buffer,
                      &msg->priority,
                      q_addr,       
                      &msg->mclass,
                      &msg->type,
                      &delivery,
                      &msg->size,
                      &timeout,
                      (struct PSB *) 0,  
                      (char *) 0,
                      (q_address *) 0,
                      (int32 *) 0,
                      (char *) 0,
                      (char *) 0 );
      
    if ( dmq_status != PAMS__SUCCESS )
        (void)printf( "Error putting message; status returned is: %ld.\n", dmq_status);

    return ( dmq_status );

} /*  end of MsgPut  */


void
MsgPrint(char *header, Msg *msg)
{
    /*  Print out the message  */
    if ( header )
        (void)printf( "%s\n", header );

    /*  This assumes the message is a string  */
    (void)printf( "\ttext:\t\"%s\"\n", msg->buffer );
    (void)printf( "\tpriority:\t%s\n", (msg->priority == 0) ? "normal" : "high" );
    (void)printf( "\tclass:\t%s\n", ClassName((short)msg->mclass) );
    (void)printf( "\ttype:\t%s\n", TypeName((short)msg->type) );

} /*  end of MsgPrint  */


char *
ClassName(short the_class)
{
    static char  *mclass_names[] =
                   { "", "FIRST", "SECOND", "THIRD", 
                     "FOURTH", "FIFTH", "NO_CLASS" };

    return ( mclass_names[the_class] );

} /*  end of ClassName  */


char *
TypeName(short     the_type)
{
    static char  *type_names[] =
                    { "EVEN", "ODD", "NO_TYPE" };

    return ( type_names[the_type] );

} /*  end of TypeName  */
