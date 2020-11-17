#include <CUnit/CUnit.h>
#include <CUnit/Console.h>
#include <stdlib.h>
#include <stdio.h>

#include "token-list.h"
#include "pretty_printer.h"

void test_sample_29p(void);

int main(){
    CU_pSuite testSuite;

    /* CUnit初期化 */
    CU_initialize_registry();
    /* Suite登録 */
    testSuite = CU_add_suite("kadai2", NULL, NULL);
    CU_add_test(testSuite, "Test sample29p", test_sample_29p);
    /* CUnit 実行 */
    CU_console_run_tests();
    CU_cleanup_registry();
    return 0;
}

void test_sample_29p(void){
    FILE *fp;
    char *file = "Inputs/2/sample29p.mpl";
    int res = init_scan(file, &fp);

    res = Parse_program(fp);
    CU_ASSERT(res != 0);

    end_scan(fp);
}