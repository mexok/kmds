#include <assert.h>
#include <km_list.h>

static void test_list_init()
{
        kmint_l list;
        kmint_l_init(&list);
        kmint_l_push(&list, 3);
        assert(list.size == 1);
        kmint_l_deinit(&list);
}

static void test_list_initd()
{
        kmint_l list;
        kmint_l_initd(&list, 2, 1, 2);
        assert(list.size == 2);
        assert(list.buffer[0] == 1);
        assert(list.buffer[1] == 2);
        kmint_l_deinit(&list);
}

static void test_list_cinit()
{
        kmint_l list, list_copy;
        kmint_l_initd(&list_copy, 2, 1, 2);
        kmint_l_cinit(&list_copy, &list);
        list_copy.buffer[0] = 10;
        assert(list.size == 2);
        assert(list_copy.size == 2);
        assert(list.buffer[0] == 1);
        assert(list.buffer[1] == 2);
        assert(list_copy.buffer[0] == 10);
        assert(list_copy.buffer[1] == 2);
        kmint_l_deinit(&list);
        kmint_l_deinit(&list_copy);
}

static void test_list_double_buffer()
{
        kmint_l list;
        kmint_l_init(&list);
        list.offset = 3;
        size_t items_to_push = list.bufferlen + 1;
        for (int i = 0; i < (int) items_to_push; ++i)
                kmint_l_push(&list, i);
        assert(list.size == items_to_push);
        assert(list.buffer[0] == 0);
        assert(list.buffer[items_to_push - 1] == (int) items_to_push - 1);
        kmint_l_deinit(&list);
}

static void test_list_set()
{
        int buffer[8];
        kmint_l list = (kmint_l) {
                .size = 3,
                .offset=6,
                .bufferlen = 8,
                .buffer = buffer
        };
        buffer[0] = 4;
        buffer[6] = 2;
        buffer[7] = 3;
        kmint_l_set(&list, 0, 20);
        kmint_l_set(&list, 2, 40);
        assert(buffer[0] == 40);
        assert(buffer[6] == 20);
        assert(buffer[7] == 3);
}

static void test_list_push()
{
        kmint_l list;
        kmint_l_init(&list);
        list.offset = list.bufferlen - 1;
        kmint_l_push(&list, 1);
        kmint_l_push(&list, 2);
        kmint_l_push(&list, 3);
        assert(list.size == 3);
        assert(list.buffer[0] == 2);
        assert(list.buffer[1] == 3);
        assert(list.buffer[list.bufferlen - 1] == 1);
        kmint_l_deinit(&list);
}

static void test_list_pop()
{
        kmint_l list;
        kmint_l_initd(&list, 3, 1, 2, 3);
        assert(kmint_l_pop(&list) == 3);
        assert(kmint_l_pop(&list) == 2);
        assert(kmint_l_pop(&list) == 1);
        kmint_l_deinit(&list);
}

static void test_list_shift()
{
        kmint_l list;
        kmint_l_initd(&list, 3, 1, 2, 3);
        assert(kmint_l_shift(&list) == 1);
        assert(kmint_l_shift(&list) == 2);
        assert(kmint_l_shift(&list) == 3);
        kmint_l_deinit(&list);
}

static void test_list_unshift()
{
        kmint_l list;
        kmint_l_init(&list);
        list.offset = 1;
        kmint_l_unshift(&list, 1);
        kmint_l_unshift(&list, 2);
        kmint_l_unshift(&list, 3);
        assert(list.size == 3);
        assert(list.buffer[0] == 1);
        assert(list.buffer[list.bufferlen - 2] == 3);
        assert(list.buffer[list.bufferlen - 1] == 2);
        kmint_l_deinit(&list);
}

static void test_list_clear()
{
        kmint_l list;
        kmint_l_initd(&list, 2, 10, 20);
        kmint_l_clear(&list);
        assert(list.size == 0);
        kmint_l_deinit(&list);
}

void test_list()
{
        test_list_init();
        test_list_initd();
        test_list_cinit();
        test_list_double_buffer();
        test_list_set();
        test_list_push();
        test_list_pop();
        test_list_shift();
        test_list_unshift();
        test_list_clear();
}
