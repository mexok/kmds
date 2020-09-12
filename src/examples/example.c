#include <stdio.h>
#include <kmds.h>

void print_list(const kmint_l *list)
{
        if (list->size == 0) {
                printf("List empty\n");
                return;
        }
        printf("List elements:\n");
        for (size_t i = 0; i < list->size; ++i) {
                printf("Index %ld: value %d\n", i, kmint_l_at(list, i));
        }
}

void print_map(const kmint_m *map)
{
        if (map->size == 0) {
                printf("Map empty\n");
                return;
        }
        printf("Map elements:\n");
        // map elements are insertion ordered
        for (kmint_e *it = map->first; it; it = it->next) {
                printf("Key '%s': value %d\n", it->key, it->value);
        }
}

int main()
{
        for (size_t i = 0; i < 100; ++i) {
                char key[20];
                sprintf(key, "key%ld", i);
                uint32_t hash = km_fnv_1a_hash32(key);
                hash = hash % (uint32_t) 16;
                printf("key %s, hash %d\n", key, hash);
        }

        printf("---- Init empty data structures ----\n");
        kmint_l list;
        kmint_l_init(&list);
        print_list(&list);

        kmint_m map;
        kmint_m_init(&map);
        print_map(&map);

        printf("\n---- Adding values to list ----\n");
        kmint_l_push(&list, 1);
        kmint_l_push(&list, 2);
        kmint_l_push(&list, 3);
        print_list(&list);

        printf("\n---- Transforming list to map ----\n");
        for (size_t i = 0; i < list.size; ++i) {
                char key[20];
                sprintf(key, "key %ld", i);
                kmint_m_set(&map, key, i);
        }
        print_map(&map);

        kmint_l_deinit(&list);
        kmint_m_deinit(&map);
        return 0;
}
