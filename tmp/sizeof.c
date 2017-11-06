#include <stdio.h>

union {
    char a;
    char b;
} foo;

union {
    char a;
    int b;
} bar;

int main(int argc, char const* argv[])
{
    printf("%lu\n", sizeof(char)); // 1
    printf("%lu\n", sizeof(foo));  // 1
    printf("%lu\n", sizeof(bar));  // 4

    char arr[1024];
    printf("%lu\n", sizeof arr);  // 1024
    printf("%lu\n", sizeof &arr); // 8
    char* ptr;
    printf("%lu\n", sizeof ptr); // 8
    char* arr_of_ptr[1024];
    printf("%lu\n", sizeof arr_of_ptr); // 8192
    char (*ptr_of_arr)[1024];
    printf("%lu\n", sizeof ptr_of_arr); // 8
    int iarr[1024];
    printf("%lu\n", sizeof iarr);  // 4096
    return 0;
}
