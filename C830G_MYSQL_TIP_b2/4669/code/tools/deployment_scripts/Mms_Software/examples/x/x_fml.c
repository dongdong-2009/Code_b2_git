/*
** Copyright (c) 1998 BEA Systems, Inc.
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
**    FILE:           x_fml.c
**
**    DESCRIPTION:    This simple example will illustrate how to build
**                    and interpret a fml message.
**                    The example simply attaches to "EXAMPLE_Q_1"
**                    puts a message to itself, decodes it and exits.
**
**    CREATION DATE:  29 Jul 1998
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

/**  ......................  **/
/**    FML include files     **/
/**  ......................  **/

#include "fml32.h"

FBFR32 *fbput;	/* pointer to the fielded buffer for put */
FBFR32 *fbget;  /* pointer to the fielded buffer for get */


/**  ...........................................................  **/
/**  user generated include - note the following definitions were **/
/**  created using the FML header scripting tool mkfldhdr which   **/
/**  takes a text file containing the field definitions and makes **/
/**  an include file for #defines used in the application below,  **/
/**  for example:                                                 **/
/**  $ mkfldhdr x_fmltbl           <- creates x_fmltbl.h          **/
/**  ...........................................................  **/

#include "x_fmltbl.h"


/**  .............................. **/
/**   Function Forward Declarations **/
/**  .............................. **/

int32 ExampleEncodeFML();
int32 ExampleDecodeFML();


/** ................................  **/
/** fields used to print message text **/
/** ................................  **/

int32  severity;               /* severity code           */
char   msg_text[512];          /* pams_status_text output */
int32  msg_text_len = 512;     /* length of buffer        */
int32  ret_len;                /* returned string length  */


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
    struct show_buffer  my_show_buffer;
    int32               my_show_buffer_len = sizeof( my_show_buffer );
    int32		large_size,large_area_len;


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
        (void)printf( "\tAttached successfully to queue \"%s\".\n", q_name );
    else {
        pams_status_text(&dmq_status,&severity,msg_text,&msg_text_len,&ret_len);
        (void)printf( 
            "Error attaching queue \"%s\" \nStatus returned is: %s (%ld)\n\n",
            q_name,msg_text,dmq_status );
        exit ( 0 );
    }
    /*
    **  Encode few elements of various datatypes
    */
    dmq_status = ExampleEncodeFML ( );
    if ( dmq_status != PAMS__SUCCESS ) {
	pams_status_text(&dmq_status,&severity,msg_text,&msg_text_len,&ret_len);
	(void)printf("Error in the EncodeFML!\n");
	(void)printf("Status returned is: %s (%ld)\n\n", msg_text,dmq_status );
        exit ( 0 );
    }

    /*
    **  Put message into my own queue  
    */
    priority   = 0;
    mclass     = 0;
    type       = 0;
    delivery   = PDEL_MODE_WF_MEM;
    msg_size   = PSYM_MSG_FML; 
    timeout    = 50;  /* 5 seconds  */
    uma        = PDEL_UMA_DISCL;

    dmq_status = pams_put_msg(
                      (char*) fbput,
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
	(void)printf( "\tPut successfully to queue: \"%s\".\n", q_name );
    else {
      	pams_status_text(&dmq_status,&severity,msg_text,&msg_text_len,&ret_len);
	(void)printf( 
        "Error sending to queue \"%s\" \nStatus returned: %s (%ld)\n\n",
               q_name,msg_text,dmq_status );
	exit ( 0 );
    }


    /*
    **  Get message from my own queue  
    */
    buffer_size = PSYM_MSG_BUFFER_PTR;
    fbget = NULL;
    dmq_status = pams_get_msg(
                      (char*) &fbget,
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
                      &large_area_len,
                      &large_size,
                      (char *) 0 );

    if ( dmq_status == PAMS__SUCCESS ) {
        (void)printf( "\tGet successfully from queue: \"%s\".\n", q_name );
        if ( my_show_buffer.endian == PSYM_FML ) {
            /*
            **  Decode message
            */
            dmq_status = ExampleDecodeFML( );
        }
        else
            (void)printf( "\n\tMessage is not an FML message\n" );
    }
    else {
       pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,&ret_len);
       (void)printf(
         "Error getting from queue \"%s\" \nStatus returned is: %s (%ld)\n\n",
          q_name,msg_text,dmq_status );
       exit ( 0 );
    }

    /*
    **  Detach from MessageQ
    */
    dmq_status = pams_exit();
    if ( dmq_status == PAMS__SUCCESS )
        (void)printf( "\tDetached successfully from MessageQ.\n\n" );
    else {
        pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,&ret_len);
        (void)printf( 
	    "Error dettaching from MessageQ.\nStatus returned is: %s (%ld)\n\n",
            msg_text, dmq_status );
    }
    return( PAMS__SUCCESS );

} /*  end of main  */



