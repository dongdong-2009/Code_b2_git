/*
** Copyright (c) 1996,1998 BEA Systems, Inc.
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
**    FILE:          x_sbstat.c
**
**    DESCRIPTION:   Illustrates how to make use of MessageQ's SBS
**                   MOT status  functionality. This will send a
**		     PAMS_SBS_REG message to the sbs server using 
**                   PAMS_SEND_MSG. If the send was successful, the program
**                   will get the response from the sbs server and print 
**                   its contents. The program will get the MOT status and
**                   finally deregister.
**
**
**    CREATION DATE: 20 March 1996
**
**    REQUIREMENTS:  This program will work on any platform that supports
**                   Selective Broadcast Services.
**                   The queue named "EXAMPLE_Q_1" must be defined in your
**                   init file as a primary queue. 
**
*/

/**  .......................  **/
/**  C library include files  **/
/**  .......................  **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**  ......................  **/
/**  MessageQ include files  **/
/**  ......................  **/
#include "p_entry.h"
#include "p_return.h"
#include "p_symbol.h"
#include "p_typecl.h"
#include "p_proces.h"
#include "p_msg.h"


/**  .......................  **/
/**         Constants         **/
/**  .......................  **/
#define MAX_MESSAGE_SIZE  50

/**.............................. **/
/** replace these when in psymbol **/
/** ............................. **/

#define PSEL_ALL_RULES 1
#define PSEL_ANY_RULE  2
#define PSEL_OPER_AND  7 

#define MSG_TYPE_SBS_REGISTER_REQ -1196 /* Long form registration           */
#define MSG_TYPE_SBS_REGISTER_RESP -1197 /* Response to REGISTER_REQ        */
#define MSG_TYPE_SBS_DEREGISTER_REQ -1170 /* Deregister from broadcast      */
#define MSG_TYPE_SBS_DEREGISTER_RESP -1172 /* Response to DEREGISTER_REQ    */
#define MSG_TYPE_SBS_STATUS_REQ -1177   /* Request status transmission      */
#define MSG_TYPE_SBS_STATUS_RESP -1178  /* Response to STATUS_REQ           */
#define MSG_TYPE_SBS_SEQUENCE_GAP -1199 /* Notification of seqgap           */

/**  .............................. **/
/**   Function Forward Declarations **/
/**  .............................. **/
int32  ExampleAttach(q_address *);
int32  ExamplePutReg(q_address *);
int32  ExamplePutDereg(q_address *);
int32  ExampleGetRegReply( );
int32  ExampleGetMOTStatus(q_address *);
int32  ExampleGetDeregReply( );
void   ExampleExit( );

/** ..........................................**/
/**  typedef                                  **/
/** ..........................................**/

/*************************************/
/* fields used to print message text */
/*************************************/


    int32  severity;               /*  severity code           */
    char   msg_text[512];          /*  pams_status_text output */
    int32  msg_text_len = 512;     /*  length of buffer        */
    int32  ret_len;                /* returned string length   */



int32
main( )
{
   q_address   my_primary_q_addr;


   (void)printf( "\nSBS status example started...\n\n" );

   /* ....................... */
   /*  Attach to MessageQ  */
   /* ....................... */
   if ( ExampleAttach( &my_primary_q_addr ) != PAMS__SUCCESS )
      exit( 0 );
   
   /*.............................*/
   /* create and send reg message */
   /*.............................*/

   if ( ExamplePutReg( &my_primary_q_addr ) != PAMS__SUCCESS )
      exit( 0 );
   
   /*..........................*/
   /*  get registration reply  */
   /*..........................*/

   if ( ExampleGetRegReply() != PAMS__SUCCESS )
      exit( 0 );


   /*..........................*/
   /*  get registration reply  */
   /*..........................*/

   if ( ExampleGetMOTStatus( &my_primary_q_addr ) != PAMS__SUCCESS )
      exit( 0 );

   /*.............................*/
   /* create and send reg message */
   /*.............................*/


   if ( ExamplePutDereg( &my_primary_q_addr ) != PAMS__SUCCESS )
      exit( 0 );
   

   /*..........................*/
   /*  get registration reply  */
   /*..........................*/

   if ( ExampleGetDeregReply() != PAMS__SUCCESS )
      exit( 0 );

   /* ....................... */
   /*  Exit from MessageQ  */
   /* ....................... */
   ExampleExit();

   (void)printf( "\nSBS status example completed.\n" );
   return( PAMS__SUCCESS );

} /*  end of main  */


