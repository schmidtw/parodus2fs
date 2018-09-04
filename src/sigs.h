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
#ifndef __P2FS_SIGS_H__
#define __P2FS_SIGS_H__

/**
 *  Exit the program when this signal is received.
 *
 *  @param sig  the signal that kills us
 */
void sig_exit( int sig );


/**
 *  Ignore this signal
 *
 *  @param sig  the signal to log and ignore
 */
void sig_ignore( int sig );

/**
 *  Convert the signals to a string if we can.
 *
 *  @note Never NULL.
 *  @note Never free().
 *
 *  @param sig  the signal to conviert
 *
 *  @return The string constant for the signal.
 */
const char * sig_to_string( int sig );

#endif
