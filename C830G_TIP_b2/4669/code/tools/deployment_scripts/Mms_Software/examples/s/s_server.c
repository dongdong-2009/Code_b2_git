/*
** Copyright (c) 1992, 1997 BEA Systems, Inc.
** All Rights Reserved.
**
** Unpublished rights reserved  under the copyright laws of the
** United States.
**
** The software  contained on this  media is proprietary to and
** embodies the  confidential  technology  of  BEA Systems, Inc.
** Possession,  use,  duplication or dissemination of the  soft-
** ware  and media is  authorized only pursuant to a valid writ-
** ten license from BEA Systems, Inc.
**
** RESTRICTED RIGHTS LEGEND Use,  duplication, or disclosure by
** the U.S.  Government is subject to restrictions as set forth
** in Subparagraph (c)(1)(ii) of  DFARS 252.227-7013, or in FAR
** 52.227-19, as applicable.
**
**++
**
** FACILITY      : MessageQ
** MODULE NAME   : s_server.c
**
** ABSTRACT      : This program receives messages and echoes
**                 them back to the sender
**
** ENVIRONMENT   : set the environment variable DMQ_BUS_ID and 
**                 DMQ_GROUP_ID to your bus and group number before
**                 starting the s_server.  For instance using csh syntax
**
**                 setenv DMQ_BUS_ID 1
**                 setenv DMQ_GROUP_ID 1
**
** USAGE         :
**
**      s_server [-q queue_number] [-n queue_name] [-d] [-h]
**
**          -q queue_number   specifies the address of a permanent primary
**                            queue that server should attach to.
**          -n queue_name     specifies the name of a permanent primary
**                            queue that server should attach to. MessageQ
**                            will search the GNT to translate the name. 
**          -d                enables debug mode
**          -h                prints a simple help message
**
**  This process uses message classes and types that must be defined in the
**  p_typecl.h header file as follows:
**
**      #define MSG_CLAS_EXAMPLES            2000
**      #define MSG_TYPE_CLIENT_REQ             1
**      #define MSG_TYPE_SERVER_RESP            2
**
**  They are conditionally defined below if they are not already defined in
**  the proper place.
**
** MODIFICATION HISTORY :
**
** 98/05/04 zhou    mqlint changes 
** 97/12/16 SML0482 fix includes to have paths\
**
**
**--
*/

/* standard C language include files.  */

#include <stdio.h>                       /* standard I/O definitions       */
#include <signal.h>                      
#include <string.h>                      

/* MessageQ include files */

#include <p_entry.h>                 /* PAMS function declarations     */
#include <p_proces.h>                /* Known Queue number definitions */
#include <p_group.h>                 /* Known Group ID definitions     */
#include <p_typecl.h>                /* Generic Type/Class definitions */
#include <p_return.h>                /* PAMS return status definitions */
#include <p_symbol.h>                /* Generic PSEL/PSYM definitions  */

#ifndef FALSE
#define TRUE 1
#define FALSE 0
#endif

/*
 * The user should have defined these in p_typecl.h - but just in case they
 * weren't...
 */

#ifndef MSG_CLAS_EXAMPLES
#define MSG_CLAS_EXAMPLES            2000
#define MSG_TYPE_CLIENT_REQ             1
#define MSG_TYPE_SERVER_RESP            2
#endif

#define BUFFER_SIZE 80

/* forward declare functions */

void print_error(char *, int32);        
/* print and error message given the status */
void print_usage(void);              
void exit_handler(void);       /* signal handler to trap SIGINT and exit cleanly */

/* Declare global variables, pointers, and arrays */

char *program_name = "s_server";
int32 name_space_list[1] =        /* List of tables to search          */
                { PSEL_TBL_GRP };
int32 name_space_list_len = 1;    /* Number of elements in search list */


/* function and global variable definitions for command line parsing */

extern int getopt();
extern int optind, opterr;
extern char *optarg;

int debug     = FALSE;  
int by_number = FALSE;       /* if true, attach by number                */
int by_name   = FALSE;       /* if true, attach by name                  */

