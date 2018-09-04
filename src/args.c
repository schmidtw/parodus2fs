/**
 * Copyright 2018 Weston Schmidt
 * Copyright 2018 Comcast Cable Communications Management, LLC
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

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "args.h"

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

/* See args.h for details. */
int process_args( int argc, char **argv, libpd_cfg_t *cfg, char **cfg_file )
{
    const char *option_string = "c:f:h:p:s::";
    static const struct option options[] = {
        { "client-url",   optional_argument, 0, 'c' },
        { "config",       optional_argument, 0, 'f' },
        { "help",         optional_argument, 0, 'h' },
        { "parodus-url",  optional_argument, 0, 'p' },
        { "service-name", optional_argument, 0, 's' },
        { 0, 0, 0, 0 } };


    int item = 0, opt_index = 0;

    while (-1 != (item = getopt_long(argc, argv, option_string, options, &opt_index))) {
        switch (item) {
            case 'c':
                cfg->client_url = strdup( optarg );
                break;
            case 'f':
                *cfg_file = strdup( optarg );
                break;
            case 'p':
                cfg->parodus_url = strdup( optarg );
                break;
            case 's':
                /* This is ONLY ok since the original name is a const. */
                cfg->service_name = strdup( optarg );
                break;
            case 'h':
                return -1;
            default:
                break;
        }
    }

    return 0;
}

/* See args.h for details. */
void print_usage( const char *name )
{
    printf( "Usage:%s -p <parodus_url> -c <client_url> "
            "[-f </path/to/config_file.json>]\n", name );
}


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/* none */
