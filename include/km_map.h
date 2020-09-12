#ifndef km_map_h
#define km_map_h

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef KM_MALLOC
#define KM_MALLOC malloc
#endif

#ifndef KM_CALLOC
#define KM_CALLOC calloc
#endif

#ifndef KM_REALLOC
#define KM_REALLOC realloc
#endif

#ifndef KM_FREE
#define KM_FREE free
#endif


uint32_t km_fnv_1a_hash32(const char *key)
{
        // we use FNV-1a hash algorithm which is in the public domain
        uint32_t hash = 2166136261;
        const uint32_t fna_prime = 16777619;
        while (*key != '\0') {
                hash ^= (uint32_t) *key++;
                hash *= fna_prime;
        }
        return hash;
}

#define KM_MAP(map, map_element, data_type, va_data_type) \
\
typedef struct map_element map_element; \
typedef struct map map; \
\
struct map_element { \
        /* Points to the next inserted element */ \
        map_element *next; \
        /* Points to the previous inserted element */ \
        map_element *previous; \
        /* A link which is needed to connect elements in a bucket together */ \
        map_element *_bucket_link; \
        data_type value; \
        const char key[]; \
}; \
\
struct map { \
        size_t size; \
        size_t bucketlen; \
        map_element *first; \
        map_element *last; \
        map_element **buckets; \
}; \
\
\
static void map ## _set(map *m, const char * key, data_type value); \
\
static void map ## _init(map *m) \
{ \
        size_t bucketlen = 16; \
        *m = (map){ \
                .bucketlen = bucketlen, \
                .buckets = KM_CALLOC(bucketlen, sizeof(map_element *)) \
        }; \
} \
\
static void map ## _vinitd(map *m, size_t size, va_list values) \
{ \
        map ## _init(m); \
        for (size_t i = 0; i < size; ++i) { \
                const char *key = va_arg(values, const char*); \
                data_type value = (data_type) va_arg(values, va_data_type); \
                map ## _set(m, key, value); \
        } \
} \
\
static void map ## _initd(map *m, size_t size, ...) \
{ \
        va_list values; \
        va_start(values, size); \
        map ## _vinitd(m, size, values); \
        va_end(values); \
} \
\
static void map ## _cinit(map *m, const map *to_copy) \
{ \
        *m = (map){ \
                .bucketlen = to_copy->bucketlen, \
                .buckets = KM_CALLOC(to_copy->bucketlen, sizeof(map_element *)) \
        }; \
        for (const map_element *it = to_copy->first; it; it = it->next) { \
                map ## _set(m, it->key, it->value); \
        } \
} \
\
static void map ## _double_buffer(map *m) \
{ \
        m->bucketlen *= 2; \
        m->buckets = KM_REALLOC(m->buckets, sizeof(map_element *) * m->bucketlen); \
        memset(m->buckets, 0, m->bucketlen); \
        for (map_element *it = m->first; it; it = it->next) { \
                uint32_t hash = km_fnv_1a_hash32(it->key); \
                hash %= m->bucketlen; \
                it->_bucket_link = m->buckets[hash]; \
                m->buckets[hash] = it; \
        } \
} \
\
static void map ## _set(map *m, const char * key, data_type value) \
{ \
        if (m->size >= m->bucketlen / 4 * 3) { \
                map ## _double_buffer(m); \
        } \
        size_t keylen = strlen(key); \
        uint32_t hash = km_fnv_1a_hash32(key); \
        hash %= m->bucketlen; \
        for (map_element *it = m->buckets[hash]; it; it = it->_bucket_link) { \
                if (strcmp(key, it->key) == 0) { \
                        it->value = value; \
                        return; \
                } \
        } \
        map_element *e = KM_MALLOC(sizeof(map_element) + keylen + 1); \
        e->next = NULL; \
        e->value = value; \
        memcpy((void *) e->key, key, keylen + 1); \
        e->previous = m->last; \
        if (m->last == NULL) { \
                m->first = e; \
        } else { \
                m->last->next = e; \
        }\
        m->last = e; \
        e->_bucket_link = m->buckets[hash]; \
        m->buckets[hash] = e; \
        m->size++; \
} \
\
static bool map ## _get(const map *m, const char * key, data_type *nullable_value_out) \
{ \
        uint32_t hash = km_fnv_1a_hash32(key); \
        hash %= m->bucketlen; \
        for (const map_element *it = m->buckets[hash]; it; it = it->_bucket_link) { \
                if (strcmp(it->key, key) == 0) { \
                        if (nullable_value_out) \
                                *nullable_value_out = it->value; \
                        return true; \
                } \
        } \
        return false; \
} \
\
static bool map ## _rm(map *m, const char * key, data_type *nullable_value_out) \
{ \
        uint32_t hash = km_fnv_1a_hash32(key); \
        hash %= m->bucketlen; \
        for (map_element **e_ref = &(m->buckets[hash]); *e_ref; e_ref = &(*e_ref)->_bucket_link) { \
                map_element *to_remove = *e_ref; \
                if (strcmp(to_remove->key, key) == 0) { \
                        if (to_remove->next) \
                                to_remove->next->previous = to_remove->previous; \
                        if (to_remove->previous) \
                                to_remove->previous->next = to_remove->next; \
                        if (nullable_value_out) \
                                *nullable_value_out = to_remove->value; \
                        if (m->first == to_remove) \
                                m->first = to_remove->next; \
                        if (m->last == to_remove) \
                                m->last  = to_remove->previous; \
                        *e_ref = to_remove->_bucket_link; \
                        KM_FREE(to_remove); \
                        m->size--; \
                        return true; \
                } \
        } \
        return false; \
} \
\
static void map ## _clear(map *m) \
{ \
        map_element *it_next; \
        for (map_element *it = m->first; it; it = it_next) { \
                it_next = it->next; \
                KM_FREE(it); \
        } \
        memset(m->buckets, 0, m->bucketlen); \
        m->size = 0; \
} \
\
static void map ## _deinit(map *m) \
{ \
        map_element *it_next; \
        for (map_element *it = m->first; it; it = it_next) { \
                it_next = it->next; \
                KM_FREE(it); \
        } \
        KM_FREE(m->buckets); \
}

KM_MAP(kmint_m, kmint_e, int, int)
KM_MAP(kmchar_m, kmchar_e, char, int)
KM_MAP(kmfloat_m, kmfloat_e, float, double)
KM_MAP(kmdouble_m, kmdouble_e, double, double)
KM_MAP(kmstr_m, kmstr_e, char*, char*)
KM_MAP(kmvoid_m, kmvoid_e, void*, void*)

#endif
