/*
** Copyright (c) 1991, 1997 BEA Systems, Inc.
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
** 98/05/04 zhou    mqlint changes
**/

#include <stdio.h>
#include <string.h>


#ifdef WIN32
static int print_err(char *, char *, int);

int opterr = 1;
int optind = 1;        /* next index into **argv               */
char *optarg = NULL;   /* holds option parameter if one exists */

int
getopt(int argc, char **argv, char *options)
{
   static int sp = 1;       /* position in argument */
   int old_sp;                 
   int c;                   /* option letter */
   char *cp;                /* option in options string */

   optarg = NULL;

   if (sp == 1) {
      if (optind >= argc ||         /* no more arguments */
	     argv[optind][0] != '-' ||  /* no more options   */
	     argv[optind][1] == '\0')   /* stdin */

         return EOF;                /* we're done */

      else if (strcmp(argv[optind], "--") == 0) {
         ++optind;                  /* skip it */
         return EOF;                /* we're done */
      }
   }

   c = argv[optind][sp];              /* this option letter */
   old_sp = sp++;                     /* next option letter */

   if (argv[optind][sp] == '\0') {    /* end of argument */        
      ++optind;
      sp = 1;                        /* beginning of next argument */
   }
	    
   if (c == ':' || (cp = strchr(options, c)) == NULL)
      return (print_err(argv[0], "illegal option", c));

   if (cp[1] == ':') {                 /* this option takes a parameter */    
      if (old_sp != 1)
         return (print_err(argv[0], "option must be clustered", c));

      if (sp != 1)
         return (print_err(argv[0], "option must be followed by whitespace",
			      c));
      if (optind >= argc)
         return (print_err(argv[0], "option requires an argument", c));

      optarg = argv[optind];            /* make parameter available */
      ++optind;                         /* skip over parameter      */
   }

   return (c);
}	



static int
print_err(char *name, char *msg, int   c)
{
   fprintf(stderr, "%s : %s -- %c\n", name, msg, c);
   return('?');     
}
#endif