int32
ExampleAttach(q_address   *q_addr)
{
   int32        attach_mode;
   int32        dmq_status;
   int32        q_name_len;
   int32        q_type;
   static char  q_name[12];

   (void)printf( "\nAttempting to Attach by name to the primary queue 'EXAMPLE_Q_1'\n" );

   attach_mode = PSYM_ATTACH_BY_NAME;
   q_type      = PSYM_ATTACH_PQ;
   (void)strcpy( q_name, "EXAMPLE_Q_1" );
   q_name_len  = (int32) strlen( q_name );
  
   dmq_status = pams_attach_q(&attach_mode, q_addr, &q_type, q_name,
                     &q_name_len,
                     (int32 *) 0,    /*  Use default name space  */
                     (int32 *) 0, (int32 *) 0, (char *) 0, (char *) 0 );

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "\tAttached to queue 'EXAMPLE_Q_1': \"%s\".\n\n", q_name );
   else
      {
      pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);

      (void)printf( "\tError attaching to queue 'EXAMPLE_Q_1': \"%s\"...status returned is: %s (%ld)\n\n",
               q_name,
               msg_text,
               dmq_status );
      }
                                                 
   return ( dmq_status );

} /*  end of ExampleAttach  */


  
int32
ExamplePutReg(q_address *q_addr)
{
   char        priority;
   char        delivery = PDEL_MODE_NN_MEM; 
   char        uma  = PDEL_UMA_DISC;
   short       mclass;
   short       type;
   short       size;
   int32       dmq_status;
   int32       put_timeout;
   struct PSB  put_psb;
   SBS_REGISTER_REQ  SBS_reg;
   q_address   target_q;
   int32       resp_q = 0;

   /* ....................................................... */
   /*  Put the sbs reg message to the sbs server queue        */
   /* ....................................................... */
   (void)printf( "\n" );

   priority    = '\0';           /* Regular priority; use 0, NOT '0'     */
   put_timeout     = 100;            /* Wait 10 seconds before giving up     */

   type = MSG_TYPE_SBS_REGISTER_REQ;
   priority = 0;
   target_q.au.group = q_addr->au.group;
   target_q.au.queue = PAMS_SBS_SERVER;
   mclass = MSG_CLAS_PAMS;                
   size = sizeof(SBS_REGISTER_REQ);
   
   SBS_reg.head.version = 40;
   SBS_reg.head.user_tag = 5155;
   SBS_reg.head.mot = 5100;
   SBS_reg.head.distribution_q.all = q_addr->all;
   SBS_reg.head.req_ack = 1;           /* Ack requested */
   SBS_reg.head.seq_gap_notify = 1;        /* SEQGAP requested */
   SBS_reg.head.auto_dereg = 0;     /* no Auto dereg*/
   SBS_reg.head.rule_count = 2;
   SBS_reg.head.rule_conjunct = PSEL_ALL_RULES;

   SBS_reg.rule[0].offset = PSEL_TYPE;
   SBS_reg.rule[0].data_operator = PSEL_OPER_EQ;
   SBS_reg.rule[0].length = 4;
   SBS_reg.rule[0].operand =  25;

   SBS_reg.rule[1].offset = PSEL_CLASS;
   SBS_reg.rule[1].data_operator = PSEL_OPER_GTR;
   SBS_reg.rule[1].length = 4;
   SBS_reg.rule[1].operand =  50;

   dmq_status = pams_put_msg(
                     (char *) &SBS_reg,
                     &priority,
                     &target_q,       
                     &mclass,
                     &type,
                     &delivery,
                     &size,
                     &put_timeout,
                     &put_psb,
                     &uma,
                     (q_address *) &resp_q,
                     (int32 *) 0,
                     (char *) 0,
                     (char *) 0 );
      
    if ( dmq_status == PAMS__SUCCESS )
         (void)printf( "\tPut message: succeeded \n" );
    else
        {
        pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);

        (void)printf( "Error putting message...status returned is: %s (%ld)\n\n",
                 msg_text,
                  dmq_status );
        }

   return ( dmq_status );

} /*  end of ExamplePutReg  */


