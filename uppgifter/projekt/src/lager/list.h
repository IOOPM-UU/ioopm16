#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct list list;
typedef struct iter iter;

typedef void (*list_visitor_func)(void *elem, void *ptr);
typedef void *(*list_map_func)(void *elem, void *ptr);
typedef int (*cmp_func)(void *a, void *b);

list *list_new();
void list_delete(list *l);
bool list_contains(list *l, void *elem, cmp_func f);
void list_append(list *l, void *elem);
void list_prepend(list *l, void *elem);
bool list_remove(list *l, int index);

void list_visit(list *l, list_visitor_func f, void *ptr);
list *list_map(list *l, list_map_func f, void *ptr);
void *list_fold(list *l, list_map_func f, void *ptr);

size_t list_length(list *l);
void *list_get(list *l, int index);

iter *list_iterator(list *l);
iter *iterator_new(void *current);
void *iterator_next(iter *iter);
bool iterator_has_more(iter *iter);
void iterator_delete(iter *iter);

