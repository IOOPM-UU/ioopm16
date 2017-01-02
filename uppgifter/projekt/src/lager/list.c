#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

#ifdef GC_TEST
extern heap_t *h; // Assume single heap for this program for simplicity
#endif

typedef struct list list;
typedef struct node node;
typedef void (*list_visitor_func)(void *elem, void *ptr);
typedef void *(*list_map_func)(void *elem, void *ptr);
typedef int (*cmp_func)(void *a, void *b);

struct list
{
  node *first;
  node *last;
  uint32_t size;
};

struct node
{
  void *elem;
  node *next;
};

iter *list_iterator(list *l)
{
  return iterator_new(l->first);
}

bool list_contains(list *l, void *elem, cmp_func f)
{
  for (node *cursor = l->first; cursor; cursor = cursor->next)
    {
      if (f(cursor->elem, elem) == 0) return true;
    }

  return false;
}

void list_visit(list *l, list_visitor_func f, void *ptr)
{
  for (node *cursor = l->first; cursor; cursor = cursor->next)
    {
      f(cursor->elem, ptr);
    }
}

list *list_new()
{
#ifdef GC_TEST
  return h_alloc_struct(h, "**i");
#else
  return calloc(1, sizeof(struct list));
#endif
}

static inline node *list_internal_node_new(void *elem, node *next)
{
#ifdef GC_TEST
  node *result = h_alloc_struct(h, "**");
#else
  node *result = malloc(sizeof(*result));
#endif
  
  *result = (struct node) { .elem = elem, .next = next };

  return result;
}

void list_append(list *l, void *elem)
{
  if (l->size)
    {
      node *old_last = l->last;
      l->last = list_internal_node_new(elem, NULL);
      old_last->next = l->last;
    }
  else
    {
      l->last = list_internal_node_new(elem, NULL);
      l->first = l->last;
    }

  ++l->size;
}

void list_prepend(list *l, void *elem)
{
  if (l->size)
    {
      l->first = list_internal_node_new(elem, l->first);
    }
  else
    {
      l->first = list_internal_node_new(elem, NULL);
      l->last = l->first;
    }

  ++l->size;
}

static inline node **list_find(list *l, int index)
{
  node **cursor = &(l->first);

  for (int i = 0; i < index; ++i) cursor = &((*cursor)->next);

  return cursor;
}

void *list_get(list *l, int index)
{
  if (0 <= index && index < l->size)
    {
      node **cursor = list_find(l, index);
      return (*cursor)->elem;
    }

  return NULL;
}

bool list_remove(list *l, int index)
{
  if (0 <= index && index < l->size)
    {
      node **cursor = list_find(l, index);

      void *tmp = *cursor;
      
      /// Unlink
      *cursor = (*cursor)->next;
      --l->size;

      free(tmp);

      return true;
    }

  return false;
}

list *list_map(list *l, list_map_func f, void *ptr)
{
  list *result = list_new();

  for (node *cursor = l->first; cursor; cursor = cursor->next)
    {
      list_append(result, f(cursor->elem, ptr));
    }

  return result;
}

void *list_fold(list *l, list_map_func f, void *ptr)
{
  void *result = ptr;

  for (node *cursor = l->first; cursor; cursor = cursor->next)
    {
      result = f(cursor->elem, result);
    }

  return result;
}

void list_delete(list *l)
{
  while (l->size > 0)
    {
      list_remove(l, 0);
    }
  free(l);
}

size_t list_length(list *l)
{
  return l->size;
}
