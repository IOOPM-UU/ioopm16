#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tree.h"
#include "list.h"
#include "undo.h"

typedef struct good good;
typedef struct db db;

void entry_deep_free_func(char *key, good *value);

struct good
{
  char *name;
  char *desc;
  list *shelves;
  unsigned int amount;
  unsigned int price_in_cents;
};

struct db
{
  tree *goods;
  list *undo_actions;
};

good *good_new(char *name, char *desc, int price, int amount)
{
  good *result = malloc(sizeof(*result));

  result->name = name;
  result->desc = desc;
  result->shelves = list_new();
  result->amount = amount;
  result->price_in_cents = price;

  return result;
}

void good_delete(good *g)
{
  free(g->name);
  free(g->desc);
  list_delete(g->shelves); // TODO: free shelf data
  free(g);
}

db *db_new()
{
  db *result = malloc(sizeof(*result));
  *result = (db) {
    .goods        = tree_new((cmp_func) strcmp),
    .undo_actions = list_new()
  };
  return result;
}

void db_delete(db *db)
{
  action_t *undo_action = undo_pop(db->undo_actions);
  while (undo_action)
    {
      undo_action_delete(undo_action);
      free(undo_action);
      undo_action = undo_pop(db->undo_actions);
    }
  list_delete(db->undo_actions);

  tree_delete_visit(db->goods, (tree_visitor_func) entry_deep_free_func);
  free(db);
}

static inline void db_internal_replace_good(db *db, good *from, good *to, bool undo)
{
  /// FIXME: use tree_insert without remove when keys are equivalent
  tree_remove(db->goods, from->name);
  tree_insert(db->goods, to->name, to);

  if (undo)
    {
      action_t *a = undo_new_edit_action(to->name, from);
      undo_store_action(db->undo_actions, a);
    }
  else
    {
      /// If we are not undoing, we should destroy the old record
      good_delete(from);
    }
}

static inline void db_internal_remove_good(db *db, char *name, bool undo)
{
  good *g = tree_get(db->goods, name);

  if (undo)
    {
      // Push a corresponding add action
      action_t *a = undo_new_add_action(g);
      undo_store_action(db->undo_actions, a);
    }
  else
    {
      /// If we are not undoing, we should destroy the old record
      good_delete(g);
    }

  // Finally remove it from the tree
  tree_remove(db->goods, name);
}

static inline void db_internal_add_good(db *db, good *g, bool undo)
{
  if (undo)
    {
      // Push a corresponding remove action
      action_t *a = undo_new_remove_action(g);
      undo_store_action(db->undo_actions, a);
    }

  tree_insert(db->goods, g->name, g);
}

void db_add_good(db *db, good *g)
{
  db_internal_add_good(db, g, true);
}

void db_remove_undos(db *db, int steps_to_remove)
{
  for (int i = 0; i < steps_to_remove; ++i)
    {
      undo_action_delete(undo_pop(db->undo_actions));
    }
}

bool db_undo_last_action(db *db)
{
  action_t *action = undo_pop(db->undo_actions);

  // If nothing to undo, do nothing
  if (action)
    {
      switch (action->type)
        {
        case add:
          {
            db_internal_add_good(db, action->good, false);
            break;
          }
        case remove:
          {
            db_internal_remove_good(db, action->good->name, false);
            break;
          }
        case edit:
          {
            db_internal_replace_good(db, tree_get(db->goods, action->name), action->good, false);
            break;
          }
        default:
          {
            assert(false);
          }
        }
      free(action);
      return true;
    }
  else
    {
      return false;
    }
}

void db_replace_good(db *db, good *from, good *to)
{
  db_internal_replace_good(db, from, to, true);
}

void db_remove_good(db *db, char *name)
{
  db_internal_remove_good(db, name, true);
}

void db_dump_into_list_func(void *name, void *ignored, void *list)
{
  list_append(list, name);
}

list *db_list_goods(db *db)
{
  list *result = list_new();
  tree_visit(db->goods, inorder, db_dump_into_list_func, result);
  return result;
}

char *good_name(good *g)
{
  return g->name;
}

char *good_desc(good *g)
{
  return g->desc;
}

int good_price(good *g)
{
  return g->price_in_cents;
}

good *db_get_good(db *db, char *name)
{
  return tree_get(db->goods, name);
}

void good_set_name(good *g, char *name)
{
  g->name = name;
}

void good_set_desc(good *g, char *desc)
{
  g->desc = desc;
}

void good_set_price(good *g, int price)
{
  g->price_in_cents = price;
}

good *good_deep_copy(good *good)
{
  return good_new(strdup(good->name),
                  strdup(good->desc),
                  good->price_in_cents,
                  good->amount);
}

void good_deep_free(good *g)
{
  free(g->name);
  free(g->desc);
  list_delete(g->shelves);
  free(g);
}

void entry_deep_free_func(char *key, good *value)
{
  good_deep_free(value);
}
