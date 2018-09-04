/**
 * Copyright 2018 Weston Schmidt
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <signal.h>
#include <stdlib.h>

#include "sigs.h"
#include "logging.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/* See sigs.h for details. */
void sig_exit( int sig )
{
    d_info( "Signal %s (%d) received.  Program Terminating.\n",
            sig_to_string(sig), sig );
    exit( 0 );
}


/* See sigs.h for details. */
void sig_ignore( int sig )
{
    d_info( "Signal %s (%d) received.  Ignoring.\n",
            sig_to_string(sig), sig );

    /* reset it to this function */
    signal(sig, sig_ignore);
}


/* See sigs.h for details. */
const char * sig_to_string( int sig )
{
    switch( sig ) {
        case SIGABRT: return "SIGABRT";
        case SIGALRM: return "SIGALRM";
        case SIGBUS:  return "SIGBUS";
        case SIGCHLD: return "SIGCHLD";
        case SIGFPE:  return "SIGFPE";
        case SIGHUP:  return "SIGHUP";
        case SIGILL:  return "SIGILL";
        case SIGINT:  return "SIGINT";
        case SIGKILL: return "SIGKILL";
        case SIGPIPE: return "SIGPIPE";
        case SIGQUIT: return "SIGQUIT";
        case SIGSEGV: return "SIGSEGV";
        case SIGUSR1: return "SIGUSR1";
        case SIGUSR2: return "SIGUSR2";
        default:      break;
    }

    return "unknown";
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/* none */
