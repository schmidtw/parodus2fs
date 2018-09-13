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
#ifndef __P2FS_HANDLE_FULL_TREE_H__
#define __P2FS_HANDLE_FULL_TREE_H__

#include <wrp-c.h>

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
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/

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
void handle_full_tree( cfg_t *cfg, struct wrp_crud_msg *r );

#endif

