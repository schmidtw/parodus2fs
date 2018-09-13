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

#include "../src/handle_full_tree.h"

void test_handle_full_tree( void )
{
    cfg_t cfg;
    struct wrp_crud_msg crud;

    cfg.read_notify = false;
    cfg.max_transfer = 100;
    cfg.receive_timeout = 1000;

    handle_full_tree( &cfg, &crud );

    CU_ASSERT( 200 == crud.status );
    CU_ASSERT_STRING_EQUAL( "application/json", crud.content_type );
    CU_ASSERT( 0 < crud.payload_size );
    CU_ASSERT( NULL != crud.payload );
    CU_ASSERT( crud.payload_size == 1 + strlen(crud.payload) );

    if( NULL != crud.payload ) {
        free( crud.payload );
    }
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "fs ", NULL, NULL );
    CU_add_test( *suite, "Test handle_full_tree", test_handle_full_tree );
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
