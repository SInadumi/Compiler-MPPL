#include <CUnit/CUnit.h>
#include <CUnit/Console.h>
#include <stdlib.h>
#include <stdio.h>

#include "token-list.h"
#include "pretty_printer.h"

/*
    memo
    compile         gcc test.c scan.c pretty_printer.c -Wall -L/usr/local/lib -lcunit
    カバレッジ測定  gcov pretty_printer.c
*/
void test_sample21(void);
void test_sample22(void);
void test_sample23(void);
void test_sample24(void);
void test_sample26(void);
void test_sample27(void);
void test_sample28p(void);
void test_sample_29p(void);
void test_sample_25t(void);
void test_sample02a(void);
void test_sample02a(void);

void test_sample02a(void);
void test_sample021(void);
void test_sample022(void);
void test_sample023(void);
void test_sample024(void);
void test_sample025(void);
void test_sample026(void);

int error(const char *mes) {
	printf("\n ERROR: line %d %s\n", get_linenum(), mes);
	return ERROR;
}

int main(){
    CU_pSuite testSuite;

    /* CUnit初期化 */
    CU_initialize_registry();
    /* Suite登録 */
    testSuite = CU_add_suite("kadai2", NULL, NULL);
    CU_add_test(testSuite, "Test sample29p", test_sample_29p);
    CU_add_test(testSuite, "Test sample21", test_sample21);
    CU_add_test(testSuite, "Test sample22", test_sample22);
    CU_add_test(testSuite, "Test sample23", test_sample23);
    CU_add_test(testSuite, "Test sample24", test_sample24);
    CU_add_test(testSuite, "Test sample26", test_sample26);
    CU_add_test(testSuite, "Test sample27", test_sample27);
    CU_add_test(testSuite, "Test sample28p", test_sample28p);
    CU_add_test(testSuite, "Test sample25t", test_sample_25t);
    CU_add_test(testSuite, "Test sample02a", test_sample02a);
    CU_add_test(testSuite, "Test sample021", test_sample021);
    CU_add_test(testSuite, "Test sample022", test_sample022);
    CU_add_test(testSuite, "Test sample023", test_sample023);
    CU_add_test(testSuite, "Test sample024", test_sample024);
    CU_add_test(testSuite, "Test sample025", test_sample025);
    CU_add_test(testSuite, "Test sample026", test_sample026);
    /* CUnit 実行 */
    CU_console_run_tests();
    CU_cleanup_registry();
    return 0;
}

void test_sample21(void){
    FILE *fp;
    char *file = "Inputs/2/sample21.mpl";
    int res = init_scan(file, &fp);
    res = Parse_program(fp);
    CU_ASSERT(res != 0);
    end_scan(fp);
}
void test_sample22(void){
    FILE *fp;
    char *file = "Inputs/2/sample22.mpl";
    int res = init_scan(file, &fp);
    res = Parse_program(fp);
    CU_ASSERT(res != 0);
    end_scan(fp);
}
void test_sample23(void){
    FILE *fp;
    char *file = "Inputs/2/sample23.mpl";
    int res = init_scan(file, &fp);
    res = Parse_program(fp);
    CU_ASSERT(res != 0);
    end_scan(fp);
}
void test_sample24(void){
    FILE *fp;
    char *file = "Inputs/2/sample24.mpl";
    int res = init_scan(file, &fp);
    res = Parse_program(fp);
    CU_ASSERT(res != 0);
    end_scan(fp);
}
void test_sample_25t(void){
    FILE *fp;
    char *file = "Inputs/2/sample25t.mpl";
    int res = init_scan(file, &fp);
    res = Parse_program(fp);
    CU_ASSERT(res != 0);
    end_scan(fp);
}
void test_sample26(void){
    FILE *fp;
    char *file = "Inputs/2/sample26.mpl";
    int res = init_scan(file, &fp);
    res = Parse_program(fp);
    CU_ASSERT(res != 0);
    end_scan(fp);
}
void test_sample27(void){
    FILE *fp;
    char *file = "Inputs/2/sample27.mpl";
    int res = init_scan(file, &fp);
    res = Parse_program(fp);
    CU_ASSERT(res != 0);
    end_scan(fp);
}
void test_sample28p(void){
    FILE *fp;
    char *file = "Inputs/2/sample28p.mpl";
    int res = init_scan(file, &fp);
    res = Parse_program(fp);
    CU_ASSERT(res != 0);
    end_scan(fp);
}

void test_sample_29p(void){
    FILE *fp;
    char *file = "Inputs/2/sample29p.mpl";
    int res = init_scan(file, &fp);

    res = Parse_program(fp);
    CU_ASSERT(res != 0);

    end_scan(fp);
}

void test_sample02a(void){
    FILE *fp;
    char *file = "Inputs/2/sample02a.mpl";
    init_scan(file, &fp);

    int res = Parse_program(fp);
    CU_ASSERT(res != NORMAL);

    end_scan(fp);
}
void test_sample021(void){
    FILE *fp;
    char *file = "Inputs/2/sample021.mpl";
    init_scan(file, &fp);

    int res = Parse_program(fp);
    CU_ASSERT(res != NORMAL);

    end_scan(fp);
}
void test_sample022(void){
    FILE *fp;
    char *file = "Inputs/2/sample022.mpl";
    init_scan(file, &fp);

    int res = Parse_program(fp);
    CU_ASSERT(res != NORMAL);

    end_scan(fp);
}
void test_sample023(void){
    FILE *fp;
    char *file = "Inputs/2/sample023.mpl";
    init_scan(file, &fp);

    int res = Parse_program(fp);
    CU_ASSERT(res != NORMAL);

    end_scan(fp);
}
void test_sample024(void){
    FILE *fp;
    char *file = "Inputs/2/sample024.mpl";
    init_scan(file, &fp);

    int res = Parse_program(fp);
    CU_ASSERT(res != NORMAL);

    end_scan(fp);
}
void test_sample025(void){
    FILE *fp;
    char *file = "Inputs/2/sample025.mpl";
    init_scan(file, &fp);

    int res = Parse_program(fp);
    CU_ASSERT(res != NORMAL);

    end_scan(fp);
}
void test_sample026(void){
    FILE *fp;
    char *file = "Inputs/2/sample026.mpl";
    init_scan(file, &fp);

    int res = Parse_program(fp);
    CU_ASSERT(res != NORMAL);

    end_scan(fp);
}