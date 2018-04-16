/*
** Copyright (c) 1997,1998 BEA Systems, Inc.
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
**    FILE:           x_sdm.c
**
**    DESCRIPTION:    This simple example will illustrate how to build
**                    and interpret a sdm message.
**                    The example simply attaches to "EXAMPLE_Q_1"
**                    puts a message to itself, decodes it and exits.
**
**    CREATION DATE:  07 February 1997
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

/**  .............................. **/
/**   Function Forward Declarations **/
/**  .............................. **/
int32 ExampleEncode(pams_handle msg_handle);
int32 ExampleDecode(pams_handle msg_handle);

long
main( )
{
    int32               attach_mode;
    int32               dmq_status;
    int32               q_name_len;
    int32               q_type;
    int32               timeout;
    short               mclass;
    short               type;
    short               msg_size;
    short               buffer_size;
    char                delivery;
    char                priority;
    char                uma;
    static char         q_name[12];
    q_address           my_queue;
    struct PSB          my_psb;
    pams_handle         msg_handle;
    struct show_buffer  my_show_buffer;
    int32               my_show_buffer_len = sizeof( my_show_buffer );


    /*
    **  Attach to MessageQ  
    */
    attach_mode = PSYM_ATTACH_BY_NAME;
    q_type      = PSYM_ATTACH_PQ;
    (void)strcpy( q_name, "EXAMPLE_Q_1" );
    q_name_len  = (int32) strlen( q_name );

    (void)printf( "\n\tAttaching to queue %s by name...\n", q_name);


    dmq_status = pams_attach_q(&attach_mode, &my_queue, &q_type, q_name,
                      &q_name_len, (int32 *) 0, (int32 *) 0, (int32 *) 0,
                      (char *) 0, (char *) 0 );

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\n\tAttached successfully to queue \"%s\".\n", q_name );
    else
        (void)printf( "\n\tError attaching to queue \"%s\"; status returned is: %d\n", 
                 q_name, dmq_status );

    /*
    **  Encode few elements of various datatypes
    */
    dmq_status = ExampleEncode ( msg_handle );
    if ( dmq_status != PAMS__SUCCESS )
        exit ( 0 );

    /*
    **  Put message into my own queue  
    */
    priority   = 0;
    mclass      = 0;
    type       = 0;
    delivery   = PDEL_MODE_WF_MEM;
    msg_size   = PSYM_MSG_HANDLE;
    timeout    = 50;  /* 5 seconds  */
    uma        = PDEL_UMA_DISCL;

    dmq_status = pams_put_msg(
                      msg_handle,
                      &priority,
                      &my_queue,
                      &mclass,
                      &type,
                      &delivery,
                      &msg_size,
                      &timeout,
                      &my_psb,
                      &uma,
                      (q_address *) 0,
                      (int32 *) 0,
                      (char *) 0,
                      (char *) 0 );

    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\n\tPut successfully to queue: \"%s\".\n", q_name );
    else
        (void)printf( "\n\tError sending to queue: \"%s\"; status returned is: %d\n", 
                 q_name, dmq_status );


    /*
    **  Get message from my own queue  
    */
    buffer_size = PSYM_MSG_HANDLE;
    dmq_status = pams_get_msg(
                      msg_handle,
                      &priority,
                      &my_queue,
                      &mclass,
                      &type,
                      &buffer_size,
                      &msg_size,
                      (int32 *) 0,
                      &my_psb,
                      &my_show_buffer,
                      &my_show_buffer_len,
                      (int32 *) 0,
                      (int32 *) 0,
                      (char *) 0 );

    if ( dmq_status == PAMS__SUCCESS )
    {
        (void)printf( "\n\tGet successfully from queue: \"%s\".\n", q_name );

        if ( my_show_buffer.endian == PSYM_SDM )
        {
            /*
            **  Decode message
            */
            dmq_status = ExampleDecode( msg_handle );
        }
        else
            (void)printf( "\n\tMessage is not an SDM message\n" );
    }
    else
        (void)printf( "\n\tError getting from queue: \"%s\"; status returned is: %d\n", 
                 q_name, dmq_status );


    /*
    **  Detach from MessageQ
    */
    dmq_status = pams_exit();
    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\n\tDetached successfully from MessageQ.\n\n" );
    else
        (void)printf( "\n\tError detaching from MessageQ; status returned is: %d\n\n", 
                 dmq_status );

    return( PAMS__SUCCESS );

} /*  end of main  */