int32
ExamplePutDereg(q_address   *q_addr)
{
   char        priority;
   char        delivery = PDEL_MODE_NN_MEM; 
   char        uma  = PDEL_UMA_DISC;
   short       mclass;
   short       type;
   short       size;
   int32       dmq_status;
   int32       put_timeout;
   struct PSB  put_psb;
   SBS_DEREGISTER_REQ  SBS_dereg;
   q_address   target_q;
   int32       resp_q = 0;

   /* ....................................................... */
   /*  Put the sbs reg message to the sbs server queue        */
   /* ....................................................... */
   (void)printf( "\n" );

   priority    = '\0';           /* Regular priority; use 0, NOT '0'     */

   put_timeout     = 100;            /* Wait 10 seconds before giving up     */

   type = MSG_TYPE_SBS_DEREGISTER_REQ;
   priority = 0;
   target_q.au.group = q_addr->au.group;
   target_q.au.queue = PAMS_SBS_SERVER;
   mclass = MSG_CLAS_PAMS;                
   size = sizeof (struct _SBS_DEREGISTER_REQ);
   
   SBS_dereg.version = 40;
   SBS_dereg.user_tag = 5155;
   SBS_dereg.mot = 5100;
   SBS_dereg.distribution_q.all = q_addr->all;
   SBS_dereg.req_ack = 1;           /* Ack requested */
   SBS_dereg.reg_id= 1;        /* SEQGAP requested */

   dmq_status = pams_put_msg(
                     (char *) &SBS_dereg,
                     &priority,
                     &target_q,       
                     &mclass,
                     &type,
                     &delivery,
                     &size,
                     &put_timeout,
                     &put_psb,
                     &uma,
                     (q_address *) &resp_q,
                     (int32 *) 0,
                     (char *) 0,
                     (char *) 0 );
      
    if ( dmq_status == PAMS__SUCCESS )
         (void)printf( "\tPut message: succeeded \n" );
    else
        {
        pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);

        (void)printf( "Error putting message...status returned is: %s (%ld)\n\n",
                  msg_text,
                  dmq_status );
        }

   return ( dmq_status );

} /*  end of ExamplePutDereg  */



