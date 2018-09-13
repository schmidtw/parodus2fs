/**
 *  Copyright 2018 Weston Schmidt
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <stdbool.h>

#include "../src/config.h"

void test_config( void )
{
    struct test_vectors {
        cfg_t goal;
        const char *file;
    } tests[] = {
        {   .goal.read_notify = false,
            .goal.max_transfer = 10000000,
            .goal.receive_timeout = 10000,
            .file = NULL },
        {   .goal.read_notify = false,
            .goal.max_transfer = 10000000,
            .goal.receive_timeout = 10000,
            .file = "../../tests/mostly-empty.json" },
        {   .goal.read_notify = false,
            .goal.max_transfer = 10000000,
            .goal.receive_timeout = 10000,
            .file = "../../tests/incorrect.json" },
        {   .goal.read_notify = false,
            .goal.max_transfer = 10000000,
            .goal.receive_timeout = 10000,
            .file = "../../tests/no-file.json" },
        {   .goal.read_notify = true,
            .goal.max_transfer = 10000000,
            .goal.receive_timeout = 10000,
            .file = "../../tests/partial.json" },
        {   .goal.read_notify = true,
            .goal.max_transfer = 1000,
            .goal.receive_timeout = 12000,
            .file = "../../tests/normal.json" }
    };
    size_t i;

    for( i = 0; i < sizeof(tests) / sizeof(struct test_vectors); i++ ) {
        cfg_t *got = NULL;

        got = config_get( tests[i].file );

        CU_ASSERT( NULL != got );
        CU_ASSERT( tests[i].goal.read_notify == got->read_notify )
        CU_ASSERT( tests[i].goal.max_transfer == got->max_transfer )
        CU_ASSERT( tests[i].goal.receive_timeout == got->receive_timeout )

        if( NULL != got ) {
            config_free( got );
        }
    }

}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "fs ", NULL, NULL );
    CU_add_test( *suite, "Test configurations", test_config );
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( void )
{
    unsigned rv = 1;
    CU_pSuite suite = NULL;

    if( CUE_SUCCESS == CU_initialize_registry() ) {
        add_suites( &suite );

        if( NULL != suite ) {
            CU_basic_set_mode( CU_BRM_VERBOSE );
            CU_basic_run_tests();
            printf( "\n" );
            CU_basic_show_failures( CU_get_failure_list() );
            printf( "\n\n" );
            rv = CU_get_number_of_tests_failed();
        }

        CU_cleanup_registry();
    }

    if( 0 != rv ) {
        return 1;
    }
    return 0;
}
