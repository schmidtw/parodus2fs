#   Copyright 2018 Weston Schmidt
#   Copyright 2018 Comcast Cable Communications Management, LLC
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

set(PROJ_P2FS parodus2fs)

set(SOURCES 
    args.c
    config.c
    handle_full_tree.c
    do_stuff.c
    sigs.c
    main.c)

add_executable(${PROJ_P2FS} ${SOURCES})

target_link_libraries (${PROJ_P2FS}
                       ${CMAKE_THREAD_LIBS_INIT}
                       -lwrp-c
                       -lmsgpackc
                       -lm
                       -lcimplog
                       -llibparodus
                       -lcjson
                       -lcjson_extras
                       -lnanomsg
                       -ltrower-base64
                       -lpthread
                       -lrt
                       )
    
install (TARGETS ${PROJ_P2FS} DESTINATION bin)
