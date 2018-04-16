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
**    FILE:          x_avail.c
**
**    DESCRIPTION:   Illustrates the avail services functionality, avail
**                   register, and avail deregister and getting the avail
**                   messages. This program attaches to a temporary queue,
**	             registers for availability of IVP_unowned_sq and prints
**	             the results, attaches to IVP_unowned_sq and gets the
**                   avail message. Deregisters IVP_unowned_sq, detaches from
**                   IVP_unowned_sq and exits.
**		  
**
**    CREATION DATE: 29 March 1996
**
**    REQUIREMENTS:  The queue named "IVP_unowned_sq" must be defined in your
**                   init file as a secondary queue. 
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
#include "p_typecl.h"
#include "p_msg.h"

/**  .......................  **/
/**         Constants         **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  512

/**  .......................  **/
/**   Function Forwards       **/
/**  .......................  **/
int32 ExampleAttach(q_address *q_addr);
int32 ExampleGet(void);
void  ExampleExit(void);
int32 ExampleLocate(char *queue_name, q_address *q_addr);
int32 ExampleAvailRegister(q_address my_primary_queue, q_address  target_q);
int32 ExampleAvailDeregister(q_address my_primary_queue, q_address  target_q);

/** ..........................**/
/**     global data           **/
/** ..........................**/
static q_address   Avail_server;

