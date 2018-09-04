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
#ifndef __P2FS_CONFIG_H__
#define __P2FS_CONFIG_H__

#include <stdbool.h>
#include <stdlib.h>

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct {
    bool     read_notify;
    long int max_transfer;
    int      receive_timeout;
} cfg_t;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/


/**
 *  Reads the specified config file & builds the configuration to use.
 *
 *  @note The configuration MUST be destroyed with config_free() or leaks
 *        may happen.
 *
 *  @param file the file to read/parse for configuration
 *
 *  @return The configuration on success, NULL otherwise.
 */
cfg_t* config_get( const char *file );


/**
 *  Frees the resources associated with cfg_t*.
 *
 *  @param c the cfg_t to free
 */
void config_free( cfg_t *c );

#endif
