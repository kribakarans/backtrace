#include <stdio.h>

int test_fun_5();

int test_fun_4()
{
	printf("Inside %s: %p\n", __func__, test_fun_4);

	test_fun_5();

	return 0;
}

int test_fun_3()
{
	printf("Inside %s: %p\n", __func__, test_fun_3);

	test_fun_4();

	return 0;
}

int test_fun_2()
{
	printf("Inside %s: %p\n", __func__, test_fun_2);

	test_fun_3();

	return 0;
}

int test_fun_1()
{
	printf("Inside %s: %p\n", __func__, test_fun_1);

	test_fun_2();

	return 0;
}

int main()
{
	printf("Inside %s: %p\n", __func__, main);

	test_fun_1();

	return 0;
}

/* EOF */
