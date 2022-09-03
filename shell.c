/* AUTH: Ethan Christensen
 * DATE: 11-24-21 (DUE: 112-12-21)
 * COURSE: CSCI: 4500 (SEC: 002)
 * PROJECT: #1
 * DESC: Implement Preprojects and knowledge or operating systems
 * ----> to write a simple shell program to proccess user inputted
 * ----> commands
*/

/* =======================INCLUDES======================= */
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<signal.h>
#include<sys/errno.h>
#include<sys/types.h>
#include<sys/wait.h>

/* =======================DEFINES======================= */
#define MAX_TOKEN 10
#define MAX_CMDLINE_SIZE 200
#define MAX_JOBS_PER_LINE 100
#define MAX_PATH_SIZE 50
#define MAX_SEQOPS 100
#define MAX_SIMPLE_CMDS 100
#define MAX_ARGS_PER_CMD 100
#define BUF_SIZE 256
#define PATH "PATH"

/* =======================GLOBALS======================= */
char cmdline[MAX_CMDLINE_SIZE];
char *jobs[MAX_JOBS_PER_LINE];
char *paths[MAX_PATH_SIZE];
int seqops[MAX_SEQOPS];
char *simple_cmds[MAX_SIMPLE_CMDS];
char *cmd;
char *cmdargs[MAX_ARGS_PER_CMD];
char *path = NULL;
char buf[200];
extern char **environ;

/* =======================PROTOTYPES======================= */

void show_prompt( char *prompt);

int split_into_jobs( char *cmdline, char *jobs[], size_t count );

int scan_seqops( int seqops[], char *jobstr, size_t count );

int extract_simple_cmds( char *simple_cmd, char *cmdargs[], size_t count );

int extract_cmd_args( char *simple_cmd, char **cmd, char *cmdargs[], size_t count );

int readline( int fd, char *buf, int bufsz );

int writeline( int fd, const char *str );

int split_into_paths( char *path, char *paths[], size_t count );

/* =======================FUNCTIONS======================= */

/* uses the write function to write out character to the the specified*/
/* file descriptor. takes in a string and writes out a character one  */
/* at a time until a newline or null character is found, inwhich a    */
/* newline character is printed.                                      */
/* RETURN: number of chars written                                    */
int writeline( int fd, const char *str ) {
	int i;
	const size_t MAXSTRLEN = 256;
	for( i = 0; i < MAXSTRLEN; i++ ) {
		if( str[i] == '\0' ) {
			write( fd, "\n", 1 );
			break;
		} else {
			write( fd, &str[i], 1 );
		}
	}
	return i + 1; /* return number of char written */
}

/* Uses the readline function to pull data from stdin It then runs    */
/* through each character checking for the '\n' or '\0' signialing    */
/* the end of the line                                                */
/* RETURN: the number of characters read                              */
int readline ( int fd, char *buf, int bufsz ) {
        int i;
        int rval;
        for( i = 0; i < bufsz - 1; i++ ) {
                rval = read( fd, &buf[i], 1 );
                if ( rval == 1) {
                        if( buf[i] == '\n') {
                                buf[i] = '\0';
                                return i + 1;
                        } else if (buf[i] == '\0') {
                                buf[i] = '\n';
                        }
                } else if ( rval == 0 ) {
                        return i;
                }
        }
        return i + 1;
}

/* Will show the inputted string and flush stdout                    */
/* RETURN: NA                                                        */
void show_prompt ( char *prompt ) {
    write( 2, prompt, strlen(prompt) );
    fflush( stdout );
}

/* This a PATH string into seperate paths that can be used to check  */
/* if a user inputted command exists on the machine                  */
/* RETURN: number of paths                                           */
int split_into_paths( char *path, char *paths[], size_t count ) {
    int i;
    char *token;
    for ( i = 0, token = strtok( path, ":"); token != NULL && i < count; token = strtok( NULL, ":" ), ++i ) {
        paths[i] = token;
    }
    return i;
}

/* takes in a cmdline string and splits the line at ";"             */
/* RETURN: number of jobs                                           */
int split_into_jobs ( char *cmdline, char *jobs[], size_t count ) {
    int i;
    char *token;
    for ( i = 0, token = strtok( cmdline, ";" ); token != NULL && i < count; token = strtok( NULL, ";" ), ++i ) {
        jobs[i] = token; /* stored into global jobs array */
    }
    return i; /* numbers of jobs */
}

/* given a job it will search for "&&" and  "||" and mark them into */
/* the seqops array where 1 -> "&&" and 2 -> "||"                   */
/* RETURN: number of seqops in the job                              */
int scan_seqops ( int seqops[], char *jobstr, size_t count ) {
    int i, sq;
    i = 0; /* = position in job string */
    sq = 0; /* = number of seqops found */
    while ( jobstr[i] != '\0') { /* looping through the job to find all "&&" and "||" */
        if ( jobstr[i] == '|' ){
            seqops[sq] = 2;
            sq++;
            i+=2; /* skip over the second '|' */
        } else if ( jobstr[i] == '&' ) {
            seqops[sq] = 1;
            sq++;
            i+=2; /* skip over the second '&' */
        } else {
            i++;
        }
    }
    return sq;
}

