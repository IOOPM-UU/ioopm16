/// This program should be run with a max heap size of
/// 1.2 * No_lists * Elements * sizeof(struct link) to
/// trigger collection during the 2nd call to create_lists.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/// Change the behaviour of the program by changing the defines
/// #define NO_GC
/// #define BOEHM_GC
/// #define IOOPM_GC

#ifdef BOEHM_GC
#include <gc/gc.h>
#endif

#ifdef IOOPM_GC
#include "gc.h"

heap_t *h;
#endif

/// Hacky: no header for utils
extern bool isnumber(const char *str);
extern bool parse_int_option(const char *arg, const char *opt, int *value);

typedef struct list list_t;
typedef struct link link_t;

struct link
{
  link_t *next;
  int element;
};

struct list
{
  link_t *first;
  link_t *last;
};

#define Choose_list(element, span) (element / span)

link_t *new_link(int element)
{
#ifdef NO_GC
  link_t *link = malloc(sizeof(struct link));
#endif
#ifdef BOEHM_GC
  link_t *link = GC_malloc(sizeof(struct link));
#endif
#ifdef IOOPM_GC
    link_t *link = h_alloc_struct(h, "*i");
#endif

  link->element = element;
  link->next = NULL;
  return link;
}

// Each list starts with a dummy to avoid constantly checking empty
list_t new_list()
{
  link_t *link = new_link(0);
  return (list_t) { .first = link, .last = link };
}

void list_append(list_t *list, int element)
{
  list->last->next = new_link(element);
  list->last = list->last->next;
}

bool list_find(list_t *list, int element)
{
  // Skip dummy
  link_t *cursor = list->first->next;

  while (cursor)
    {
      if (cursor->element == element) return true;
      cursor = cursor->next;
    }

  return false;
}

void teardown(list_t lists[], int no_lists)
{
#ifdef NO_GC
  for (int i = 0; i < no_lists; ++i)
    {
      link_t *cursor = lists[i].first;
      while (cursor)
        {
          link_t *tmp = cursor->next;
          free(cursor);
          cursor = tmp;
        }
    }
#endif
#ifdef BOEHM_GC
  GC_gcollect(); /// Requires building and linking against BDW GC
#endif
#ifdef IOOPM_GC
  h_gc(h); /// Requires building and linking against IOOPM GC
#endif
}

void create_lists(list_t lists[], int no_lists, int elements, int max)
{
  assert((max / no_lists) % 2 == 0);

  for (int i = 0; i < elements; ++i)
    {
      int element = rand() % max;

      list_append(&lists[Choose_list(element, max / no_lists)], element);
    }
}

int lookup_elements(list_t lists[], int no_lists, int elements, int max)
{
  int found = 0;
  
  for (int i = 0; i < elements; ++i)
    {
      int element = rand() % max;

      if (list_find(&lists[Choose_list(element, max / no_lists)], element)) ++found;
    }

  return found;
}

int main(int argc, char *argv[])
{
  int no_lists = 4;
  int elements = 1024 * 128;
  int max = no_lists * elements * 2;

#ifdef IOOPM_GC
  h = h_init(max * sizeof(struct link) * 2.4, true, 0.5);
#endif
  
  if (argc > 1)
    {
      for (int i = 1; i < argc; ++i)
        {
          const char *arg = argv[i];
          if (parse_int_option("--lists=", arg, &no_lists)) continue;
          if (parse_int_option("--max=", arg, &max)) continue;
          if (parse_int_option("--elements=", arg, &elements)) continue;
        }
    }

  printf("Running with lists=%d, max=%d, elements=%d\n", no_lists, max, elements);

  list_t lists[no_lists];

  /// Initiate the list heads
  for (int i = 0; i < no_lists; ++i) lists[i] = new_list();
  /// Create some extra memory pressure
  create_lists(lists, no_lists, elements, max);

  /// Delete the roots to the lists
  for (int i = 0; i < no_lists; ++i) lists[i] = new_list();
  /// Re-create the lists again
  create_lists(lists, no_lists, elements, max);

  int found = lookup_elements(lists, no_lists, elements, max);

  printf("Done. Found %d elements.\n", found);

  teardown(lists, no_lists);
  
  return 0;
}