int
main(int     argc, char    **argv)
{
   int ch;
   int32          status;             /* Completion status code         */
   int32          attach_mode;        /* how were are going to attach   */
   int32          q_type;             /* type of queue to attach to     */
   char           q_name[32];         /* Queue name for PAMS_ATTACH_Q   */
   int32          q_name_len;         /* Length of queue name           */
   q_address      q_attached,         /* queue we are attached to       */
                  msg_target,         /* target process                 */
                  response_queue,     /* where target should send its   */
                                      /* responses to                   */
                  msg_source;         /* who sent us the message        */
   char           msg_priority = 0;   /* receive all messages           */
   short int      msg_class,          /* message class                  */
                  msg_type;           /* message type                   */
   int32          send_timeout = 0,   /* Timeout for outbound messages: */
                                      /* Default of 30 seconds          */
                  recv_timeout = 0;   /* Timeout for inbound messages:  */
                                      /* infinite, nothing better to do */
   int32          selection_mask = 0; /* No special selection mask      */
   struct PSB     psb;                /* PAMS Status Block              */

   char           delivery_mode = PDEL_MODE_NN_MEM;  /* non-recoverable */
   char           uma = PDEL_UMA_DISCL;     /* discard if undeliverable */
   char           msg_buffer[BUFFER_SIZE + 1];       /* message buffer  */
   short int      msg_buffer_size = BUFFER_SIZE,     /* size of buffer  */
   msg_size;                          /* size of message */

/* go parse command line */

   while ((ch = getopt(argc, argv, "q:n:dh")) != EOF) {
      switch (ch) {
         case 'd' :
	     debug = TRUE;
	     break;

         case 'q' :
            (void)sscanf (optarg, "%s", q_name);      
            by_number = TRUE;
            break;

         case 'n' :
            (void)sscanf (optarg, "%s", q_name);
            by_name = TRUE;
            break;

         case '?' :
         case 'h' :
            print_usage();
            return(1);
      }
   }

   if (by_name && by_number) {
      (void)fprintf(stderr, "%s : must select either attach\n", program_name);
      (void)fprintf(stderr, " by queue or attach by name - but not both!\n");
      print_usage();
      return(1);
   }
   
   if (! (by_name || by_number)) {
      (void)fprintf(stderr, "%s : must select either attach\n", program_name);
      (void)fprintf(stderr, " by queue or attach by name!\n");
      print_usage();
      return(1);
   }
 
 
/* Install signal handler to trap SIGINT (CTRL-C) so that
   we can detach from the message bus  and  exit cleanly. */

#ifdef SIGINT
   (void)signal(SIGINT, (void(*)(int))exit_handler);
#endif

#ifdef SIGQUIT
   (void)signal(SIGQUIT, (void(*)(int))exit_handler);
#endif

#ifdef SIGBREAK
   (void)signal(SIGBREAK, (void(*)(int))exit_handler);
#endif

#ifdef SIGTERM
   (void)signal(SIGTERM, (void(*)(int))exit_handler);
#endif

/* setup attributes for our primary queue and the attach mode. Note that
   the q_name parameter of pams_attach_q is used to pass the queue number,
   for attach by number, or the queue name, for attach by name. */

   if (by_name)
      attach_mode = PSYM_ATTACH_BY_NAME;      /* user passed us a name */
   else {
      attach_mode = PSYM_ATTACH_BY_NUMBER;    /* user passed us an address */
   }

   q_name_len = strlen(q_name);
   q_type = PSYM_ATTACH_PQ;                    /* primary queue */


/* Attach server process to a permanent queue (i.e., a well-known address    */

   status = pams_attach_q (&attach_mode,         /* as specified by user     */
			   &q_attached,          /* returns our queue number */
			   &q_type,              /* queue type -> primary    */
			   q_name,               /* queue name or number     */
			   &q_name_len,          /* length of q_name string  */
			   name_space_list,      /* namespace search list    */
			   &name_space_list_len, /* # namespaces to search   */
			   NULL, NULL, NULL);    /* reserved - must be NULL  */

   if (status != PAMS__SUCCESS) {
      print_error("Error returned by PAMS_ATTACH_Q", status);
      return(1);
   }

   if (debug) {
      (void)printf("\n\n%s : Attached to queue %hd.%hd.\n", 
      program_name, q_attached.au.group, q_attached.au.queue);
   }

   for(;;) {   /* server's main processing loop */

   /* Wait (forever) for a  message  to  be  sent  by  a
      client program If any errors are detected,  report
      it and return. Otherwise, echo the message to stdout. */

      if (debug) {
         (void)fprintf(stderr,"\n%s : Waiting for a request.\n", program_name);
      }

      status = pams_get_msgw(msg_buffer,        /* buffer to store msg in */
			     &msg_priority,     /* message priority       */
			     &msg_source,       /* who sent it to us      */
			     &msg_class,        /* message class          */
			     &msg_type,         /* message type           */
			     &msg_buffer_size,  /* size of message buffer */
			     &msg_size,         /* size of message        */
			     &recv_timeout,     /* how long should we wait*/
			     &selection_mask,   /* default to FIFO order  */
			                        /* and primary queue      */
			     &psb,              /* PAMS status block      */
			     (struct show_buffer *) NULL, /* show buffer  */
			     (int32 *) NULL,     /* show buffer length     */
			     NULL, NULL, NULL); /* reserved - must be NULL*/
      
      if (status != PAMS__SUCCESS) {
         print_error("Error returned by PAMS_GET_MSGW.\n", status);
         pams_exit();                 
         return(status);
      }

      if ((msg_class == MSG_CLAS_EXAMPLES)   &&
	     (msg_type  == MSG_TYPE_CLIENT_REQ)) {
	 
         (void)printf("\n%s : Received message from %hd.%hd.\n",
	        program_name, msg_source.au.group, msg_source.au.queue);
         (void)printf("%s : Message class = %hd, Message type = %hd\n",
	        program_name, msg_class, msg_type);
         (void)printf ("%s : Message = '%s'\n",program_name, msg_buffer);
	 
      /* Return received message to the client program. */
	 
	     msg_target.all = msg_source.all;      /* send it back to source */
	     response_queue.all = q_attached.all;  /* our primary queue      */
	 
	     msg_class = MSG_CLAS_EXAMPLES;
	     msg_type = MSG_TYPE_SERVER_RESP;      /* respond to request     */
	 
	     if (debug) {
	        (void)fprintf(stderr, "\n%s : Sending a response to %hd.%hd\n",
		    program_name, msg_target.au.group, msg_target.au.queue);
	     }	 

	     status = pams_put_msg(msg_buffer,           /* message to send      */
                      &msg_priority,     /* message priority     */
			          &msg_target,       /* who to send it to    */
			          &msg_class,        /* message class        */
			          &msg_type,         /* message type         */
			          &delivery_mode,    /* is it recoverable?   */
			          &msg_buffer_size,  /* size of msg buffer   */
			          &send_timeout,     /* how long to wait     */
			          &psb,              /* PAMS status block    */
			          &uma,          /* undeliverable msg action */
			          &response_queue,   /* return address       */
			          NULL, NULL, NULL); /* reserved - NULL      */

         if (status != PAMS__SUCCESS) {
            print_error("Error returned by PAMS_PUT_MSG.\n", status); 
            status = pams_exit();
            return(status);
         }

         (void)printf("\n%s : Sent Message to %hd.%hd.\n",
	        program_name, msg_target.au.group, msg_target.au.queue);
         (void)printf("%s : Message class = %hd, Message type = %hd\n",
	        program_name, msg_class, msg_type);
         (void)printf ("%s : Message = '%s'\n", program_name, msg_buffer);
      } /* end if msg_class ... */

      else {  /* not proper message class and or type */
         (void)fprintf(stderr, "\n%s : Received message of unknown", program_name);
         (void)fprintf(stderr, " class or type - Message discarded!\n");
      }
   } /* end of main processing loop */
} /* end of main */


