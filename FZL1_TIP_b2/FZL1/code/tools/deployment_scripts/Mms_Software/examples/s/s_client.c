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
** MODULE NAME   : s_client.c
**
** ABSTRACT      : This program sends messages to a echo server
**                 located on a permanent queue. 
**
** ENVIRONMENT   : set the environment variable DMQ_BUS_ID and 
**                 DMQ_GROUP_ID to your bus and group number before
**                 starting the s_client.  For instance using sh syntax
**
**                 DMQ_BUS_ID=1
**                 DMQ_GROUP_ID=2
**                 export DMQ_BUS_ID DMQ_GROUP_ID
**
** USAGE         :
** 
**      s_client [-g group_number] [-q queue_number] [-n queue_name] [-d] [-h]
**
**          -g group_number   Specifies the group that the server process
**                            is running in.
**          -q queue_number   Specifies the address of the server process.
**                            The -q and -n options cannot be used together. 
**          -n queue_name     Specifies the name of the server process. DMQ
**                            will search the process cache and the GNT to
**                            translate queue_name. The -n and -q options
**                            cannot be used together.
**          -d                Enables debug mode.
**          -h                Prints a simple help message.
**
**
** This process uses message classes and types that must be defined in the
** p_typecl.h header file as follows:
**
**      #define MSG_CLAS_EXAMPLES            2000
**      #define MSG_TYPE_CLIENT_REQ             1
**      #define MSG_TYPE_SERVER_RESP            2
**
** These are conditionally define them in below if they are not already
** defined in the proper place. 
**
** MODIFICATION HISTORY :
**
** 98/05/04 zhou    mqlint changes 
** 97/12/16 SML0482 fix includes to have paths
**
**
**--
*/

/* standard C language include files.  */

#include <stdio.h>                       /* standard I/O definitions       */
#include <string.h>  

/* MessageQ include files */

#include <p_entry.h>                  /* PAMS function declarations     */
#include <p_proces.h>                 /* Known Queue number definitions */
#include <p_group.h>                  /* Known Group ID definitions     */
#include <p_typecl.h>                 /* Generic Type/Class definitions */
#include <p_return.h>                 /* PAMS return status definitions */
#include <p_symbol.h>                 /* Generic PSEL/PSYM definitions  */

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
#define PROGRAM_NAME "s_client"

/* forward declare functions */

int32 translate_name(char*, q_address *, int32*);   
/* translate queue name to address  */
void print_usage(void);                      /* prints some helpful instructions */
void print_error(char *, long);      
/* print error message given status */


/* Declare global variables, pointers, and arrays */

int32 name_space_list[1] =        /* List of tables to search          */
  { PSEL_TBL_GRP };
int32 name_space_list_len = 1;    /* Number of elements in search list */


/* function and global variable definitions for command line parsing */

extern int getopt();
extern int optind, opterr;
extern char *optarg;

int debug     = FALSE;  
int by_number = FALSE;       /* if true, target was specified by address */
int by_name   = FALSE;       /* if true, target was specified by name    */ 