/* now that we know where the seqops happen we can tokenize it and   */
/* remove the "||" and  "&&" without worry split the job down into   */
/* simple groups of commands that were seperated by "||" and "&&"    */
/* RETURN: number of commands delimited by "||" and "&&"             */
int extract_simple_cmds ( char *jobstr, char *simple_cmds[], size_t count ) {
    int i;
    char *token;
    for ( i = 0, token = strtok( jobstr, "|&" ); token != NULL && i < count; token = strtok( NULL, "|&" ), ++i ) {
        simple_cmds[i] = token; 
    }
    return i;
}


/* given a simple command, the first word is the command and anything*/ 
/* that follows is an argument                                       */
/* RETURN: number of tokens -> cmd + total args                      */
int extract_cmd_args( char *simple_cmd, char **cmd, char *cmdargs[], size_t count ) {
    int i;
    char *token;
    for ( i = 0, token = strtok( simple_cmd, " " ); token != NULL && i < count; token = strtok( NULL, " " ), ++i ) {
        if ( i == 0 ) {
            *cmd = token;
        } else {
            cmdargs[i] = token; /* need to offset i because i already incremented once for the first word */
        }
    }
    return i;
}

/* =======================MAIN======================= */
int main(void) {
    int bytes_read, total_jobs, total_paths, /*seqops_cnt,*/ total_simple_cmds, total_tokens, i, /*sn,*/ j, k, /*t,*/ child_status, l, current_seqop;
    char check_path[200];
    pid_t pid, which_child;

    writeline( 1, "" );
    writeline( 1, "***********************************************************" );
    writeline( 1, "* This is a simple shell made by Ethan Christensen for    *" );
    writeline( 1, "* CSCI4500 at the Univerity of Nebrasaka at Omaha. This   *" );
    writeline( 1, "* shell aims to emulate the a UNIX-type shell but only    *" );
    writeline( 1, "* for external commands, and not commands that are built  *" );
    writeline( 1, "* into the system. This is meant to be a simple shell and *" );
    writeline( 1, "* is not meant to emulate every command on a UNIX-type    *" );
    writeline( 1, "* shell.                                                  *" );
    writeline( 1, "*                                                         *" );
    writeline( 1, "* \033[0;36mAuthor: Ethan Christensen\033[0m                               *" );
    writeline( 1, "* \033[0;33mResources: Professor Jose Santos\033[0m                        *" );
    writeline( 1, "***********************************************************" );

    path = getenv( PATH );

    if( path == NULL ) {

        sprintf( buf, "\033[0;31mError: \033[0mgetenv() failed with \"%s\".", PATH );

        writeline( 2, buf );

        exit( EXIT_FAILURE );

    } else {

        total_paths = split_into_paths( path, paths, MAX_PATH_SIZE );

    }

    while(1) {

        show_prompt( ">> " ); /* prompt so signify program and not UNIX prompt */

        bytes_read = readline( 0, cmdline, MAX_CMDLINE_SIZE ); /* read the line */

        writeline( 1, "" );

        if ( bytes_read > 0 ) { /* if not EOF run main section */

            /*printf( "readline(): Got: \"%s\" (rval = %d)\n\n", cmdline, bytes_read); */

            total_jobs = split_into_jobs ( cmdline, jobs, MAX_JOBS_PER_LINE ); /* split the string into jobs */

            for ( i = 0; i < total_jobs; ++i ) {

                /* show_prompt( "\033[0;33m**************************DEBUG***************************\033[0m\n" ); */

                /* printf( "\033[0;36mJob #%d: \033[0m\"%s\"\n", i, jobs[i] );  printing the current job */

                scan_seqops( seqops, jobs[i], MAX_SEQOPS ); /* getting and printing the seqops for the job */

                /* printf( "\tseqops[] = [ ");  printing contents of the current state of seqops array */

                /* for ( sn = 0; sn < seqops_cnt; sn++ ) { */

                    /* printf( "%d ", seqops[sn] ); */

                /* } */
                /* printf( "]\n" ); */

                total_simple_cmds = extract_simple_cmds( jobs[i], simple_cmds, MAX_SEQOPS ); /* getting each command delimited by the seqops */

                current_seqop = 0;

                for ( j = 0; j < total_simple_cmds; ++j ) {

                    /* printf( "\tSimple Command #%d = \"%s\"\n", j, simple_cmds[j] );  printing the simple command */

                    total_tokens = extract_cmd_args( simple_cmds[j], &cmd, cmdargs, MAX_ARGS_PER_CMD ); /* breaking up the command */

                    for ( k = 0; k < total_paths; ++k ) {

                        if( strchr( &cmd[0], '/' ) != NULL ) {

                            sprintf( check_path, "%s", &cmd[0] );

                        } else {

                            sprintf( check_path, "%s/%s", paths[k], &cmd[0] );

                        }

                        if( access( check_path, X_OK ) == 0 ) {

                            pid = fork();

                            if ( pid == -1 ) {

                                writeline( 2, "\033[0;31mError: \033[0mcalling fork()." );

                                exit( EXIT_FAILURE );

                            }

                            /* CHILD RUNNING THE COMMAND WHILE PARENT WAITS */
                            if( pid == 0 ) {
            
                                /* COMMAND WITH NO ARGUMENTS PROVIDED */
                                if ( total_tokens == 1 ) {

                                    /* printf( "\t\t\033[0;35mpath:\033[0m\t\t%s\n", check_path ); */

                                    /* printf( "\t\t\033[0;33mcmd:\033[0m\t\t\"%s\"\n", &cmd[0] );  print lone command */

                                    /* printf( "\t\t\033[0;32margs:\033[0m\t\t(None supplied)\n");  print no args */
                                    
                                    cmdargs[0] = check_path;

                                    /* show_prompt( "\033[0;33m************************END-DEBUG*************************\033[0m\n" ); */

                                    if( execve( check_path, cmdargs, environ ) < 0 ) {

                                        writeline( 2, "\033[0;31mError: \033[0mexecve()." );

                                        exit( EXIT_FAILURE );

                                    }

                                /* COMMAND WITH ARGUMENTS PROVIDED */
                                } else {

                                    /* printf( "\t\t\033[0;35mpath:\033[0m\t\t%s\n", check_path ); */

                                    /* for ( t = 0; t < total_tokens; ++t ) {  print the command and args */

                                        /* if ( t == 0 ) { */

                                            /*printf( "\t\t\033[0;33mcmd:\033[0m\t\t\"%s\"\n", &cmd[0] );  print lone command */

                                        /* } else { */

                                            /* printf( "\t\t\033[0;32marg[%d]:\033[0m\t\t\"%s\"\n", t - 1, cmdargs[t] );  print args */

                                        /* } */
                                    /* } */

                                    cmdargs[0] = check_path;

                                    /* show_prompt( "\033[0;33m************************END-DEBUG*************************\033[0m\n" ); */

                                    if( execve( check_path, cmdargs, environ ) < 0 ) {

                                        writeline( 2, "\033[0;31mERROR:\033[0m execve()." );

                                        exit( EXIT_FAILURE );

                                    }
                                }
                            }

                            /* PARENT WAITING WHILE CHILD RUNS */
                            else if( pid > 0 ) {

                                for (l = 0; l < total_tokens; ++l ) {
                            
                                    cmdargs[l + 1] = NULL;
                                
                                } 

                                which_child = wait ( &child_status );

                                /* printf("%d: %d\n", current_seqop ,seqops[current_seqop]); */

                                /* printf("%d\n", child_status); */

                                if( seqops[current_seqop] == 1 ) {

                                    if ( child_status == 0 ) {

                                        current_seqop++;

                                    } else {

                                        current_seqop+=2;

                                        j++;

                                    }

                                } else if( seqops[current_seqop] == 2 ) {

                                    if ( child_status == 0 ) {

                                        current_seqop+=2;

                                        j++;

                                    } else {

                                        current_seqop++;

                                    }
                                }


                                if( which_child == -1 ) {

                                    writeline( 2, "\033[0;31mError: \033[0mwait() failed." );

                                    exit( EXIT_FAILURE );

                                }
                            }

                            memset( check_path, 0, 200 );

                            break;
                        }

                        if ( k == total_paths - 1 ) { /* comand not found */

                            /* show_prompt( "\033[0;33m************************END-DEBUG*************************\033[0m\n" ); */

                            sprintf( buf, "\033[0;31mError: \033[0;33mpath: \033[0m\"%s\" not found.", &cmd[0] );

                            writeline( 2, buf );

                            memset( buf, 0, strlen( buf ) );

                            for (l = 0; l < total_tokens; ++l ) {
                            
                                memset( &cmdargs[l], 0, 100 );
                                
                            }

                        }                      
                    }
                }

                writeline( 1, "" );

            }
        } else {

            sprintf( buf, "\nreadline(): Returned with code = %d", bytes_read );

            writeline( 1, buf); /* readline gets EOF */

            memset( buf, 0, strlen( buf ) );

            writeline( 1, "Exiting . . .\n" ); /* print exiting prompt */

            exit( EXIT_SUCCESS ); /* exit program */

        }
    }
}

