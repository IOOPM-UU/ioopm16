#pragma once

#include "list.h"

typedef struct good good;
typedef struct db db;

good *good_new(char *name, char *desc, int price, int amount);
void good_delete();
db *db_new();
void db_delete(db *db);
void db_add_good(db *db, good *g);
good *db_get_good(db *db, char *name);
void db_remove_good(db *db, char *name);
void db_replace_good(db *db, good *from, good *to);
list *db_list_goods(db *db);
char *good_name(good *g);
char *good_desc(good *g);
int good_price(good *g);
void good_set_name(good *g, char *name);
void good_set_desc(good *g, char *desc);
void good_set_price(good *g, int price);
good *good_deep_copy(good *g);
void good_deep_free(good *g);
bool db_undo_last_action(db *db);
void db_remove_undos(db *db, int steps_to_remove);
