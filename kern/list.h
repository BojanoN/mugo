#pragma once

typedef struct lst_hdr {
    struct lst_hdr* next;
    struct lst_hdr* prev;
} lst_hdr_t;

#define LIST_HEAD(var) lst_hdr_t var = { &(var), &(var) }

#define LIST_HEAD_INIT(var) (var)->next = (var)->prev = (var);

// Returns underlying struct pointer
#define list_data(ptr, type, lst_hdr_member) ({                    \
    (type*)((char*)ptr - (size_t) & (((type*)0)->lst_hdr_member)); \
})

#define list_for_each(cursor, hdr) \
    for (cursor = (hdr)->next; cursor != (hdr); cursor = cursor->next)

static inline void list_add(lst_hdr_t* new, lst_hdr_t* head)
{
    lst_hdr_t* head_next = head->next;

    head_next->prev = new;
    head->next      = new;
    new->next       = head_next;
    new->prev       = head;
}

static inline void list_remove(lst_hdr_t* hdr)
{
    hdr->next->prev = hdr->prev;
    hdr->prev->next = hdr->next;
}

static inline int list_is_empty(lst_hdr_t* hdr)
{
    return hdr->next == hdr && hdr->prev == hdr;
}

static inline lst_hdr_t* list_next(lst_hdr_t* hdr)
{
    lst_hdr_t* ret = hdr->next;

    list_remove(ret);

    return ret;
}
