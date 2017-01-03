#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <alloca.h>
#include "db.h"
#include "list.h"

static inline void clear_screen()
{
  printf("\e[1;1H\e[2J");
}

static inline void tui_getline(char **buf, size_t *len)
{
  if (len == 0 || *buf == NULL)
    {
      *len = 128;
      *buf = calloc(*len, sizeof(char));
    }
  
  size_t read = 0;
  int c;
  
  do
    {
      if (read < *len)
        {
          (*buf)[read++] = c = getchar();
        }
      else
        {
          /// Grow buffer size using realloc
          *len *= 2;
          *buf = realloc(*buf, *len);
          continue;
        }
    }
  while (c != '\n' && c != EOF && c != '\0');
    
  (*buf)[read-1] = '\0';
  *len = read-1;
}

static inline bool is_number(const char *str)
{
  if (*str == '-') ++str;
  
  while (*str)
    {
      if (isdigit(*str++) == false) return false;
    }
  return true;
}

char *ask_question_string_into(char *prompt, char **buf, size_t *len)
{
  printf("%s\n", prompt);
  tui_getline(buf, len);
  return *buf;
}

char *ask_question_string(char *prompt)
{
  char *result = NULL;
  size_t len = 0;
  return ask_question_string_into(prompt, &result, &len);
}

char ask_question_char_and_validate(char *prompt, char *valid)
{
  size_t len = 16;
  char *buf = alloca(len);
  
  do
    {
      ask_question_string_into(prompt, &buf, &len);
    }
  while (valid && strchr(valid, buf[0]) == NULL);
  
  return buf[0];
}

char ask_question_char(char *prompt)
{
  return ask_question_char_and_validate(prompt, NULL);
}

char *ask_question_shelf(char *prompt)
{
  char *result = ask_question_string(prompt);
  while (strlen(result) != 3 &&
         isdigit(result[0]) &&
         isascii(result[1]) &&
         isascii(result[2])) {
    free(result);
    result = ask_question_string(prompt);
  }
  return result;
}

long ask_question_int(char *prompt)
{
  char *buf = NULL;
  size_t len = 0;
    
  ask_question_string_into(prompt, &buf, &len);

  while (is_number(buf) == false)
    {
      free(buf);
      buf = NULL;
      len = 0;
      
      puts("Invalid number!");
      ask_question_string_into(prompt, &buf, &len);
    }
  
  long result = atol(buf);
  free(buf);
  return result;
}

void tui_display_good(good *g)
{
  printf("Good --------------------------\n"
         "  Name: %s\n"
         "  Desc: %s\n"
         "  Price: %d\n"
         "-------------------------------\n",
         good_name(g), good_desc(g), good_price(g));
}

good *tui_enter_good()
{
  clear_screen();
  char *name = ask_question_string("Name: ");
  char *desc = ask_question_string("Description: ");
  int price = ask_question_int("Price (in cents): ");
  int amount = ask_question_int("Amount: ");
  return good_new(name, desc, price, amount);
}

char *tui_remove_good()
{
  clear_screen();
  return ask_question_string("Name of good to remove: ");
}

bool user_event_edit_good_internal(db *db, good *g)
{
  good *copy = good_deep_copy(g);
  tui_display_good(copy);
  int edits = 0;
  
  while (true)
    {
      puts("\nAction:\n"
           "[N]ame\n"
           "[D]escription\n"
           "[P]rice\n"
           "[S]helf\n");
      char choices = ask_question_char("Choose option (or, [E]xit to DB or [A]bort)");

      switch (choices)
        {
        case 'E':
        case 'e':
          {
            db_replace_good(db, g, copy);
            return true;
          }
        case 'A':
        case 'a':
          {
            db_remove_undos(db, edits);
            
            good_deep_free(copy); 
            return false;
          }
        case 'N':
        case 'n':
          {
            ++edits; 

            printf("Old name: '%s'\n", good_name(copy));
            free(good_name(copy));
            good_set_name(copy, ask_question_string("New name: "));
            break;
          }
        case 'D':
        case 'd':
          {
            ++edits; 

            printf("Old description: '%s'\n", good_desc(copy));
            free(good_desc(copy));
            good_set_desc(copy, ask_question_string("New description: "));
            break;
          }
        case 'P':
        case 'p':
          {
            ++edits; 

            printf("Old price: '%d'\n", good_price(copy));
            good_set_price(copy, ask_question_int("New price: "));
            break;
          }
        case 'S':
        case 's':
          {
            puts("Not implemented yet!");
            break;
          }
        }
    }
  return false;
}