int32 ExampleEncodeFML( )
{
    int32           fml_status = PAMS__SUCCESS;
    int32           tag;
    int32           length;
    int32           elt1 = 123456789;
    char            *elt2 = "This is a string";
    int16           elt3 = -12345;
    int16           *short_ptr;
    int32           *long_ptr;

    /*
    **  Create the FML message handle
    */

    fbput = Falloc32( (FLDOCC32) 10 , (FLDLEN32) 1000 );
    if(fbput == NULL) {
	F_error32("Falloc32");
	return PAMS__FAILED;
    }

    /*
    **  Encode a 16 bit integer
    */
    (void) printf("\tFML Encoding a 16 bit integer = %d\n",elt3);
    short_ptr = &elt3; /* we must point the short */
    fml_status = Fadd32( fbput, FSHORT, (char*) short_ptr, NULL); 
    if(fml_status == -1) {
	F_error32("Fadd32 - FSHORT");
	return PAMS__FAILED;
    }

    /*
    **  Encode a null-terminated string
    */
    (void) printf("\tFML Encoding a null-terminated string = \"%s\"\n",elt2);
    fml_status = Fadd32( fbput, FSTRING, (char*) elt2, NULL);
    if(fml_status == -1) {
        F_error32("Fadd32 - FSTRING");
        return PAMS__FAILED;
    }

    /*
    **  Encode a 32-bit integer
    */
    (void) printf("\tFML Encoding a 32-bit signed integer = %ld\n",elt1);
    long_ptr = &elt1;
    fml_status = Fadd32( fbput, FLONG, (char*) long_ptr, NULL);
    if(fml_status == -1) {
	F_error32("Fadd32 - FLONG");
	return PAMS__FAILED;
    }

    return fml_status;
}


int32 ExampleDecodeFML( ) 
{
    int32           fml_status = PAMS__SUCCESS;
    int32           tag;
    int32           length;
    int32           elt1;
    int32          *long_ptr;
    char            *elt2;
    char            a_string[30];
    int16           elt3;
    int16	    *short_ptr;
    FLDOCC32	    occ = 0;

    /*
    **  Extract the elements in message
    */

    long_ptr = &elt1;
    fml_status = Fget32(fbget, FLONG, (FLDOCC32) occ, (char*) long_ptr,
                       (FLDLEN32 *)0);
    if(fml_status == -1) {
	   F_error32("Fadd32 - FLONG");
	   return PAMS__FAILED;
    }
    (void) printf("\tFML Decoded 32 bit integer = %d\n",elt1);


    short_ptr = &elt3;
    fml_status = Fget32(fbget, FSHORT, (FLDOCC32) occ, (char*) short_ptr,
                       (FLDLEN32 *)0);
    if(fml_status == -1) {
           F_error32("Fadd32 - FSHORT");
           return PAMS__FAILED;
    }
    (void) printf("\tFML Decoded 16 bit integer = %d\n",elt3);

    elt2 = a_string;
    fml_status = Fget32(fbget, FSTRING, (FLDOCC32) occ, (char*) elt2, 
                       (FLDLEN32 *) 0);
    if(fml_status == -1) {
           F_error32("Fadd32 - FSTRING");
           return PAMS__FAILED;
    }
    (void) printf("\tFML Decoded a null terminated string = \"%s\"\n",a_string);

    return fml_status;
}
