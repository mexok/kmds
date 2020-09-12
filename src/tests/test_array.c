#include <assert.h>
#include <km_array.h>

static void test_array_init()
{
        kmint_a a;
        kmint_a_init(&a, 20);
        a.values[1] = 3;
        assert(a.values[0] == 0);
        assert(a.values[1] == 3);
        kmint_a_deinit(&a);
}

static void test_array_initd()
{
        kmint_a a;
        kmint_a_initd(&a, 3, 5, 6, 7);
        assert(a.values[0] == 5);
        assert(a.values[1] == 6);
        assert(a.values[2] == 7);
        kmint_a_deinit(&a);
}

static void test_array_cinit()
{
        kmint_a a, a_copy;
        kmint_a_initd(&a, 2, 5, 6);
        kmint_a_cinit(&a_copy, &a);
        a_copy.values[0] = 0;
        assert(a.values[0] == 5);
        assert(a.values[1] == 6);
        assert(a_copy.values[0] == 0);
        assert(a_copy.values[1] == 6);
        kmint_a_deinit(&a);
        kmint_a_deinit(&a_copy);
}

void test_array()
{
        test_array_init();
        test_array_initd();
        test_array_cinit();
}
