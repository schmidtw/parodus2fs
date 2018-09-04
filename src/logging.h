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
#ifndef __P2FS_LOG_H__
#define __P2FS_LOG_H__

#include <stdarg.h>
#include <cimplog/cimplog.h>

extern const char *__program_name;

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define d_error(...)      cimplog_error( __program_name, __VA_ARGS__ )
#define d_info(...)       cimplog_info(  __program_name, __VA_ARGS__ )
#define d_print(...)      cimplog_debug( __program_name, __VA_ARGS__ )

#endif