bool user_event_undo(db *db)
{
  return db_undo_last_action(db);
}

bool user_event_edit_good(db *db)
{
  clear_screen();

  char *name = ask_question_string("Name of good to edit: ");
  good *g = db_get_good(db, name);
  free(name);
  
  if (g == NULL)
    {
      puts("Error: no good with that name!");
      ask_question_char("Press ENTER to continue.");
      return false;
    }
  else
    {
      user_event_edit_good_internal(db, g);
      return true;
    }
}

bool user_event_add_good(db *db)
{
  clear_screen();
  good *g = tui_enter_good();
  tui_display_good(g);

  char choices = ask_question_char("[S]ave to DB, [E]dit or [A]bort?");

  switch (choices)
    {
    case 'S':
    case 's':
      {
        db_add_good(db, g);
        return true;
      }
    case 'E':
    case 'e':
      {
        return user_event_edit_good_internal(db, g);
      }
    case 'A':
    case 'a':
      {
        if (g) good_deep_free(g);
        return false;
      }
    }
  return false;
}

bool user_event_view_goods(list *good_names, db *db)
{
  int good_index = ask_question_int("Enter good number, 0 to exit");

  if (good_index == 0) return false; /// 0 to exit
  
  if (0 < good_index && good_index <= list_length(good_names))
    {
      tui_display_good(db_get_good(db, list_get(good_names, good_index - 1)));
    }
  else
    {
      puts("Invalid index");
    }

  return true;
}

bool user_event_remove_good(db *db)
{
  char *name = tui_remove_good();
  if (db_get_good(db, name))
    {
      db_remove_good(db, name);
      free(name);
      return true;
    }
  else
    {
      return false;
    }
}

void user_event_list_goods(db *db)
{
  list *good_names = db_list_goods(db);

  if (list_length(good_names) > 0)
    {
      iter *name_iterator = list_iterator(good_names);
      int index = 0;

      puts("\nGoods in DB:");
      while (iterator_has_more(name_iterator))
        {
          printf("%d: %s\n", ++index, (char *)iterator_next(name_iterator));
        }
      puts("--------------");
      iterator_delete(name_iterator);
    }
  else
    {
      puts("Database does not contain any records\n");
    }

  while (true)
    {
      char answer = ask_question_char("[V]iew good, or [R]eturn to main menu");

      if (answer == 'R' || answer == 'r') break;

      if (user_event_view_goods(good_names, db) == false) break;
    }

  list_delete(good_names);
}


void event_loop(db* db)
{
  char *status_msg = NULL;

  while (true)
    {
      clear_screen();
      puts(
           "Welcome to DB 0.1\n"
           "=================================\n"
           "\n"
           "[A]dd Goods\n"
           "[R]emove Goods\n"
           "[E]dit Goods\n"
           "[U]ndo Last Action \n"
           "[P]rint the DB\n"
           "[Q]uit\n"
           );

      if (status_msg)
        {
          printf(">> %s <<\n\n", status_msg);
          status_msg = NULL;
        }

      char choice = ask_question_char_and_validate("What's up, doc?", "AREUPQareupq");

      switch (choice)
        {
        case 'A':
        case 'a':
          {
            status_msg = user_event_add_good(db) ?
              "Added" :
              "Nothing added";
            break;
          }
        case 'R':
        case 'r':
          {
            status_msg = user_event_remove_good(db) ?
              "Removed" :
              "Nothing removed";
            break;
          }
        case 'E':
        case 'e':
          {
            user_event_edit_good(db);
            break;
          }
        case 'U':
        case 'u':
          {
            status_msg = user_event_undo(db) ?
              "Undo!" :
              "Nothing to undo!";
            break;
          }
        case 'P':
        case 'p':
          {
            user_event_list_goods(db);
            break;
          }
        case 'Q':
        case 'q':
          {
            return;
          }
        }
    }
}

int main(int argc, char *argv[])
{
  db *db = db_new();
  event_loop(db);
  db_delete(db);
  return 0;
}
