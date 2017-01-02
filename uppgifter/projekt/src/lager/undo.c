#include <stdlib.h>
#include "undo.h"
#include "list.h"
#include "db.h"

void undo_store_action(undo_t *undo, action_t *action)
{
  list_prepend(undo, action);
}

action_t *undo_top(undo_t *undo)
{
  return list_get(undo, 0);
}

action_t *undo_pop(undo_t *undo)
{
  if (list_length(undo) > 0)
    {
      action_t *action = undo_top(undo);
      list_remove(undo, 0);
      return action;
    }
  else
    {
      return NULL;
    }
}

void undo_action_delete(action_t *a)
{
  switch (a->type)
    {
    case remove: // item is in DB so will be deleted from db_delete
      {
        break;
      }
    case add:  // action has master copy -- should be deleted
    case edit: // action has master copy -- should be deleted
      {
        good_delete(a->good);
        break;
      }
    }
}

action_t *undo_new_add_action(good *g)
{
  action_t *action = calloc(1, sizeof(action_t));
  action->type = add;
  action->name = good_name(g);
  action->good = g;
  return action;
}

action_t *undo_new_remove_action(good *g)
{
  action_t *action = calloc(1, sizeof(action_t));
  action->type = remove;
  action->name = good_name(g);
  action->good = g;
  return action;
}

action_t *undo_new_edit_action(char *original_key, good *g)
{
  action_t *action = calloc(1, sizeof(action_t));
  action->type = edit;
  action->name = original_key;
  action->good = g;
  return action;
}