int
main (int argc, char **argv)
{
   int        ch;
   int32      status;             /* Completion status code         */
   int32      attach_mode;        /* how were are going to attach   */
   int32      q_type;             /* type of queue to attach to     */
   char       target_name[32];    /* Queue name for PAMS_ATTACH_Q   */
   int32      target_name_len;    /* Length of queue name           */

   q_address  q_attached,         /* Address of our primary queue   */
              msg_target,         /* Address of the target process  */
              msg_source;         /* Who sent us the message        */

   char       msg_priority = 0;   /* receive all messages           */

   short int  msg_class,          /* message class                  */
              msg_type;           /* message type                   */

   int32      send_timeout = 0,   /* Timeout for outbound messages  */
                                  /* Default of 30 seconds          */

              recv_timeout = 100; /* Timeout for inbound messages   */
                                  /* 10 seconds                     */
   
   int32      selection_mask = 0; /* FIFO order from primary queue  */
   struct PSB psb;                /* PAMS Status Block              */

   char       delivery_mode = PDEL_MODE_NN_MEM;  /* non-recoverable */
   char       uma = PDEL_UMA_DISCL;     /* discard if undeliverable */ 
   char       msg_buffer[BUFFER_SIZE + 1];       /* message buffer  */
   short int  msg_buffer_size = BUFFER_SIZE,     /* size of buffer  */
              msg_size;                          /* size of message */

   char       cBuffer[1024];
#ifdef VMS
   char       cEof[] = "CTRL-Z";
#else
   char       cEof[] = "CTRL-D";
#endif

/* initialize target group as this group */

   msg_target.au.group = 0;

/* go parse command line */

   while ((ch = getopt(argc, argv, "g:q:n:dh")) != EOF) {
      switch (ch) {
         case 'd':                      /* turn on debug mode */
            debug = TRUE;
            break;

         case 'g':                      /* target processes group number  */
            (void)sscanf (optarg, "%hd", &msg_target.au.group);
            break;

         case 'q':                      /* target processes queue number  */
            (void)sscanf (optarg, "%hd", &msg_target.au.queue);      
            by_number = TRUE;
            break;

         case 'n':                      /* name of target processes queue */
            (void)sscanf (optarg, "%s", target_name);
            by_name = TRUE;
            break;

         case 'h':
         case '?':
            print_usage();

            return(1);
      }
   } /* finished parsing command line options */

   if (by_name && by_number) {
      (void)fprintf(stderr, "%s : Must specify target by", PROGRAM_NAME);
      (void)fprintf(stderr, " by number or by name - but not both!\n");
      print_usage();
      return(1);
   }

   if (! (by_name || by_number)) {
      (void)fprintf(stderr, "%s : must specify either the", PROGRAM_NAME);
      (void)fprintf(stderr, " targets queue number or its name!\n");
      print_usage();
      return(1);
   }

/* Attach our client process to a temporary, primary queue. */
   
   attach_mode = PSYM_ATTACH_TEMPORARY;
   q_type = PSYM_ATTACH_PQ;
   
/* Attach server process to a permanent queue (i.e., a well-known address */
   
   status = pams_attach_q (&attach_mode,       /* attach to temporary queue */
			   &q_attached,        /* returns our queue number  */
			   &q_type,            /* primary queue */
			   (char *) 0,         /* not used here */
			   (int32 *) 0,        /* not used here */
			   (int32 *) 0,        /* not used here */
			   (int32 *) 0,        /* not used here */
			   NULL, NULL, NULL);  /* reserved - must be NULL */

   if (status != PAMS__SUCCESS) {
      print_error("Error returned by PAMS_ATTACH_Q", status);
      return(1);
   }
   
   if (debug) {
      (void)printf("\n\n%s : Attached to queue %hd.%hd.\n", 
	     PROGRAM_NAME, q_attached.au.group, q_attached.au.queue);
   }


/* translate name of target to its address if necessary */
   
   if (by_name) {
      target_name_len = strlen(target_name);
       status = translate_name(target_name, (q_address*)&target_name_len, (int32*)&msg_target);

      if (status != PAMS__SUCCESS) {
         print_error("Error returned by PAMS_LOCATE_Q", status);
         status = pams_exit();
         return(status);
      }
      
      if (debug) {
         (void)fprintf(stderr, "%s : Translated name %s to address %hd\n",
         PROGRAM_NAME, target_name, msg_target.au.queue);
      }
   }

   for (;;) {   /* clients's main processing loop */

   /* get a message from the user and send it to the server process */
      

      (void)sprintf(cBuffer, "\nEnter message (up to %d characters) or %s to return\n",
         BUFFER_SIZE, cEof);
      (void)printf("%s", cBuffer);
      (void)printf("> ");
      
      if (gets(msg_buffer) == '\0') {
         break;          /* break out of while loop on EOF and return */
      }

   /* Specify message class and type. These are constants
      that must be defined in the p_typecl.h header file. */
      
      msg_class = MSG_CLAS_EXAMPLES;
      msg_type = MSG_TYPE_CLIENT_REQ;  
      
      if (debug) {
	 (void)fprintf(stderr, "\n%s : Sending a message to %hd.%hd\n",
		 PROGRAM_NAME, msg_target.au.group, msg_target.au.queue);
      }
      
      status = pams_put_msg(msg_buffer,       /* pointer to message buffer */
			    &msg_priority,    /* messages priority         */
			    &msg_target,      /* target process            */
			    &msg_class,       /* message class             */
			    &msg_type,        /* message type              */
			    &delivery_mode,   /* how to send it            */
			    &msg_buffer_size, /* size of message buffer    */
			    &send_timeout,    /* timeout for blocking puts */
			    &psb,             /* PAMS Status Block         */
			    &uma,          /* what to do if delivery fails */
			    &q_attached,      /* response queue            */
			    NULL, NULL, NULL);/* reserved - must be NULL   */
      
      if (status != PAMS__SUCCESS) {
         print_error("Error returned by PAMS_PUT_MSG", status);
         status = pams_exit();
         return(status);
      }

      (void)printf("\n%s : Sent Message to %hd.%hd.\n",
	     PROGRAM_NAME, msg_target.au.group, msg_target.au.queue);
      (void)printf("%s : Message class = %hd, Message type = %hd\n",
	     PROGRAM_NAME, msg_class, msg_type);
      (void)printf("%s : Message = '%s'\n", PROGRAM_NAME, msg_buffer);
      
   /* Wait (for default timeout) for a message to be returned
      from the server. If any errors are detected,  report it
      and return.   Otherwise,   echo  the  message  to  stdout. */
      
      if (debug) {
         (void)fprintf(stderr,"\n%s : Waiting for a response.\n", PROGRAM_NAME);
      }      

      status = pams_get_msgw(msg_buffer,      /* pointer to message buffer */
			     &msg_priority,   /* messages priority         */
			     &msg_source,     /* source of the message     */
			     &msg_class,      /* message class             */
			     &msg_type,       /* message type              */
			     &msg_buffer_size,/* size of message buffer    */
			     &msg_size,       /* size of message received  */
			     &recv_timeout,   /* how long should we wait?  */
			     &selection_mask, /* selection instructions    */
			     &psb,            /* PAMS status block         */
			     (struct show_buffer *) NULL, /* not used      */
			     (int32 *) NULL,              /* not used      */
			     NULL, NULL, NULL); /* reserved - must be NULL */
      
      if (status != PAMS__SUCCESS) {
         print_error("Error returned by PAMS_GET_MSGW", status);
         status = pams_exit();
         return(status);
      }

      (void)printf("\n%s : Message received from %hd.%hd.\n",
	     PROGRAM_NAME, msg_source.au.group, msg_source.au.queue);
      (void)printf("%s : Message class = %hd, Message type = %hd\n", 
	     PROGRAM_NAME, msg_class, msg_type);
      (void)printf ("%s : Message = '%s'\n", PROGRAM_NAME, msg_buffer);
      
   } /* end of main processing loop */
   
   if (debug) {
      (void)fprintf(stderr, "\n%s : Detaching from message bus.\n", PROGRAM_NAME);
   }

   status = pams_exit();
   
   if (debug) {
      (void)fprintf(stderr, "%s : Exiting from application.\n", PROGRAM_NAME);
   }

   return(status);
} /* end of main */