int32
ExampleGetMOTStatus(q_address   *q_addr)
{
   SBS_STATUS_REQ status_req;

   char        priority;
   short       msg_class;
   short       msg_type;
   short       msg_data_len;
   short       msg_area_len;
   int32       dmq_status;
   int32       sel_filter;
   int32       timeout;
   q_address   msg_source;
   struct PSB  get_psb;

   time_t *timeval;
   char dummy[20];
   int i,j;
   char *ctime_string;
   char *z;
   
   struct _SBS_STATUS_RESP   sbs_status_resp;
   struct _SBS_STATUS_RESP_MOT *sbs_status_resp_mot;
   struct _SBS_STATUS_REP_REG_Q *sbs_status_rep_reg_q;
   struct _SBS_STATUS_REP_NUM_GROUPS *sbs_status_rep_num_groups;
   struct _SBS_STATUS_RESP_GROUP *sbs_status_resp_group;



   char        delivery = PDEL_MODE_NN_MEM; 
   char        uma  = PDEL_UMA_DISC;
   int32       put_timeout;
   struct PSB  put_psb;

   q_address   target_q;
   int32       resp_q = 0;

   put_timeout        = 100;

   msg_type = MSG_TYPE_SBS_STATUS_REQ;
   priority = 0;
   target_q.au.group = q_addr->au.group;
   target_q.au.queue = PAMS_SBS_SERVER;
   msg_class = MSG_CLAS_PAMS;                


   msg_area_len   = sizeof (struct _SBS_STATUS_REQ);
   dmq_status     = PAMS__SUCCESS;

   status_req.version = 40;
   status_req.user_tag = 5520;
   status_req.start_mot = 5000;
   status_req.end_mot = 5200;
   status_req.reset = 0;       /* don't reset counters */

  /* ................................................. */
  /*  Initialize the message area to hold the message  */
  /* ................................................. */


   dmq_status = pams_put_msg(
                     (char *) &status_req,
                     &priority,
                     &target_q,       
                     &msg_class,
                     &msg_type,
                     &delivery,
                     &msg_area_len,
                     &put_timeout,
                     &put_psb,
                     &uma,
                     (q_address *) &resp_q,
                     (int32 *) 0,
                     (char *) 0,
                     (char *) 0 );
      
    if ( dmq_status == PAMS__SUCCESS )
         (void)printf( "\tPut message: succeeded \n" );
    else
        {
        pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);

        (void)printf( "Error putting message...status returned is: %s (%ld)\n\n",
                  msg_text,
                  dmq_status );
        }
   msg_area_len = sizeof(   SBS_STATUS_RESP );

  /* ............... */
  /*  Get the reply  */
  /* ............... */
  dmq_status = pams_get_msgw(
		    (char *) &sbs_status_resp,
		    &priority,
		    &msg_source,
		    &msg_class,
		    &msg_type,
		    &msg_area_len,
		    &msg_data_len,
		    &timeout,
		    &sel_filter,
		    &get_psb,
		    (struct show_buffer *) 0,
		    (int32 *) 0,
		    (int32 *) 0,
		    (int32 *) 0,
		    (char *) 0 );

  switch ( dmq_status )
  {
     case PAMS__SUCCESS :
	(void)printf("Got Sbs Reply:");

(void)printf("\nSBS_STATUS_RESP - Message Header Block\n");
(void)printf("\tSBS_STATUS_RESP.version = %d\n",
	sbs_status_resp.version);
(void)printf("\tSBS_STATUS_RESP.user_tag = %d\n",
	sbs_status_resp.user_tag);
(void)printf("\tSBS_STATUS_RESP.status = %d\n",
	sbs_status_resp.status);
(void)printf("\tSBS_STATUS_RESP.num_rec = %d\n",
	sbs_status_resp.num_rec);
(void)printf("\tSBS_STATUS_RESP.last_block = %d\n",
	sbs_status_resp.last_block);

z = &sbs_status_resp.data[0];

if(sbs_status_resp.num_rec == 0)
	{
	(void)printf("There are no MOTs reported in this message.\n");
	return dmq_status;
	}

for(j=0;j<sbs_status_resp.num_rec;j++) {
    sbs_status_resp_mot = (struct  _SBS_STATUS_RESP_MOT*)z;

    (void)printf("\nSBS_STATUS_RESP - MOT\n");
    (void)printf("\tSBS_STATUS_RESP_MOT.mot = %d\n",
    	sbs_status_resp_mot -> mot);
    
    (void)memcpy((void*)&dummy[0], (void*)sbs_status_resp_mot ->transport.eth.mca_addr,12);
    dummy[12] = 0;
    (void)printf("\tSBS_STATUS_RESP_MOT.MCA = %s\n",
    	dummy);
    
    (void)memcpy((void*)dummy, (void*)sbs_status_resp_mot ->transport.eth. protocol,4);
    dummy[4] = 0;
    (void)printf("\tSBS_STATUS_RESP_MOT.protocol = %s\n",
    	dummy);
    (void)printf("\tSBS_STATUS_RESP_MOT.heartbeat = %d\n",
        sbs_status_resp_mot ->heartbeat_timer);

    (void)printf("\tSBS_STATUS_RESP_MOT.xmit_silo = %d\n",
    	sbs_status_resp_mot -> xmit_silo);

    (void)printf("\tSBS_STATUS_RESP_MOT.rcv_silo = %d\n",
    	sbs_status_resp_mot -> rcv_silo);

    (void)printf("\tSBS_STATUS_RESP_MOT.rcv_silo_max = %d\n",
    	sbs_status_resp_mot -> rcv_silo_max);

    (void)printf("\tSBS_STATUS_RESP_MOT.num_reg = %d\n",
    	sbs_status_resp_mot -> num_reg);

    (void)printf("\tSBS_STATUS_RESP_MOT.complete_rcvd = %d\n",
    	sbs_status_resp_mot -> complete_rcvd);

    (void)printf("\tSBS_STATUS_RESP_MOT.complete_bypes = %d\n",
    	sbs_status_resp_mot -> complete_bytes);

    (void)printf("\tSBS_STATUS_RESP_MOT.seq_gaps = %d\n",
    	sbs_status_resp_mot -> seq_gaps);

    (void)printf("\tSBS_STATUS_RESP_MOT.whole_msg_gaps = %d\n",
    	sbs_status_resp_mot -> whole_msg_gaps);

    (void)printf("\tSBS_STATUS_RESP_MOT.whole_silo_gap = %d\n",
    	sbs_status_resp_mot -> whole_silo_gap);


    (void)printf("\tSBS_STATUS_RESP_MOT.rail_0\n");

    (void)memcpy((void*)dummy, (void*)sbs_status_resp_mot -> rail[0].device_name ,16);
    dummy[16] = 0;

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_0.device_name = %s\n",
    	dummy );

    timeval = (time_t *)&sbs_status_resp_mot ->rail[0].fail_tod;
    if (sbs_status_resp_mot->rail[0].fail_tod.tv_sec == 0 &&
        sbs_status_resp_mot->rail[0].fail_tod.tv_usec == 0)
        {
        (void)printf("\t\tSBS_STATUS_RESP_MOT->rail[0].fail_tod = 0\n");
	}
    else
	{
        ctime_string = ctime(timeval);

        (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_0.fail_tod = %s\n",
    	       ctime_string);                               
	}


    
    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_0.msgs_sent = %d\n",
    	sbs_status_resp_mot ->rail[0].msgs_sent);

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_0.bytes_sent = %d\n",
    	sbs_status_resp_mot ->rail[0].bytes_sent);

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_0.pkts_sent = %d\n",
    	sbs_status_resp_mot ->rail[0].pkts_sent);

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_0.pkts_rcvd = %d\n",
    	sbs_status_resp_mot ->rail[0].pkts_rcvd);
    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_0.dupl_pkts_disc = %d\n",
    	sbs_status_resp_mot ->rail[0].dupl_pkts_disc);


    (void)printf("\tSBS_STATUS_RESP_MOT.rail_1\n");

    (void)memcpy((void*)dummy, (void*)sbs_status_resp_mot -> rail[1].device_name ,16);
    dummy[16] = 0;

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_1.device_name = %s\n",
    	dummy );

    timeval = (time_t *)&sbs_status_resp_mot ->rail[1].fail_tod;
    if (sbs_status_resp_mot->rail[1].fail_tod.tv_sec == 0 &&
        sbs_status_resp_mot->rail[1].fail_tod.tv_usec == 0) {
        (void)printf("\t\tSBS_STATUS_RESP_MOT->rail_1.fail_tod = 0\n");
	} else {
        ctime_string = ctime(timeval);

        (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_1.fail_tod = %s\n",
    	       ctime_string);                               
	}

    
    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_1.msgs_sent = %d\n",
    	sbs_status_resp_mot ->rail[1].msgs_sent);

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_1.bytes_sent = %d\n",
    	sbs_status_resp_mot ->rail[1].bytes_sent);

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_1.pkts_sent = %d\n",
    	sbs_status_resp_mot ->rail[1].pkts_sent);

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_1.pkts_rcvd = %d\n",
    	sbs_status_resp_mot ->rail[1].pkts_rcvd);

    (void)printf("\t\tSBS_STATUS_RESP_MOT.rail_1.dupl_pkts_disc = %d\n",
    	sbs_status_resp_mot ->rail[1].dupl_pkts_disc);

  
    z = z + sizeof( struct _SBS_STATUS_RESP_MOT);
    
    if (sbs_status_resp_mot->num_reg != 0)
       {
       sbs_status_rep_reg_q = (struct _SBS_STATUS_REP_REG_Q *) z;
    
       (void)printf("\nThe following queue(s) are registered for this MOT\n");

       (void)printf("SBS_STATUS_REP_REG_Q.reg_q = %d.%d\n",
    	      sbs_status_rep_reg_q -> reg_q.au.group,
	      sbs_status_rep_reg_q -> reg_q.au.queue);

       z = z + sizeof( struct _SBS_STATUS_REP_REG_Q);
       }
    else
       {
       (void)printf("\nThere are no queues registered for this MOT\n");
       }
    		
    sbs_status_rep_num_groups = (struct _SBS_STATUS_REP_NUM_GROUPS *) z;

    z= z+ sizeof(struct _SBS_STATUS_REP_NUM_GROUPS);

    if(sbs_status_rep_num_groups -> num_groups == 0)
    	(void)printf("\nThere are no remote groups communicating with this SBS Server.\n");
    else
    	{
    	(void)printf("\nThere are %d remote groups communicating with this SBS Server:\n\n",
    		sbs_status_rep_num_groups -> num_groups);
    	z = z + sizeof(struct _SBS_STATUS_RESP_GROUP);
    	for(i=0;i<sbs_status_rep_num_groups -> num_groups;i++)
    	    {
    	    sbs_status_resp_group = (struct _SBS_STATUS_RESP_GROUP *) z;
    	    (void)printf("\nSBS_STATUS_RESP_GROUP.group = %d\n",
    			sbs_status_resp_group -> group);
    	    (void)printf("SBS_STATUS_RESP_GROUP.rexmit_reqs_to_remote = %d\n",
    			sbs_status_resp_group -> rexmit_reqs_to_remote);
    	    (void)printf("SBS_STATUS_RESP_GROUP.rexmit_sat_by_remote = %d\n",
    			sbs_status_resp_group -> rexmit_sat_by_remote);
    	    (void)printf("SBS_STATUS_RESP_GROUP.late_rexmit = %d\n",
    			sbs_status_resp_group -> late_rexmit);
    	    (void)printf("SBS_STATUS_RESP_GROUP.rexmit_reqs_from_remote = %d\n",
    	 		sbs_status_resp_group -> rexmit_reqs_from_remote);
    	    (void)printf("SBS_STATUS_RESP_GROUP.rexmit_sat_by_local = %d\n",
    			sbs_status_resp_group -> rexmit_sat_by_local);
    	    z = z + sizeof(struct _SBS_STATUS_RESP_GROUP);    	    z = z + sizeof(struct _SBS_STATUS_RESP_GROUP);
    	    }    /*  end of group for loop */
	}    /* end of group if/else */

    }
	
