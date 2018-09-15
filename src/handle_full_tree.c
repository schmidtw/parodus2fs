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

#include <stdio.h>
#include <string.h>

#include <wrp-c.h>

#include "handle_full_tree.h"

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

/* See handle_full_tree.h for details. */
void handle_full_tree( cfg_t *cfg, struct wrp_crud_msg *r )
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
    size_t len;

    /* Default response. */
    r->status = 500;
    r->payload_size = 0;

    /* Build/ship the payload. */
    len = snprintf( NULL, 0, c, (cfg->read_notify ? "true" : "false"),
                    cfg->max_transfer, cfg->receive_timeout );
    len++;  /* For the trailing '\0'. */
                                                                  
    r->payload = malloc( len );
    if( NULL != r->payload ) {
        r->payload_size = len;
        r->status = 200;
        r->content_type = "application/json";
        sprintf( r->payload, c, (cfg->read_notify ? "true" : "false"),
                 cfg->max_transfer, cfg->receive_timeout );
    }
}


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/* none */
