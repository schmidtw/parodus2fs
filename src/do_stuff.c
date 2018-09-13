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

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cJSON.h>
#include <wrp-c.h>

#include "handle_full_tree.h"
#include "do_stuff.h"
#include "logging.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef enum {
    SEND_FROM_HEAD,
    SEND_ALL_ONLY,
    SEND_FROM_TAIL
} do_stuff_mode_t;

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static void __process_msg( libpd_instance_t parodus, cfg_t *cfg, wrp_msg_t *in );
static void __process_retrieve( cfg_t *cfg, wrp_msg_t *in,
                                struct wrp_crud_msg *out,
                                const char *endpoint );
static void __process_delete( cfg_t *cfg, wrp_msg_t *in,
                              struct wrp_crud_msg *out,
                              const char *endpoint );
static void __read_file( cfg_t *cfg, struct wrp_crud_msg *r,
                         const char *fname, do_stuff_mode_t mode );
static void __read_dir( cfg_t *cfg, struct wrp_crud_msg *r, const char *dname );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See do_stuff.h for details. */
int runloop( libpd_instance_t parodus, cfg_t *cfg )
{
    int done = 0;
    int rv = -1;

    while( 0 == done ) {
        wrp_msg_t *in;
        int err;

        err = libparodus_receive( parodus, &in, cfg->receive_timeout );

        switch( err ) {
            case 0:     /* Message received. */
                __process_msg( parodus, cfg, in );
                wrp_free_struct( in );
                break;
                
            case 1:     /* Timed out, just loop. */
                break;

            case 2:     /* Closed connection, exit. */
                done = 1;
                rv = 0;
                break;

            default:    /* Something else, exit. */
                done = 1;
                rv = err;
                break;
        }
    }

    return rv;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  This function makes sure it knows the request message type & sets up
 *  a default response of 400 to default to a "I don't know" response.
 *
 *  @param parodus the instance of libparodus to send the response to
 *  @param cfg     the configuration to apply
 *  @param in      the WRP message we got from Parodus to process
 */
static void __process_msg( libpd_instance_t parodus, cfg_t *cfg, wrp_msg_t *in )
{
    char *endpoint = NULL;
    wrp_msg_t resp;
    struct wrp_crud_msg *crud = &(resp.u.crud);

    if( (NULL == cfg) || (in == NULL) ) {
        return;
    }

    /* Ensure everything is 0/NULL. */
    memset( &resp, 0, sizeof(wrp_msg_t) );

    /* All responses basically look like this. */
    resp.msg_type          = in->msg_type;
    crud->status           = 400;
    crud->transaction_uuid = in->u.crud.transaction_uuid;
    crud->dest             = in->u.crud.source;
    crud->source           = in->u.crud.dest;
    crud->path             = in->u.crud.path;
    
    // printf( "Dest: %s\n", in->u.crud.dest );

    endpoint = wrp_get_msg_element( WRP_ID_ELEMENT__APPLICATION, in, DEST );

    switch( in->msg_type ) {
        case WRP_MSG_TYPE__CREATE:
            // TODO: __process_create( cfg, in );
            break;
        case WRP_MSG_TYPE__RETREIVE:
            __process_retrieve( cfg, in, crud, endpoint );
            break;
        case WRP_MSG_TYPE__UPDATE:
            // TODO: __process_update( cfg, in );
            break;
        case WRP_MSG_TYPE__DELETE:
            __process_delete( cfg, in, crud, endpoint );
            break;

        default:
            break;
    }

    libparodus_send( parodus, &resp );

    /* Only the payload is allocated, free it, but everything else is a
     * constant, so don't free it. */
    if( NULL != crud->payload ) {
        free( crud->payload );
    }
}

/**
 *  This function makes sure it knows the request message type & sets up
 *  a default response of 400 to default to a "I don't know" response.
 *
 *  @param cfg      the configuration to apply
 *  @param in       the WRP message we got from Parodus to process
 *  @param out      the WRP message we will send
 *  @param endpoint the endpoint data string
 */
static void __process_retrieve( cfg_t *cfg, wrp_msg_t *in,
                                struct wrp_crud_msg *out,
                                const char *endpoint )
{
    if( NULL == endpoint ) {
        handle_full_tree( cfg, out );

        d_info( "Request: Retrieve - CONFIG, Status: %d, Payload Size: %d\n",
                out->status, out->payload_size );
    } else if( 0 == strncmp("full/", endpoint, (sizeof "full/"-1)) ) {
        /* Only return a full file (if it fits). */
        const char *abs_filename = &endpoint[(sizeof "full" - 1)];

        __read_file( cfg, out, abs_filename, SEND_ALL_ONLY );

        d_info( "Request: Retrieve - FULL, Status: %d, Payload Size: %d, File: %s\n",
                out->status, out->payload_size, abs_filename );
    } else if( 0 == strncmp("head/", endpoint, (sizeof "head/"-1)) ) {
        /* Return the head of the file. */
        const char *abs_filename = &endpoint[(sizeof "head" - 1)];

        __read_file( cfg, out, abs_filename, SEND_FROM_HEAD );

        d_info( "Request: Retrieve - HEAD, Status: %d, Payload Size: %d, File: %s\n",
                out->status, out->payload_size, abs_filename );
    } else if( 0 == strncmp("tail/", endpoint, (sizeof "tail/"-1)) ) {
        /* Return the tail of the file. */
        const char *abs_filename = &endpoint[(sizeof "tail" - 1)];

        __read_file( cfg, out, abs_filename, SEND_FROM_TAIL );

        d_info( "Request: Retrieve - TAIL, Status: %d, Payload Size: %d, File: %s\n",
                out->status, out->payload_size, abs_filename );
    } else if( 0 == strncmp("ls/", endpoint, (sizeof "ls/"-1)) ) {
        /* Return the directory listing for the endpoint specified. */
        const char *abs_dirname = &endpoint[(sizeof "ls" - 1)];

        __read_dir( cfg, out, abs_dirname );

        d_info( "Request: Retrieve - LS, Status: %d, Payload Size: %d, Dir: %s\n",
                out->status, out->payload_size, abs_dirname );
    } else {
        d_info( "Request: Retrieve - Unknown, Status: %d, Dest: %s\n",
                out->status, in->u.crud.dest );
    }
}

/**
 *  Based on the configuration parameters, this deletes a file if possible.
 *
 *  Possible Status Codes:
 *      200 - Success
 *      403 - Program user does not have permissions to read the file from fs
 *      404 - No file found
 *      500 - Operation failed
 *
 *  @param cfg      the configuration to apply
 *  @param in       the WRP message we got from Parodus to process
 *  @param out      the WRP message we will send
 *  @param endpoint the endpoint data string
 */
static void __process_delete( cfg_t *cfg, wrp_msg_t *in,
                              struct wrp_crud_msg *out,
                              const char *endpoint )
{
    (void) cfg;

    if( 0 == strncmp("full/", endpoint, (sizeof "full/"-1)) ) {
        /* Only return a full file (if it fits). */
        const char *fname = &endpoint[(sizeof "full" - 1)];
        struct stat s;

        if( (0 == stat(fname, &s)) && (S_ISREG(s.st_mode)) ) {
            /* Check if we can open it... */
            if( 0 == access(fname, R_OK) ) {
                if( 0 == remove(fname) ) {
                    out->status = 200;
                } else {
                    /* Operation failed. */
                    out->status = 500;
                }
            } else {
                /* Permissions block. */
                out->status = 403;
            }
        } else {
            /* No file. */
            out->status = 404;
        }

        d_info( "Request: Delete - FULL, Status: %d, File: %s\n",
                out->status, fname );
    } else {
        d_info( "Request: Delete - Unknown, Status: %d, Dest: %s\n",
                out->status, in->u.crud.dest );
    }
}


/**
 *  Based on the configuration parameters, this reads the file & sends it
 *  back if possible.
 *
 *  Possible Status Codes:
 *      200 - Success
 *      403 - Program user does not have permissions to read the file from fs
 *      404 - No file found
 *      413 - File is too large to fit & entire file requested
 *      500 - Memory allocation error
 *
 *  @param cfg   the configuration to apply
 *  @param r     the response message to populate
 *  @param fname the name of the file to read
 *  @param mode  the request mode
 */
static void __read_file( cfg_t *cfg, struct wrp_crud_msg *r,
                         const char *fname, do_stuff_mode_t mode )
{
    struct stat s;

    /* Check if the target is a normal file. */
    if( (0 == stat(fname, &s)) && (S_ISREG(s.st_mode)) ) {
        /* Check if we can open it... */
        if( 0 == access(fname, R_OK) ) {
            /* Get the file size... */
            FILE *f;
            long int len;

            f = fopen( fname, "r" );
            if( NULL == f ) { return; } /* Not sure how this could happen... */

            fseek( f, 0L, SEEK_END );
            len = ftell( f );

            if( (SEND_ALL_ONLY == mode) && (len > cfg->max_transfer) ) {
                /* File is too big, bail. */
                r->status = 413;
            } else {
                /* Set the payload size & allocate it's buffer */
                r->payload_size = cfg->max_transfer;
                if( len < cfg->max_transfer ) {
                    r->payload_size = len;
                }

                r->payload = malloc( r->payload_size );
                if( NULL == r->payload ) {
                    r->status = 500;
                    r->payload_size = 0;
                } else {
                    size_t got;

                    /* See to the point where we will read/fill our buffer. */
                    if( (len > cfg->max_transfer) && (SEND_FROM_TAIL == mode) ) {
                        fseek( f, (len - cfg->max_transfer), SEEK_SET );
                    } else {
                        fseek( f, 0, SEEK_SET );
                    }

                    got = fread( r->payload, r->payload_size, 1, f );
                    if( r->payload_size != (long int) got ) {
                        /* Perfect, we're done! */
                        r->status = 200;
                        r->content_type = "binary/octet-stream";
                    } else {
                        /* Something bad happened. Bail. */
                        r->status = 500;
                        r->payload_size = 0;
                        free( r->payload );
                        r->payload = NULL;
                    }
                }
            }
            fclose( f );
        } else {
            /* Permissions block. */
            r->status = 403;
        }
    } else {
        /* No file. */
        r->status = 404;
    }
}


/**
 *  Based on the configuration parameters, this reads a directory listing and
 *  returns is if possible.
 *
 *  @note Limited to the max_transfer size.
 *
 *  Possible Status Codes:
 *      200 - Success
 *      403 - Program user does not have permissions to read the file from fs
 *      404 - No dir found
 *      500 - Memory allocation error
 *
 *  @param cfg   the configuration to apply
 *  @param r     the response message to populate
 *  @param dname the name of the dir to read
 */
static void __read_dir( cfg_t *cfg, struct wrp_crud_msg *r, const char *dname )
{
    struct stat s;

    /* Check if the target is a directory. */
    if( (0 == stat(dname, &s)) && (S_ISDIR(s.st_mode)) ) {
        /* Check if we can open it... */
        if( 0 == access(dname, R_OK) ) {
            DIR *d;
            struct dirent *ep;

            d = opendir( dname );
            if( NULL != d ) {
                cJSON *doc = cJSON_CreateObject();

                if( NULL == doc ) {
                    /* Something bad happened. Bail. */
                    r->status = 500;
                } else {
                    cJSON *list = cJSON_CreateArray();
                    if( NULL == list ) {
                        /* Something bad happened. Bail. */
                        r->status = 500;
                    } else {
                        cJSON_AddItemToObject( doc, "list", list );

                        while( NULL != (ep = readdir(d)) ) {
                            cJSON *entry = cJSON_CreateString( ep->d_name );
                            if( NULL == entry ) {
                                /* Something bad happened. Bail. */
                                r->status = 500;
                                break;
                            } else {
                                cJSON_AddItemToArray( list, entry );
                            }
                        }

                        /* Success! */
                        r->status = 200;
                        r->content_type = "application/json";

                        r->payload = cJSON_Print( doc );
                        r->payload_size = strlen( (char*) r->payload );

                        /* Limit our transfer size by throwing away entryes
                         * at "random" aka the 0th element until we fit.
                         * Then, ship it! */
                        if( r->payload_size > cfg->max_transfer ) {
                            cJSON_AddBoolToObject( doc, "partial", cJSON_True );
                            while( r->payload_size > cfg->max_transfer) {
                                free( r->payload );
                                cJSON_DeleteItemFromArray( list, 0 );
                                r->payload = cJSON_Print( doc );
                                r->payload_size = strlen( (char*) r->payload );
                            }
                        }
                    }
                    cJSON_Delete( doc );
                }
                closedir( d );
            } else {
                /* Something bad happened. Bail. */
                r->status = 500;
            }
        } else {
            /* Permissions block. */
            r->status = 403;
        }
    } else {
        /* No directory or not a directory. */
        r->status = 404;
    }
}
