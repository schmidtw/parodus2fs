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
static void __handle_full_tree( cfg_t *cfg, struct wrp_crud_msg *r );
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

    /* Only support getting things for now. */
    if( WRP_MSG_TYPE__RETREIVE != in->msg_type ) {
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

    if( NULL == endpoint ) {
        __handle_full_tree( cfg, crud );

        d_info( "Request: CONFIG, Status: %d, Payload Size: %d\n",
                crud->status, crud->payload_size );
    } else if( 0 == strncmp("full/", endpoint, (sizeof "full/"-1)) ) {
        /* Only return a full file (if it fits). */
        char *abs_filename = &endpoint[(sizeof "full" - 1)];

        __read_file( cfg, crud, abs_filename, SEND_ALL_ONLY );

        d_info( "Request: FULL, Status: %d, Payload Size: %d, File: %s\n",
                crud->status, crud->payload_size, abs_filename );
    } else if( 0 == strncmp("head/", endpoint, (sizeof "head/"-1)) ) {
        /* Return the head of the file. */
        char *abs_filename = &endpoint[(sizeof "head" - 1)];

        __read_file( cfg, crud, abs_filename, SEND_FROM_HEAD );

        d_info( "Request: HEAD, Status: %d, Payload Size: %d, File: %s\n",
                crud->status, crud->payload_size, abs_filename );
    } else if( 0 == strncmp("tail/", endpoint, (sizeof "tail/"-1)) ) {
        /* Return the tail of the file. */
        char *abs_filename = &endpoint[(sizeof "tail" - 1)];

        __read_file( cfg, crud, abs_filename, SEND_FROM_TAIL );

        d_info( "Request: TAIL, Status: %d, Payload Size: %d, File: %s\n",
                crud->status, crud->payload_size, abs_filename );
    } else if( 0 == strncmp("ls/", endpoint, (sizeof "ls/"-1)) ) {
        /* Return the directory listing for the endpoint specified. */
        char *abs_dirname = &endpoint[(sizeof "ls" - 1)];

        __read_dir( cfg, crud, abs_dirname );

        d_info( "Request: LS, Status: %d, Payload Size: %d, Dir: %s\n",
                crud->status, crud->payload_size, abs_dirname );
    } else {
        d_info( "Request: Unknown, Status: %d, Dest: %s\n",
                crud->status, in->u.crud.dest );
    }

    libparodus_send( parodus, &resp );

    /* Only the payload is allocated, free it, but everything else is a
     * constant, so don't free it. */
    if( NULL != crud->payload ) {
        free( crud->payload );
    }
}


/**
 *  This sends back all the details about how to use this API.
 *
 *  Possible Status Codes:
 *      200 - Success
 *      500 - Memory allocation error
 *
 *  @param cfg   the configuration to apply
 *  @param r     the response message to populate
 */
static void __handle_full_tree( cfg_t *cfg, struct wrp_crud_msg *r )
{
    static const char c[] = "{ "
                            "\"config\": { "
                                "\"read-notify\": %s, "
                                "\"max-bytes-to-transfer\": %ld, "
                                "\"receive-timeout\": %d "
                            "}, "
                            "\"head\": { "
                                "\"desc\": \"Transfer up to the max-bytes-to-transfer "
                                           "maximum starting with the head and "
                                           "truncating the tail, if needed.  "
                                           "A binary copy of the file is returned.\""
                            "}, "
                            "\"tail\": { "
                                "\"desc\": \"Transfer up to the max-bytes-to-transfer "
                                           "maximum truncating the head of the file "
                                           "to preserve the tail, if needed.  "
                                           "A binary copy of the file is returned.\""
                            "}, "
                            "\"full\": { "
                                "\"desc\": \"Only transfer the file if the entire "
                                           "file can be transfered without "
                                           "truncation.  A binary copy of the "
                                           "file is returned.\""
                            "}, "
                            "\"ls\": { "
                                "\"desc\": \"Transfer the directory listing as a "
                                           "JSON list in random order.  If the list "
                                           "is truncated then the \\\"partial\\\" "
                                           "boolean parameter (omitted unless "
                                           "needed) shall be set to true.  "
                                           "The list is returned with the name "
                                           "\\\"list\\\".  "
                                           "The max-bytes-to-transfer parameter "
                                           "governs the truncation size.\""
                            "} "
                        "}";

    /* Build/ship the payload. */
    r->payload_size = snprintf( NULL, 0, c, (cfg->read_notify ? "true" : "false"),
                                cfg->max_transfer, cfg->receive_timeout );
    r->payload_size++;  /* For the trailing '\0'. */
                                                                  
    r->payload = malloc( r->payload_size );
    if( NULL == r->payload ) {
        r->status = 500;
        r->payload_size = 0;
    } else {
        r->status = 200;
        r->content_type = "application/json";
        sprintf( r->payload, c, (cfg->read_notify ? "true" : "false"),
                 cfg->max_transfer, cfg->receive_timeout );
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
