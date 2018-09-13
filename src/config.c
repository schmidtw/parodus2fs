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

#include <stdlib.h>

#include <cjson_extras/cjson-extras.h>

#include "config.h"

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

/* See config.h for details. */
cfg_t* config_get( const char *file )
{
    cfg_t *c;

    c = (cfg_t*) malloc( sizeof(cfg_t) );
    if( NULL != c ) {
        c->read_notify  = false;
        c->max_transfer = 10000000;
        c->receive_timeout = 10000;
    }

    if( NULL != file ) {
        cJSON *json;
        int err;

        json = cJSON_Parse_File( file, &err );
        if( NULL != json ) {
            cJSON *item;
            item = cJSON_GetObjectItemCaseSensitive( json, "read-notify" );
            if( cJSON_IsBool(item) ) {
                c->read_notify = (cJSON_True == item->type) ? true : false;
            }
            item = cJSON_GetObjectItemCaseSensitive( json, "max-bytes-to-transfer" );
            if( cJSON_IsNumber(item) ) {
                c->max_transfer = item->valueint;
            }
            item = cJSON_GetObjectItemCaseSensitive( json, "receive-timeout" );
            if( cJSON_IsNumber(item) ) {
                c->receive_timeout = item->valueint;
            }
            cJSON_Delete( json );
        }
    }
    return c;
}

/* See config.h for details. */
void config_free( cfg_t *c )
{
    if( NULL != c ) {
        free( c );
    }
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/* none */
