#   Copyright 2018 Weston Schmidt
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


if(NOT DISABLE_VALGRIND)
set (MEMORY_CHECK valgrind --leak-check=full --show-reachable=yes -v)
endif ()  


set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W  -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -O0")


link_directories ( ${LIBRARY_DIR} )

add_executable(simple simple.c)
target_link_libraries (simple  gcov  cunit
                       ${CMAKE_THREAD_LIBS_INIT}
                       -lwrp-c
                       -lmsgpackc
                       -lm
                       -lcimplog
                       -llibparodus
                       -lnanomsg
                       -ltrower-base64
                       -lpthread
                       -lrt 
                       )
add_test(NAME Simple COMMAND ${MEMORY_CHECK} ${CMAKE_CURRENT_BINARY_DIR}/simple)

add_executable(test_config ../src/config.c test_config.c)
target_link_libraries (test_config  gcov  cunit
                       ${CMAKE_THREAD_LIBS_INIT}
                       -lcimplog
                       -lcjson_extras
                       -lcjson
                       )
add_test(NAME test_config COMMAND ${MEMORY_CHECK} ${CMAKE_CURRENT_BINARY_DIR}/test_config)

add_executable(test_handle_full_tree ../src/handle_full_tree.c test_handle_full_tree.c)
target_link_libraries (test_handle_full_tree  gcov  cunit
                       ${CMAKE_THREAD_LIBS_INIT}
                       )
add_test(NAME test_handle_full_tree COMMAND ${MEMORY_CHECK} ${CMAKE_CURRENT_BINARY_DIR}/test_handle_full_tree)

add_custom_target(coverage
                  COMMAND lcov -q --capture --directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_config.dir/__/src --output-file coverage.info
                  COMMAND genhtml coverage.info
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