int32
main( )
{
   q_address   my_primary_queue;
   q_address   target_q;
   int32       attach_mode = PSYM_ATTACH_TEMPORARY;
   int32       attach_type = PSYM_ATTACH_PQ;
   int32       nStatus;
   int32       severity;                /*  severity code           */
   char        status_text[512];        /*  pams_status_text output */
   int32       status_text_len = 512;   /*  length of buffer        */
   int32       ret_len;                 /* returned string length   */
   int32       detach_opt = PSYM_NOFLUSH_Q;
   int32       detach_op_len = 1;
   int32       msgs_flushed = 0;

   (void)printf( "\nExample of DMQ Avail services started...\n\n" );
   
   /*****************************/
   /* attach to temporary queue */
   /*****************************/
   nStatus = pams_attach_q(&attach_mode, &my_primary_queue,
      &attach_type, 0, 0, 0,0,0,0,0);
   
   if (nStatus == PAMS__SUCCESS) 
      (void)printf( "Attached to temporary queue : %d.%d\n", 
               my_primary_queue.au.group,
               my_primary_queue.au.queue );
   else {
      pams_status_text(&nStatus,&severity, status_text,&status_text_len,
                                &ret_len);
      (void)printf( "\tError attaching to temporary queue.status returned is: %s (%ld)\n\n",
               status_text,
               nStatus );
      exit (0);
      }                                                              

      
   /***************************************/
   /* locate the queue were interested in */
   /***************************************/
   nStatus = ExampleLocate ("IVP_unowned_sq", &target_q);

   if (nStatus != PAMS__SUCCESS) {
      exit (0);
   }

   /*********************************/
   /* locate the avail_server queue */
   /* The availability messages are */
   /* to this queue.                */
   /*********************************/

   nStatus = ExampleLocate ("AVAIL_SERVER", &Avail_server);

   if (nStatus != PAMS__SUCCESS) {
      exit (0);
   }

   /******************************/
   /* create and send an avail   */
   /* register request to the    */
   /* avail server for the       */
   /* target queue with response */
   /* to my primary queue        */
   /******************************/
   nStatus = ExampleAvailRegister(my_primary_queue, target_q);

   if (nStatus != PAMS__SUCCESS) {
      pams_status_text(&nStatus,&severity,status_text,&status_text_len,
                                &ret_len);

      (void)printf("error registering for availability-status  %s\n", status_text);
      exit (0);
      } else {
     (void)printf("\nAvail registration succeeded\n");
   }

   /**********************************/
   /* get the response message for   */
   /* the avail registration request */
   /**********************************/
   nStatus = ExampleGet();

   if (nStatus != PAMS__SUCCESS) {
      pams_status_text(&nStatus,&severity,status_text,&status_text_len,
                                &ret_len);

      (void)printf("error getting avail_response -status  %s\n", status_text);
      exit (0);
   }

   /***********************************/
   /* Since no one is attached to the */
   /* target queue IVP_unowned_sq, we */
   /* should get an UNAVAIL message.  */
   /***********************************/
   (void)printf ("\nget unavail message:");

   nStatus = ExampleGet();

   if (nStatus != PAMS__SUCCESS) {
      pams_status_text(&nStatus,&severity,status_text,&status_text_len,
                                &ret_len);

      (void)printf("error getting  availability  -status  %s\n", status_text);
      exit (0);
   }

   /*****************************/
   /* Attach to IVP_unowned_sq  */
   /*****************************/
   (void)printf ("attach to IVP_unowned_sq\n\n");

   if ( ExampleAttach( &target_q ) != PAMS__SUCCESS )
         exit( 0 );

   /***********************************/
   /* Now that we have attached to    */
   /* IVP_unowned_sq we should get an */
   /* avail message returned          */
   /***********************************/
   (void)printf ("\ngot avail message:");
   nStatus = ExampleGet();

   if (nStatus != PAMS__SUCCESS) {
      pams_status_text(&nStatus,&severity,status_text,&status_text_len,
                                &ret_len);

      (void)printf("error getting  availability-status  %s\n", status_text);
      exit (0);
   }

   /******************************/
   /* detach from IVP_unowned_sq */
   /******************************/

   (void)printf ("detach from IVP_unowned_sq\n\n");

   nStatus = pams_detach_q (&target_q,
                            &detach_opt,
                            &detach_op_len,
                            &msgs_flushed);

   if (nStatus != PAMS__SUCCESS) {
      pams_status_text(&nStatus,&severity,status_text,&status_text_len,
                                &ret_len);

      (void)printf("error detaching queue -status  %s\n", status_text);
      exit (0);
   }

   /*********************************/
   /* since we have detached from   */
   /* IVP_unowned_sq, we should get */
   /* an unavail message            */
   /*********************************/
   (void)printf ("got unavail message:");

   nStatus = ExampleGet();

   if (nStatus != PAMS__SUCCESS) {
      pams_status_text(&nStatus,&severity,status_text,&status_text_len,
                                &ret_len);

      (void)printf("error registering for availability-status  %s\n", status_text);
      exit (0);
   }

   /******************************/
   /* create and send an avail   */
   /* register request to the    */
   /* avail server for the       */
   /* target queue with response */
   /* to my primary queue        */
   /******************************/
   nStatus = ExampleAvailDeregister(my_primary_queue, target_q);

   if (nStatus != PAMS__SUCCESS) {
      pams_status_text(&nStatus,&severity,status_text,&status_text_len,
                                &ret_len);

      (void)printf("error deregistering -status  %s\n", status_text);
      exit (0);
      } else {
     (void)printf("\nAvail deregistration succeeded\n");
   }


   /*  Exit from MessageQ  */
   ExampleExit();

   (void)printf( "\nAvail services example complete.\n" );

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
   (void)strcpy( q_name, "IVP_unowned_sq" );
   attach_mode = PSYM_ATTACH_BY_NAME;
   q_type      = PSYM_ATTACH_SQ;
   q_name_len  = (int32) strlen( q_name );
  
   dmq_status  = pams_attach_q(&attach_mode, q_addr, &q_type, q_name, 
                 &q_name_len, (int32 *) 0,    /*  Use default name space  */
                     (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "Attached to queue 'IVP_unowned_sq': \"%s\".\n", q_name );
   else
      (void)printf( "Error attaching to queue 'IVP_unowned_sq': \"%s\"; status returned is: %ld.\n", 
               q_name, dmq_status );

   return ( dmq_status );

} /*  end of ExampleAttach  */


int32
ExampleGet( )
{
   char        priority;
   short       msg_class;
   short       msg_type;
   short       msg_area_len;
   short       msg_data_len;
   int32       dmq_status;
   int32       sel_filter;
   char        msg_area[MAX_MESSAGE_SIZE];
   q_address   msg_source;
   struct PSB  get_psb;
   AVAIL_REG_REPLY *avail_reg_reply;
   int32       timeout = 30;

  msg_area[0] = '\0';
  /*
  **  Get a messages currently in the queue and print print out
  */
  (void)printf( "\n" );   

  priority     = 0;   /*  get messages of both high and normal priorities */
  sel_filter   = 0;   /*  select all messages, i.e. don't filter any out  */
  msg_area_len = MAX_MESSAGE_SIZE;
  dmq_status   = PAMS__SUCCESS;

      /*  Initialize the message area to hold the message  */
      (void)memset((void*)msg_area, '\0', MAX_MESSAGE_SIZE );

      /*  Get a message  */
      dmq_status = pams_get_msgw( msg_area, &priority, &msg_source,
                 &msg_class, &msg_type, &msg_area_len, &msg_data_len, &timeout,
                 &sel_filter, &get_psb, (struct show_buffer *) 0, (int32 *) 0,
                 (int32 *) 0, (int32 *) 0, (char *) 0 );

      switch ( dmq_status ) {
	  case PAMS__SUCCESS :
	    switch (msg_type)
		 {
		 case MSG_TYPE_AVAIL_REG_REPLY:
		     avail_reg_reply = (AVAIL_REG_REPLY *)msg_area;
		     (void)printf ( "got avail reg reply:\n");
		     (void)printf ("\tregistrant source = %d.%d\n",
			    msg_source.au.group,
			    msg_source.au.queue);
		     (void)printf ("\tclass = %d\n\ttype = %d\n",
			     msg_class,
			     msg_type);
		     (void)printf ( "\tstatus = %d\n\treg_id = %d\n\treg number = %d\n",
			     avail_reg_reply->status,
			     avail_reg_reply->reg_id,
			     avail_reg_reply->number_reg); 
		     break;

		case MSG_TYPE_AVAIL : {
		     AVAIL * pAvail = (AVAIL *) msg_area;
		     (void)printf("\tclass  : MSG_CLAS_PAMS\n");
		     (void)printf("\ttype  : MSG_TYPE_AVAIL\n");
		     (void)printf("\tsource : %d.%d\n", msg_source.au.group, msg_source.au.queue);
		     (void)printf("\ttarget_q   %d.%d\n\n", 
		     pAvail->target_q.au.group, pAvail->target_q.au.queue);

		break;
	     }

	     case MSG_TYPE_UNAVAIL : {
		UNAVAIL * pUnavail = (UNAVAIL *) msg_area;

		(void)printf("\tclass  : MSG_CLAS_PAMS\n");
		(void)printf("\ttype   : MSG_TYPE_UNAVAIL\n");
		(void)printf("\tsource : %d.%d\n", msg_source.au.group, msg_source.au.queue);

		(void)printf("\ttarget_q   %d.%d\n\n",
		  pUnavail->target_q.au.group, pUnavail->target_q.au.queue);

		break;
	     }

             default:
                 (void)printf ("msg type unknown\n");
                 break;
             }
        break;

      case PAMS__NOMOREMSG :
        /*  Status returned when queue is empty  */
        (void)printf("primary queue is empty\n");
        break;

      default :
        (void)printf( "Error getting message; status returned is %ld.\n",
            dmq_status );
        break;
    
    }

  if ( dmq_status == PAMS__NOMOREMSG )
    dmq_status = PAMS__SUCCESS;

  return ( dmq_status );

} /* end of ExampleGet  */



int32 ExampleAvailRegister(q_address my_primary_queue,
   q_address  target_q)
{
   int32       nStatus;
   AVAIL_REG   availReg;
   char        uma;
   int32       timeout;
   struct PSB  put_psb;
   char        priority = 0;
   short       msg_class = MSG_CLAS_PAMS;
   short       msg_type = MSG_TYPE_AVAIL_REG;
   char        delivery = PDEL_MODE_NN_MEM;
   short       msg_size = sizeof(struct _AVAIL_REG);
	     
   timeout     = 100;               /* Wait 10 seconds before giving up     */
   uma         = PDEL_UMA_DISCL;    /* If can't deliver it, DISCard and Log */

/* fill in the message, make the distribution queue ourself */

   availReg.version           = 31;
   availReg.timeout = 0;      /* set to default of 30 sec*/


   
   /****************************************************************/
   /* set up the queue for which you want to know the availability */
   /****************************************************************/
   

   availReg.target_q.au.group = target_q.au.group;
   availReg.target_q.au.queue = target_q.au.queue;

   /****************************************************************/
   /* set up the queue where the avail server will return messages */
   /* usually your primary queue- but any queue can be used        */
   /****************************************************************/

   availReg.distribution_q.au.group = my_primary_queue.au.group;
   availReg.distribution_q.au.queue = my_primary_queue.au.queue;

   /************************************************************/
   /* put the message to the server and specify response queue.*/
   /* The response queue receives the ack of this message      */
   /************************************************************/

   nStatus = pams_put_msg((char *) &availReg,
                          &priority,
                          &Avail_server,
	                       &msg_class,
                          &msg_type,
                          &delivery,
                          &msg_size,
                          &timeout,
                          &put_psb,
                          &uma,
                          &my_primary_queue,
                     	  (int32 *) 0,
                     	  (char *) 0,
                     	  (char *) 0 );
   
   return nStatus;
}

int32 ExampleAvailDeregister(q_address my_primary_queue,
   q_address  target_q)
{
   int32       nStatus;
   AVAIL_DEREG availdereg;
   char        uma;
   int32       timeout;
   struct PSB  put_psb;
   char        priority = 0;
   short       msg_class = MSG_CLAS_PAMS;
   short       msg_type = MSG_TYPE_AVAIL_DEREG;
   char        delivery = PDEL_MODE_NN_MEM;
   short       msg_size = sizeof(struct _AVAIL_DEREG);
	     
   timeout     = 100;               /* Wait 10 seconds before giving up     */
   uma         = PDEL_UMA_DISCL;    /* If can't deliver it, DISCard and Log */



   availdereg.version           = 31;   /* latest format version */

   
   /*****************************************************/
   /* set up the queue for which you want to deregister */
   /*****************************************************/
   

   availdereg.target_q.au.group = target_q.au.group;
   availdereg.target_q.au.queue = target_q.au.queue;

   /****************************************************************/
   /* set up the queue where the avail server will return messages */
   /* usually your primary queue- but any queue can be used        */
   /****************************************************************/

   availdereg.distribution_q.au.group = my_primary_queue.au.group;
   availdereg.distribution_q.au.queue = my_primary_queue.au.queue;

   availdereg.req_ack = '\0';

   /************************************************************/
   /* put the message to the server and specify response queue.*/
   /* The response queue receives the ack of this message      */
   /************************************************************/

   nStatus = pams_put_msg((char *) &availdereg,
                          &priority,
                          &Avail_server,
                          &msg_class,
                          &msg_type,     
                          &delivery,
                          &msg_size,
                          &timeout,
                          &put_psb,
                          &uma,
                          &my_primary_queue,
                     	  (int32 *) 0,
                     	  (char *) 0,
                     	  (char *) 0 );

   return nStatus;
}


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


int32
ExampleLocate(char *queue_name, q_address *q_addr)
{
    int32     dmq_status;
    int32     queue_name_len;
    int32     wait_mode;
    int32     req_id;

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
                           (int32 *) 0,  /* Use default name space list of
                           process and group  */
                           (int32 *) 0,  /* name space list len not needed */
                           (int32 *) 0 );

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