int32
translate_name(char *name, q_address *q_located, int32 *name_len)
{
   int32 status;
   int32 wait_mode = PSYM_WF_RESP;

/* wait for the response, since we don't have asynchronous */

   status = pams_locate_q(name,               /* name to translate          */
			  name_len,           /* length of name             */
			  q_located,          /* address of translated name */
			  &wait_mode,         /* wait for translation       */
			  NULL,               /* not used */
			  NULL,               /* not used */
			  name_space_list,    /* namespace search list      */
			  &name_space_list_len, /* length of list           */
			  NULL);              /* reserved - must be NULL    */  

  return (status);
}



void
print_usage()
{
   (void)fprintf(stderr,"usage: s_client [-g group_number] [-q queue_number | -n queue_name] [-d] [-h]\n");
   (void)fprintf(stderr,"  -g group_number  Server processes group number.\n");
   (void)fprintf(stderr,"  -q queue_number  Server processes queue number.\n");
   (void)fprintf(stderr,"  -n queue_name    Name of the server processes queue.\n");
   (void)fprintf(stderr,"  -d               Enable debug mode.\n");
   (void)fprintf(stderr,"  -h               Print this message.\n\n");
}



void
print_error(char *msg, long status)
{
   (void)fprintf(stderr, "%s : %s -> %d.\n", PROGRAM_NAME, msg, (int)status);

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
         (void)fprintf(stderr,
	     "Target has insufficient quota to receive message.\n");
         break;

      case PAMS__EXHAUSTBLKS:
         (void)fprintf(stderr, "No more message blocks available.\n");
         break;

      case PAMS__REMQUEFAIL:
         (void)fprintf(stderr, "Failed to properly dequeue message.\n");
         break;

      case PAMS__NETNOLINK:
      case PAMS__NETLINKLOST:
         (void)fprintf(stderr, "No link or link lost to target group.\n");
         break;

      case PAMS__AREATOSMALL:
         (void)fprintf(stderr, "Message received too large for buffer.\n");
         break;

      case PAMS__INSQUEFAIL:
         (void)fprintf(stderr, "Failed to properly queue a message.\n");
         break;

      case PAMS__NOTSUPPORTED:
         (void)fprintf(stderr, "Operation not supported.\n");
         break;

      case PAMS__MSGUNDEL:
         (void)fprintf(stderr, "Message returned is undeliverable.\n");
         break;
      default:
         (void)fprintf(stderr, "Record status and refer to the documentation");
         (void)fprintf(stderr, " for more information.\n");
   }
}
