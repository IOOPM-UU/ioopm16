#pragma once

#include "list.h"
#include "db.h"

enum action_type { add, remove, edit };

struct action
{
  enum action_type type;
  char *name;  // Always an alias so should never be free'd
  good *good;
  int amount;
  char *shelf; // TODO
};

typedef list undo_t;
typedef struct action action_t;

void undo_store_action(undo_t *undo, action_t *a);
action_t *undo_pop(undo_t *undo);

action_t *undo_new_add_action(good *g);
action_t *undo_new_remove_action(good *g);
action_t *undo_new_edit_action(char *original_key, good *g);
void undo_action_delete(action_t *a);
