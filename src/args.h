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
#ifndef __P2FS_ARGS_H__
#define __P2FS_ARGS_H__

#include <libparodus.h>

/**
 *  Processes the arguments from the CLI.
 *
 *  @param argc     argc from main
 *  @param argv     argv from main
 *  @param cfg      the libpd_cfg_t struct to configure
 *  @param cfg_file the config file name to populate
 */
int process_args( int argc, char **argv, libpd_cfg_t *cfg, char **cfg_file );

/**
 *  Prints the options and how to use them to STDOUT.
 *
 *  @param name the name of the program
 */
void print_usage( const char *name );

#endif