(void)printf("\n\n");

     break;

     case PAMS__NOMOREMSG :
	/*  Status returned when queue is empty  */
     break;

     case PAMS__TIMEOUT :
	/*  Status returned when queue is empty  */
     break;

     default :
        pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);
	(void)printf( "\n\tError getting message:...status returned is: %s (%ld)\n\n",
	      msg_text,
	      dmq_status );
     break;
  }


  return ( dmq_status );

} /* end of ExampleGetMOTStatus  */



int32
ExampleGetRegReply( )
{
   SBS_REGISTER_RESP reg_reply;
   char        priority;
   short       msg_class;
   short       msg_type;
   short       msg_data_len;
   short       msg_area_len;
   int32       dmq_status;
   int32       sel_filter;
   int32       timeout;
   q_address   msg_source;
   struct PSB  get_psb;

   /* ............................................................... */
   /*  Get the reply to the sbs_registration message                  */ 
   /*................................................................ */
   priority       = 0;

   /* ................................................ */
   /*  select all messages, i.e. don't filter any out  */
   /* ................................................ */
   sel_filter     = 0;
   timeout        = 100;

   msg_area_len   = sizeof (struct _SBS_REGISTER_RESP);
   dmq_status     = PAMS__SUCCESS;

  /* ................................................. */
  /*  Initialize the message area to hold the message  */
  /* ................................................. */
  (void)memset((void*)&reg_reply, '\0', sizeof(struct _SBS_REGISTER_RESP) );

  /* ............... */
  /*  Get the reply  */
  /* ............... */
  dmq_status = pams_get_msgw((char *) &reg_reply, &priority, &msg_source,
		    &msg_class, &msg_type, &msg_area_len, &msg_data_len,
		    &timeout, &sel_filter, &get_psb, (struct show_buffer *) 0,
		    (int32 *) 0, (int32 *) 0, (int32 *) 0, (char *) 0 );

  switch ( dmq_status ) {
     case PAMS__SUCCESS :
	(void)printf("Got Sbs Reply:");
	(void)printf ("\tstatus = %d registration_id = %d registration_number = %d\
           user_tag = %d\n", 
		    reg_reply.status, reg_reply.reg_id,
		    reg_reply.number_reg,
                    reg_reply.user_tag);
     break;

     case PAMS__NOMOREMSG :
	/*  Status returned when queue is empty  */
     break;

     case PAMS__TIMEOUT :
	/*  Status returned when queue is empty  */
     break;

     default :
        pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);
	(void)printf( "\n\tError getting message:...status returned is: %s (%ld)\n\n",
	      msg_text,
	      dmq_status );
     break;
  }


  return ( dmq_status );

} /* end of ExampleGet  */