void
print_usage()
{
    (void)fprintf(stderr,"usage: s_server [-q queue_number | -n queue_name] [-d] [-h]\n");
    (void)fprintf(stderr,"  -q queue_number  Server processes queue number.\n");
    (void)fprintf(stderr,"  -n queue_name    Name of the server processes queue.\n");
    (void)fprintf(stderr,"  -d               Enable debug mode.\n");
    (void)fprintf(stderr,"  -h               Print this message.\n\n");
}


void
exit_handler(void)
{
   if (debug) {
      (void)fprintf(stderr,"\n%s : Detaching from message bus\n", program_name);
   }

   (void)pams_exit();

   if (debug) {
      (void)fprintf(stderr, "%s : Program returning\n", program_name);
   }
}


void
print_error(char *msg, int32 status)
{
   (void)fprintf(stderr, "%s : %s -> %d.\n", program_name, msg, (int)status);

   switch (status) {
      case PAMS__DECLARED:
         (void)fprintf(stderr, "Queue is in use by another process.\n");
         break;

      case PAMS__NOOBJECT:
         (void)fprintf(stderr, "Queue is unknown.\n");
         break;

      case PAMS__RESRCFAIL:
         (void)fprintf(stderr, "Failed to allocate resource.\n");
         break;

      case PAMS__NOTACTIVE:
         (void)fprintf(stderr, "Queue is not currently active.\n");
         break;

      case PAMS__PAMSDOWN:
         (void)fprintf(stderr, "MessageQ group is down.\n");
         break;

      case PAMS__TIMEOUT:
         (void)fprintf(stderr, "Function timed out.\n");
         break;

      case PAMS__NOTDCL:
         (void)fprintf(stderr, "Process is not declared to MessageQ.\n");
         break;

      case PAMS__EXCEEDQUOTA:
         (void)fprintf(stderr, "Target has insufficient quota to receive message.\n");
         break;

      case PAMS__EXHAUSTBLKS:
         (void)fprintf(stderr, "No more message blocks available\n");
         break;

      case PAMS__REMQUEFAIL:
         (void)fprintf(stderr, "Failed to properly dequeue message\n");
         break;

      case PAMS__NETNOLINK:
      case PAMS__NETLINKLOST:
         (void)fprintf(stderr, "No link or link lost to target group\n");
         break;

      case PAMS__AREATOSMALL:
         (void)fprintf(stderr, "Message received too large for buffer\n");
         break;

      case PAMS__INSQUEFAIL:
         (void)fprintf(stderr, "Failed to properly queue a message\n");
         break;

      case PAMS__NOTSUPPORTED:
         (void)fprintf(stderr, "Operation not supported.\n");
         break;

      case PAMS__MSGUNDEL:
         (void)fprintf(stderr, "Message returned is undeliverable.\n");
         break;

      default:
         (void)fprintf(stderr,"Record status and refer to the documentation");
         (void)fprintf(stderr," for more information.\n");
   }
}