int32 ExampleEncode(pams_handle msg_handle)
{
    int32           dmq_status;
    int32           tag;
    int32           length;
    uint32          elt1 = 0xFFFFFFFF;
    char            *elt2 = "This is a string";
    int16           elt3 = -12345;

    /*
    **  Create the message handle
    */
    dmq_status = pams_create_handle ( msg_handle, NULL );
    if ( dmq_status != PAMS__SUCCESS ) {
        (void)printf( "\n\tError creating message handle; status returned is: %d\n", 
                 dmq_status );
        return dmq_status;
    }

    /*
    **  Encode a 3-bits unsigned integer
    */
    tag = PSDM_UINT32 | 1;
    dmq_status = pams_encode_uint32 ( msg_handle, &tag, &elt1 );
    if ( dmq_status != PAMS__SUCCESS)
        (void)printf( "\n\tError encoding element of type uint32; status returned is: %d\n", 
                dmq_status );
    else
        (void)printf( "\n\tSuccesfully encoded element of type uint32 : <%X>\n", 
                elt1);


    /*
    **  Encode a null-terminated string
    */
    tag = PSDM_STRING | 2;
    length = -1;
    dmq_status = pams_encode_string ( msg_handle, &tag, elt2, &length );
    if ( dmq_status != PAMS__SUCCESS)
        (void)printf( "\n\tError encoding element of type string; status returned is: %d\n", 
                 dmq_status );
    else
        (void)printf( "\n\tSuccesfully encoded element of type string : <%s>\n", 
                elt2);

    /*
    **  Encode a 16-bits signed integer
    */
    tag = PSDM_INT16 | 3;
    dmq_status = pams_encode_int16 ( msg_handle, &tag, &elt3 );
    if ( dmq_status != PAMS__SUCCESS)
        (void)printf( "\n\tError encoding element of type int16; status returned is: %d\n", 
                 dmq_status );
    else
        (void)printf( "\n\tSuccesfully encoded element of type int16 : <%d>\n", 
                elt3);

    return dmq_status;
}



int32 ExampleDecode(pams_handle msg_handle)
{
    int32           dmq_status;
    int32           tag;
    int32           length;
    uint32          elt1;
    char            *elt2;
    int16           elt3;

    /*
    **  Loop through all encoded elements in message
    */
    for ( ; ; )
    {
        /* get next element */
        dmq_status = pams_next_msg_field ( msg_handle, &tag, &length );

        /* if status is PAMS__MOMORETAG all tags have been decoded */
        if ( dmq_status != PAMS__SUCCESS)
        {
            if ( dmq_status != PAMS__NOMORETAG )
                (void)printf( "\n\tError decoding next element; status returned is: %d\n", 
                        dmq_status );
            break;  /* stop the decoding loop */
        }

        /* process decoded tag */
        switch (tag) 
        {
        case PSDM_UINT32 | 1 :
            /* decode integer */
            dmq_status = pams_decode_uint32 ( msg_handle, &tag, &elt1 );
            if ( dmq_status == PAMS__SUCCESS )
                (void)printf( "\n\tDecoded uint32 element with value : <%X>\n", 
                        elt1 );
            else
                (void)printf( "\n\tError decoding element of type uint32; status returned is: %d\n", 
                        dmq_status );
            break;

        case PSDM_STRING | 2 :
            /* allocate enough memory for string, decode string and free it */
            elt2 = (char *) malloc((size_t)(length + 1));
            dmq_status = pams_decode_string ( msg_handle, &tag, elt2, &length, NULL );
            if ( dmq_status == PAMS__SUCCESS )
                (void)printf( "\n\tDecoded string element with value : <%s>\n", 
                        elt2 );
            else
                (void)printf( "\n\tError decoding element of type string; status returned is: %d\n", 
                        dmq_status );
            (void)free((void*)elt2);
            break;

        case PSDM_INT16 | 3 :
            /* decode integer */
            dmq_status = pams_decode_int16 ( msg_handle, &tag, &elt3 );
            if ( dmq_status == PAMS__SUCCESS )
                (void)printf( "\n\tDecoded int16 element with value : <%d>\n", 
                        elt3 );
            else
                (void)printf( "\n\tError decoding element of type int16; status returned is: %d\n", 
                        dmq_status );
            break;

        default :
            /* decoded unknown tag */
            (void)printf( "\n\tDecoded unexpected element with tag : %d\n", 
                     tag );
        }
    }

    /*
    **  Delete the message handle
    */
    dmq_status = pams_delete_handle ( msg_handle );
    if ( dmq_status != PAMS__SUCCESS )
        (void)printf( "\n\tError deleting message handle; status returned is: %d\n", 
                 dmq_status );

    return dmq_status;
}
