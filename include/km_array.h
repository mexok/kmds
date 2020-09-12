#ifndef km_array_h
#define km_array_h

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef KM_MALLOC
#define KM_MALLOC malloc
#endif

#ifndef KM_CALLOC
#define KM_CALLOC calloc
#endif

#ifndef KM_FREE
#define KM_FREE free
#endif


#define KM_ARRAY(array, data_type, va_data_type) \
typedef struct{ \
        size_t len; \
        data_type *values; \
} array; \
\
static void array ## _init(array *a, size_t len) \
{ \
        *a = (array) { \
                .len = len, \
                .values =  (data_type *) KM_CALLOC(len, sizeof(data_type)) \
        }; \
} \
\
static void array ## _vinitd(array *a, size_t len, va_list values) \
{ \
        *a = (array) { \
                .len = len, \
                .values = (data_type *) KM_MALLOC(len * sizeof(data_type)) \
        }; \
        for (size_t i = 0; i < len; ++i) \
                a->values[i] = (data_type) va_arg(values, va_data_type); \
} \
\
static void array ## _initd(array *a, size_t len, ...) \
{ \
        va_list values; \
        va_start(values, len); \
        array ## _vinitd(a, len, values); \
        va_end(values); \
} \
\
static void array ## _cinit(array *a, const array *to_copy) \
{ \
        a->values = (data_type *) KM_MALLOC(to_copy->len * sizeof(data_type)); \
        a->len = to_copy->len; \
        memcpy(a->values, to_copy->values, a->len * sizeof(data_type)); \
} \
\
static void array ## _deinit(array *a) \
{ \
        KM_FREE(a->values); \
}

KM_ARRAY(kmint_a, int, int)
KM_ARRAY(kmchar_a, char, int)
KM_ARRAY(kmfloat_a, float, double)
KM_ARRAY(kmdouble_a, double, double)
KM_ARRAY(kmstr_a, char*, char*)
KM_ARRAY(kmvoid_a, void*, void*)

#endif