int32
ExampleGetDeregReply( )
{
   SBS_DEREGISTER_RESP reg_reply;
   char        priority;
   short       msg_class;
   short       msg_type;
   short       msg_data_len;
   short       msg_area_len;
   int32       dmq_status;
   int32       sel_filter;
   int32       timeout;
   q_address   msg_source;
   struct PSB  get_psb;

   /* ............................................................... */
   /*  Get the reply to the sbs_registration message                  */ 
   /*................................................................ */
   priority       = 0;

   /* ................................................ */
   /*  select all messages, i.e. don't filter any out  */
   /* ................................................ */
   sel_filter     = 0;
   timeout        = 100;

   msg_area_len   = sizeof (struct _SBS_DEREGISTER_RESP);
   dmq_status     = PAMS__SUCCESS;

  /* ................................................. */
  /*  Initialize the message area to hold the message  */
  /* ................................................. */
  (void)memset((void*)&reg_reply, '\0', sizeof(struct _SBS_DEREGISTER_RESP) );

  /* ............... */
  /*  Get the reply  */
  /* ............... */
  dmq_status = pams_get_msgw((char *) &reg_reply, &priority, &msg_source,
		    &msg_class, &msg_type, &msg_area_len, &msg_data_len,
		    &timeout, &sel_filter, &get_psb, (struct show_buffer *) 0,
		    (int32 *) 0, (int32 *) 0, (int32 *) 0, (char *) 0 );

  switch ( dmq_status ) {
     case PAMS__SUCCESS :
	(void)printf("Got Sbs Reply:");
	(void)printf ("\tstatus = %d  registration_number = %d\
           user_tag = %d\n", 
		    reg_reply.status,
		    reg_reply.number_reg,
                    reg_reply.user_tag);
     break;

     case PAMS__NOMOREMSG :
	/*  Status returned when queue is empty  */
     break;

     case PAMS__TIMEOUT :
	/*  Status returned when queue is empty  */
     break;

     default :
        pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);
	(void)printf( "\n\tError getting message:...status returned is: %s (%ld)\n\n",
	      msg_text,
	      dmq_status );
     break;
  }


  return ( dmq_status );

} /* end of ExampleDeregGet  */


void
ExampleExit( )
{
   int32    dmq_status;


   dmq_status    = pams_exit();

   if ( dmq_status == PAMS__SUCCESS )
      (void)printf( "\nExited from MessageQ.\n" );
   else
      {
      pams_status_text(&dmq_status,&severity, msg_text,&msg_text_len,
                                &ret_len);

      (void)printf( "\nError exiting;...status returned is: %s (%ld)\n\n",
                  msg_text,
                  dmq_status );
      }	
} /*  end of ExampleExit  */


