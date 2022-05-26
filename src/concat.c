#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

typedef char bool;
#define true ((bool)1)
#define false ((bool)0)
#define MAX_SEP_LEN  512

/* functions <<< */
void printHelp(void)
{
   printf("USAGE: concat [-vhlwne] [-s <sep>] [-g <num>] [-S <sep>] [file]\n");
   printf("\n");
   printf("OPTIONS:\n");
   printf("  -s <sep>  optional separator string\n");
   printf("  -g <num>  add group separator string after <num> of lines\n");
   printf("  -S <sep>  optional group separator string (default is newline)\n");
   printf("  -e        interpret some few escape sequences (\\\\,\\t,\\n)\n");
   printf("  -l        remove empty lines\n");
   printf("  -w        remove leading whitespaces\n");
   printf("  -n        do not output the trailing newline\n");
   printf("  -v        print version info\n");
   printf("  -h        print help text\n");
   printf("\n");
}

void printUsage(void)
{
   fprintf(stderr, "USAGE: concat [-vhlwne] [-s <sep>] [-g <num>] [-S <sep>] [file]\n");
   fprintf(stderr, "run 'concat -h' for more information\n");
}

void printVersion(void)
{
   printf("VERSION: 0.1\n");
}

void replEscSeq(char *str)
{
   unsigned long rd = 0u;
   unsigned long wr = 0u;
   unsigned long n  = strlen(str);
   bool bslash      = false;

   for(rd = 0u; rd <= n ; rd++)
   {
      if(bslash)
      {
         switch(str[rd])
         {
            case '\\':
               {
                  wr++;
                  break;
               }
            case 't':
               {
                  str[wr] = '\t';
                  wr++;
                  break;
               }
            case 'n':
               {
                  str[wr] = '\n';
                  wr++;
                  break;
               }
            default:
               {
                  wr++;
                  str[wr] = str[rd];
                  wr++;
                  break;
               }
         }
         bslash = false;
      }
      else
      {
         if(str[rd] == '\\')
         {
            bslash = true;
         }
         else
         {
            str[wr] = str[rd];
            wr++;
         }
      }
   }
}

/* >>> */

int main(int argc, char * const argv[])
{
   
   char separator[MAX_SEP_LEN] = {'\0'};
   char groupsep[MAX_SEP_LEN]  = {'\n','\0'};
   FILE* input      = stdin;
   bool version     = false;
   bool help        = false;
   bool sep         = false;
   bool escape      = false;
   bool nopipe      = false;
   bool newline     = false;
   bool printSep    = false;
   bool printGrpSep = false;
   bool emptylines  = false;
   bool whitespaces = false;
   bool addlf       = true;
   int  groupsize   = 0;
   int  groupcount  = 0;
   int  opt;
   int  character;

   while((opt = getopt(argc, argv, ":vhlwneg:s:S:")) != -1) /* <<< */
   {
      switch(opt)
      {
         case 'v':
            {
               version = true;
               break;
            }
         case 'h':
            {
               help = true;
               break;
            }
         case 'l':
            {
               emptylines = true;
               break;
            }
         case 'w':
            {
               whitespaces = true;
               break;
            }
         case 'n':
            {
               addlf = false;
               break;
            }
         case 'e':
            {
               escape = true;
               break;
            }
         case 'g':
            {
               groupsize = atoi(optarg);
               if(groupsize == 0)
               {
                  fprintf(stderr, "concat error: invalid group size '%s'\n", optarg);
               }
               break;
            }
         case 's':
            {
               strncpy(separator, optarg, MAX_SEP_LEN-1); /* strncpy does not append \0 */
               sep = true;
               break;
            }
         case 'S':
            {
               strncpy(groupsep, optarg, MAX_SEP_LEN-1); /* strncpy does not append \0 */
               break;
            }
         case ':':
            {
               if( optopt == 's' )
               {
                  fprintf(stderr, "concat error: option -s (separator) needs a string value\n");
                  return 6;
               }
               else if( optopt == 'S' )
               {
                  fprintf(stderr, "concat error: option -S (group separator) needs a string value\n");
                  return 7;
               }
               else if( optopt == 'g' )
               {
                  fprintf(stderr, "concat error: option -g (groupsize) needs an integer value\n");
                  return 8;
               }
               break;
            }
         case '?':
            {
               fprintf(stderr, "concat error: unknown option => '%c'\n", optopt);
               return 1;
               break;
            }
      }
   } /* >>> */

   if(help)
   {
      printHelp();
      return 0;
   }

   if(version)
   {
      printVersion();
      return 0;
   }

   if(escape)
   {
      replEscSeq(separator);
      replEscSeq(groupsep);
   }

   if(isatty(fileno(stdin))) /* is connected to terminal, string not coming from pipe take second arg */
   {
      nopipe = true;

      if((argc - optind) < 1)
      {
         fprintf(stderr, "concat error: too few arguments\n");
         printUsage();
         return 2;
      }

      if((argc - optind) > 1)
      {
         fprintf(stderr, "concat error: too many arguments\n");
         printUsage();
         return 3;
      }

      input = fopen(argv[argc-1], "r");
      if(!input)
      {
         fprintf(stderr, "concat error: could not open file '%s'\n", argv[argc-1]);
         return 4;
      }
   }
   else /* program used in pipe take string from pipe */
   {
      if((argc - optind) > 0)
      {
         fprintf(stderr, "concat error: too many arguments\n");
         printUsage();
         return 5;
      }
   }

   while((character = fgetc(input)) != EOF)
   {
      if(printSep)
      {
         fputs(separator, stdout);
         printSep = false;
      }

      if(printGrpSep)
      {
         fputs(groupsep, stdout);
         printGrpSep = false;
      }

      if(character == '\n')
      {
         if(emptylines && newline)
         {
            continue;
         }

         newline = true;

         if(groupsize)
         {
            groupcount++;
            if(groupcount == groupsize)
            {
               printGrpSep = true;
               groupcount = 0;
               continue;
            }
         }

         if(sep)
         {
            printSep = true;
         }
      }
      else
      {
         if(whitespaces && newline && ((character == ' ') || (character == '\t')))
         {
            continue;
         }
         newline = false;
         fputc(character, stdout);
      }
   }

   if(addlf) /* add line feed to output */
   {
      fputc('\n', stdout);
   }

   if(nopipe)
   {
      fclose(input);
   }

   return 0;
}

/* vim: fmr=<<<,>>> fdm=marker
 */
