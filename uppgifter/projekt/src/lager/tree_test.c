#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <alloca.h>

#include "tree.h"

void test_simple()
{
  char *a = "a";
  char *b = "b";
  char *c = "c";
  char *d = "d";
  char *e = "e";
  char *f = "f";
  
  tree *t = tree_new((cmp_func) strcmp);

  assert(!tree_contains(t, a));
  tree_insert(t, a, NULL);
  assert(tree_contains(t, a));
  
  assert(!tree_contains(t, b));
  tree_insert(t, b, NULL);
  assert(tree_contains(t, b));

  assert(!tree_contains(t, c));
  tree_insert(t, c, NULL);
  assert(tree_contains(t, c));

  assert(!tree_contains(t, d));
  tree_insert(t, d, NULL);
  assert(tree_contains(t, d));

  assert(!tree_contains(t, e));
  tree_insert(t, e, NULL);
  assert(tree_contains(t, e));

  assert(!tree_contains(t, f));
  tree_insert(t, f, NULL);
  assert(tree_contains(t, f));

  char *buf = alloca(16);
  tree_path_for_key(t, a, &buf);
  assert(strcmp("", buf) == 0);
  
  tree_path_for_key(t, b, &buf);
  assert(strcmp("R", buf) == 0);
  
  tree_path_for_key(t, c, &buf);
  assert(strcmp("RR", buf) == 0);
  
  tree_path_for_key(t, d, &buf);
  assert(strcmp("RRR", buf) == 0);

  tree_path_for_key(t, d, &buf);
  assert(tree_key_for_path(t, buf) == d);

  tree_path_for_key(t, d, &buf);
  printf("%p '%s' is at '%s'\n", d, d, buf);

  tree_balance(t);

  tree_path_for_key(t, a, &buf);
  printf("%p '%s' is at '%s'\n", a, a, buf);
  tree_path_for_key(t, b, &buf);
  printf("%p '%s' is at '%s'\n", b, b, buf);
  tree_path_for_key(t, c, &buf);
  printf("%p '%s' is at '%s'\n", c, c, buf);
  tree_path_for_key(t, d, &buf);
  printf("%p '%s' is at '%s'\n", d, d, buf);
  tree_path_for_key(t, e, &buf);
  printf("%p '%s' is at '%s'\n", e, e, buf);
  tree_path_for_key(t, f, &buf);
  printf("%p '%s' is at '%s'\n", f, f, buf);

  tree_delete(t);
}

int int_cmp(void *a, void *b)
{
  return (*(int *)a) - (*(int *)b);
}

void test_hard()
{
  tree *t = tree_new((cmp_func) int_cmp);

  int keys = rand() % (1024 * 1024);
  int *key_set = calloc(keys, sizeof(int));

  printf("Keys: %d\n", keys);

  for (int i = 0; i < keys; ++i)
    {
      key_set[i] = rand();
    }

  for (int i = 0; i < keys; ++i)
    {
      tree_insert(t, &key_set[i], NULL);
    }

  printf("Tree size: %d\n", tree_size(t));
  printf("Tree depth: %d\n", tree_depth(t));
  
  tree_balance(t);

  char *path_buf = NULL; 

  tree_path_for_key(t, &key_set[0], &path_buf);
  printf("Key: %d at %s\n", key_set[0], path_buf);

  for (int i = 0; i < keys; ++i)
    {
      tree_path_for_key(t, &key_set[i], &path_buf);
      assert(*(int *)tree_key_for_path(t, path_buf) == key_set[i]);
    }

  /// tree_internal_consistency_check(t);

  free(path_buf);
  free(key_set);
  tree_delete(t);
}

void test_remove()
{
  char *a = "a";
  char *b = "b";
  char *c = "c";
  char *d = "d";
  char *e = "e";
  char *f = "f";
  
  tree *t = tree_new((cmp_func) strcmp);

  tree_insert(t, a, NULL);
  tree_insert(t, b, NULL);
  tree_insert(t, c, NULL);
  tree_insert(t, d, NULL);
  tree_insert(t, e, NULL);
  tree_insert(t, f, NULL);

  char *buf = alloca(16);

  /// Test removing root
  assert(tree_contains(t, a));
  assert(tree_contains(t, b));
  tree_path_for_key(t, b, &buf);
  assert(strncmp(buf, "R", 16) == 0);
  tree_remove(t, a);
  assert(tree_contains(t, a) == false);
  assert(tree_contains(t, b));
  tree_path_for_key(t, b, &buf);
  assert(strncmp(buf, "", 16) == 0);

  /// Test removing root due to removal
  assert(tree_contains(t, b));
  assert(tree_contains(t, c));
  tree_path_for_key(t, c, &buf);
  assert(strncmp(buf, "R", 16) == 0);
  tree_remove(t, b);
  assert(tree_contains(t, b) == false);
  assert(tree_contains(t, c));
  tree_path_for_key(t, c, &buf);
  assert(strncmp(buf, "", 16) == 0);

  /// Test removing leaf 
  tree_balance(t);
  assert(tree_contains(t, c));
  tree_remove(t, c);
  assert(tree_contains(t, c) == false);

  tree_delete(t);
}

void test_tree_walkable()
{
  tree *t = tree_new((cmp_func) strcmp);

  char *llr = "LLR";
  assert(tree_walk(t, llr) == llr);

  char *a = "a";
  char *b = "b";
  char *c = "c";

  tree_insert(t, b, NULL);
  tree_insert(t, c, NULL);
  tree_insert(t, a, NULL);

  assert(*tree_walk(t, "")  == '\0');
  assert(*tree_walk(t, "L") == '\0');
  assert(*tree_walk(t, "R") == '\0');

  assert(tree_walk(t, llr) == llr + 1);

  tree_delete(t);
}

int main(int argc, char *argv[])
{
  test_simple();
  /// test_hard();
  test_remove();

  test_tree_walkable();

  puts("------------------------------------------------------------\n"
       "If you can read this, then the tests have (probably) passed.\n"
       "------------------------------------------------------------\n");

  return 0;
}
