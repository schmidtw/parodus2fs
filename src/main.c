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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <libparodus.h>

#include "args.h"
#include "config.h"
#include "do_stuff.h"
#include "logging.h"
#include "sigs.h"

/*----------------------------------------------------------------------------*/
/*                          Globally Scoped Constants                         */
/*----------------------------------------------------------------------------*/
const char *__program_name = "parodus2fs";  /* Saves space for logging. */

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
static void __setup_sig_handlers( void );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/* none */

int main( int argc, char **argv)
{
    const char default_service_name[] = "fs";
    libpd_instance_t libpd;
    libpd_cfg_t libpd_cfg = {
        .service_name = default_service_name,
        .receive = true,
        .keepalive_timeout_secs = 64,
        .parodus_url = NULL,
        .client_url = NULL,
        .test_flags = 0,
    };
    char* config_file = NULL;
    int err;
    cfg_t *cfg;

    __setup_sig_handlers();

    err = process_args( argc, argv, &libpd_cfg, &config_file );

    /* Make sure we got what we wanted, or die. */
    if( (0 != err)                       ||
        (NULL == libpd_cfg.parodus_url)  ||
        (NULL == libpd_cfg.client_url)   ||
        (NULL == libpd_cfg.service_name)    )
    {
        print_usage( argv[0] );
        goto done;
    }

    cfg = config_get( config_file );
    if( NULL == cfg ) {
        goto done;
    }

    d_info( "configuration - service_name: \"%s\", parodus_url: \"%s\", "
            "client_url: \"%s\"\n", libpd_cfg.service_name,
            libpd_cfg.parodus_url, libpd_cfg.client_url );

    /* Connect up to Parodus, oh provider of communication. */
    err = -1;
    while( err != 0 ) {
        err = libparodus_init( &libpd, &libpd_cfg );

        if( 0 != err ) {
            d_error( "libparodus_init() unsuccessful: '%s'\n",
                     libparodus_strerror(err) );

            libparodus_shutdown( libpd );
            sleep( 5 );
        }
    }

    d_info( "libparodus_init() successful\n" );

    /* Do stuff... basically forever. */
    err = runloop( libpd, cfg );

    /* Clean up our link to Parodus before we exit. */
    libparodus_shutdown( libpd );

    config_free( cfg );

done:

    /* Free anything we allocated. */
    if( (NULL != libpd_cfg.service_name) &&
        (default_service_name != libpd_cfg.service_name) )
    {
        free( (char*) libpd_cfg.service_name );
    }

    if( NULL != libpd_cfg.client_url ) {
        free( (char*) libpd_cfg.client_url );
    }

    if( NULL != libpd_cfg.parodus_url ) {
        free( (char*) libpd_cfg.parodus_url );
    }

    if( NULL != config_file ) {
        free( config_file );
    }

	return err;
}


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Setup the signal handlers.
 */
static void __setup_sig_handlers( void )
{
    /* Please keep these alphabetized. */
    signal( SIGABRT, sig_exit );
    signal( SIGINT,  sig_exit );
    signal( SIGTERM, sig_exit );

    /* Please keep these alphabetized. */
    signal( SIGALRM, sig_ignore );
    signal( SIGCHLD, sig_ignore );
    signal( SIGPIPE, sig_ignore );
    signal( SIGUSR1, sig_ignore );
    signal( SIGUSR2, sig_ignore );
}
