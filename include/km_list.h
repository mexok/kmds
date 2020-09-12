#ifndef KM_LIST_h
#define KM_LIST_h

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef KM_MALLOC
#define KM_MALLOC malloc
#endif

#ifndef KM_FREE
#define KM_FREE free
#endif


#define KM_LIST(list, data_type, va_data_type) \
typedef struct{ \
        size_t size; \
        size_t offset; \
        size_t bufferlen; \
        data_type *buffer; \
} list; \
\
static void list ## _init(list *l) \
{ \
        size_t bufferlen = 16; \
        *l = (list){ \
                .bufferlen = bufferlen, \
                .buffer =  (data_type *) KM_MALLOC(bufferlen * sizeof(data_type)) \
        }; \
} \
\
static void list ## _vinitd(list *l, size_t size, va_list values) \
{ \
        size_t bufferlen = 16; \
        while (bufferlen < size) \
                bufferlen *= 2; \
        *l = (list){ \
                .size = size, \
                .offset = 0, \
                .bufferlen = bufferlen, \
                .buffer =  (data_type *) KM_MALLOC(bufferlen * sizeof(data_type)) \
        }; \
        for (size_t i = 0; i < size; ++i) \
                l->buffer[i] = (data_type) va_arg(values, va_data_type); \
} \
\
static void list ## _initd(list *l, size_t size, ...) \
{ \
        va_list values; \
        va_start(values, size); \
        list ## _vinitd(l, size, values); \
        va_end(values); \
} \
\
static void list ## _cinit(list *l, const list *to_copy) \
{ \
        *l = (list){ \
                .size = to_copy->size, \
                .offset = to_copy->offset, \
                .bufferlen = to_copy->bufferlen, \
                .buffer = (data_type *) KM_MALLOC(to_copy->bufferlen * sizeof(data_type)) \
        }; \
        memcpy(l->buffer, to_copy->buffer, l->bufferlen); \
} \
\
static void list ## _double_buffer(list *l) \
{ \
        size_t new_bufferlen = l->bufferlen * 2; \
        data_type *new_buffer = (data_type *) KM_MALLOC(new_bufferlen * sizeof(data_type)); \
        memcpy(new_buffer, l->buffer + l->offset, (l->size - l->offset) * sizeof(data_type)); \
        if (l->offset + l->size > l->bufferlen) \
                memcpy(new_buffer + l->offset, l->buffer, l->offset + l->size - l->bufferlen); \
        l->offset = 0; \
        l->buffer = new_buffer; \
        l->bufferlen = new_bufferlen; \
} \
\
static data_type list ## _at(const list *l, size_t index) \
{ \
        assert(index < l->size); \
        return l->buffer[(index + l->offset) % l->bufferlen]; \
} \
\
static void list ## _set(list *l, size_t index, data_type d) \
{ \
        assert(index < l->size); \
        l->buffer[(index + l->offset) % l->bufferlen] = d; \
} \
\
static void list ## _push(list *l, data_type d) \
{ \
        if (l->size == l->bufferlen) \
                list ## _double_buffer(l); \
        l->buffer[(l->offset + l->size) % l->bufferlen] = d; \
        ++l->size; \
} \
\
static data_type list ## _pop(list *l) \
{ \
        assert(l->size > 0); \
        return l->buffer[(--l->size + l->offset) % l->bufferlen]; \
}\
\
static data_type list ## _shift(list *l) \
{ \
        assert(l->size > 0); \
        data_type result = list ## _at(l, 0); \
        --l->size; \
        l->offset = (l->offset + 1) % l->bufferlen; \
        return result; \
} \
\
static void list ## _unshift(list *l, data_type d) \
{ \
        if (l->size == l->bufferlen) \
                list ## _double_buffer(l); \
        l->offset = (l->offset + l->bufferlen - 1) % l->bufferlen; \
        l->buffer[l->offset] = d; \
        ++l->size; \
} \
\
static void list ## _clear(list *l) \
{ \
        l->offset = 0; \
        l->size = 0; \
} \
\
static void list ## _deinit(list *l) \
{ \
        KM_FREE(l->buffer); \
}

KM_LIST(kmint_l, int, int)
KM_LIST(kmchar_l, char, int)
KM_LIST(kmfloat_l, float, double)
KM_LIST(kmdouble_l, double, double)
KM_LIST(kmstr_l, char*, char*)
KM_LIST(kmvoid_l, void*, void*)

#endif
