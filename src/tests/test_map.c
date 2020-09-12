#include <assert.h>
#include <km_map.h>


static void test_fna_v1_hash32()
{
        assert(km_fnv_1a_hash32("hello") == 0x4f9f2cab);
        assert(km_fnv_1a_hash32("world") == 0x37a3e893);
}

static void test_map_init()
{
        kmint_m map;
        kmint_m_init(&map);
        kmint_m_set(&map, "key1", 1);
        kmint_m_set(&map, "key2", 3);

        int val = 0;
        assert(kmint_m_get(&map, "key1", &val));
        assert(val == 1);
        assert(kmint_m_get(&map, "key2", &val));
        assert(val == 3);
        assert(kmint_m_get(&map, "key3", NULL) == false);
        assert(val == 3);
        assert(map.size == 2);
        kmint_m_deinit(&map);
}

static void test_map_initd()
{
        kmint_m map;
        kmint_m_initd(
                &map, 3,
                "key 1", 1,
                "key 2", 2,
                "key 3", 3
        );
        int val = 0;
        assert(kmint_m_get(&map, "key 1", &val));
        assert(val == 1);
        assert(map.size == 3);
        kmint_m_deinit(&map);
}

void test_double_buffer()
{
        kmint_m map;
        kmint_m_initd(
                &map, 3,
                "key 1", 1,
                "key 2", 2,
                "key 3", 3
        );
        kmint_m_double_buffer(&map);
        int val = 0;
        assert(kmint_m_get(&map, "key 1", &val));
        assert(val == 1);
        assert(map.size == 3);
        kmint_m_deinit(&map);
}

void test_remove()
{
        kmint_m map;
        kmint_m_initd(
                &map, 2,
                "key 1", 1,
                "key 2", 2
        );
        int value = 0;
        assert(kmint_m_rm(&map, "key 1", &value));
        assert(value == 1);
        assert(kmint_m_rm(&map, "key 1", NULL) == false);
        assert(map.first == map.last);
        assert(kmint_m_rm(&map, "key 2", NULL));
        assert(map.first == NULL);
        assert(map.last == NULL);
        kmint_m_deinit(&map);
}

void test_set_twice()
{
        kmint_m map;
        kmint_m_init(&map);
        kmint_m_set(&map, "key1", 1);
        kmint_m_set(&map, "key1", 3);
        int value = 3;
        assert(kmint_m_get(&map, "key1", &value));
        assert(value == 3);
        assert(map.size == 1);
        assert(kmint_m_rm(&map, "key1", NULL));
        assert(kmint_m_get(&map, "key1", NULL) == false);
        kmint_m_deinit(&map);
}

void test_insertion_order_asc()
{
        kmint_m map;
        kmint_m_initd(
                &map, 3,
                "key 1", 1,
                "key 3", 3,
                "key 2", 2
        );
        kmint_e *e = map.first;
        assert(e->value == 1);
        e = e->next;
        assert(e->value == 3);
        e = e->next;
        assert(e->value == 2);
        kmint_m_deinit(&map);
}

void test_insertion_order_desc()
{
        kmint_m map;
        kmint_m_initd(
                &map, 3,
                "key 1", 1,
                "key 3", 3,
                "key 2", 2
        );
        kmint_e *e = map.last;
        assert(e->value == 2);
        e = e->previous;
        assert(e->value == 3);
        e = e->previous;
        assert(e->value == 1);
        kmint_m_deinit(&map);
}

void test_hash_collision()
{
        kmint_m map;
        kmint_m_init(&map);

        char *key1 = "key0";
        char *key2 = "key26";
        char *key3 = "key31";
        assert(map.bucketlen == 16);  // if this is not the case we have to adjust the keys
        assert(km_fnv_1a_hash32(key1) % 16 == 4);
        assert(km_fnv_1a_hash32(key2) % 16 == 4);
        assert(km_fnv_1a_hash32(key3) % 16 == 4);

        kmint_m_set(&map, key1, 1);
        kmint_m_set(&map, key2, 2);
        kmint_m_set(&map, key3, 3);
        int value = 0;
        assert(kmint_m_rm(&map, key2, &value));
        assert(value == 2);
        assert(kmint_m_rm(&map, key1, &value));
        assert(value == 1);
        assert(kmint_m_rm(&map, key3, &value));
        assert(value == 3);
        assert(map.size == 0);
        kmint_m_deinit(&map);
}

void test_map()
{
        test_fna_v1_hash32();
        test_map_init();
        test_map_initd();
        test_double_buffer();
        test_remove();
        test_set_twice();
        test_insertion_order_asc();
        test_insertion_order_desc();
        test_hash_collision();
}
