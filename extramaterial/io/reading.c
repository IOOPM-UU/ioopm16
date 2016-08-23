#include "reading.h"
#include <alloca.h>

/// Notes
/// 1) This assumes the platform uses \n for newline, some uses \r\n.
///    Fixing this to be platform-independent is left as an exercise. 

#define READ_INT_INVALID_MSG "Invalid input"

/// Reads a line from the keyboard, puts it in the len-sized
/// memory space pointed to by buf, and optionally removed newlines
char *read_string_with_buffer(char *buf, size_t len, bool strip_newline)
{
  ssize_t read = getline(&buf, &len, stdin);
  if (read > 0 && strip_newline)
    {
      buf[read-1] = '\0'; // Note 1
    }
  return buf;
}

/// Reads a line from the keyboard, converts it to an int if it contains
/// only digits, otherwise, keeps asking. 
int read_int(bool repeat_until_valid_int)
{
  #define BUF_LEN 16
  char *buf = alloca(BUF_LEN);
  bool only_digits = true;

  do
    {
      if (!only_digits) puts(READ_INT_INVALID_MSG); 
	
      read_string_with_buffer(buf, BUF_LEN, STRIP_NEWLINE);
      only_digits = true;
  
      for (char *c = buf; *c && only_digits; ++c)
	{
	  only_digits = only_digits && isdigit(*c);
	}

    } while (only_digits == false);

  return atol(buf);
}

bool is_valid_int(char *str)
{
  bool valid_int = true;
  for (char *c = str; *c && valid_int; ++c)
    {
      valid_int = isdigit(*str);
    }
  return valid_int;
}

shelt_t ask_shelf_question()
{
  shelf_t shelf; // har char name; int number;

  char *name = NULL; 
  do {
    if (name) ffree(name);
    name = ask_string_question("Mata in ett tecken");
    shelf.name = name[0]; 
  } while (strlen(name != 1));

  long number = 0; 
  do {
    if (number) ffree(number);
    number = ask_int_question("Mata in ett tal 0--99");
    shelf.number = number;
  } while (0 <= number && number < 100);

  return shelf;
}

/// Asks a question and reads a string in response
char *ask_string_question(char *question)
{
  puts(question);
  return read_string();
}

/// Asks a question and reads a char in response
int ask_int_question(char *question)
{
  puts(question);
  return read_int(VALIDATE);
}

void *ask_question(char *q, v_f validate, m_f convert, c_f cleanup)
{
  // Ask question until optional validation of input is satisfied
  char *input = NULL;
  do {
    puts(q);
    if (input) lfree(input);
    input = read_string();
  } while (validate && validate(input) == false);

  // If a conversion function was specificied, convert input
  void *result = convert ? convert(input) : input; 

  // If a cleanup function was specificied, run it
  if (cleanup) cleanup(input);
  return result;
}

typedef bool  (*v_f)(void *);
typedef void *(*m_f)(void *);
typedef void  (*c_f)(void *);

bool ok_shelf(char *input)
{
  return strlen(input) == 3 && isalpha(input[0]) && valid_int(input+1); 
}

shelf_t *make_shelf(char *input)
{
  shelf_t *shelf = lmalloc(sizeof(shelf_t));
  shelf->name = input[0];
  shelf->number = atol(input+1);
  return shelf;
}

union result
{
  void *ptr;
  long value;
};

// ändra void * => result_t i ask_question och m_f
typedef union result result_t;

union_t make_shelf(char *input)
{
  shelf_t *shelf = lmalloc(sizeof(shelf_t));
  shelf->name = input[0];
  shelf->number = atol(input+1);
  return (union_t) { .ptr = shelf };
};

shelf_t str_to_int(char *s)
{
  return (union_t) { .ptr = atol(s) };
}

shelf_t ask_shelf_question()
{ 
  return *ask_question("Mata in ett heltal",
                       valid_int,
                       str_to_int,
                       true); 
}

#define Ask_char(q)         ask_question(q, valid_char, str_to_char, true);
#define Ask_int(q)          ask_question(q, valid_int,  str_to_int,  true);
#define Ask_str(q)          ask_question(q, NULL,       str_to_str,  false);

#define Ask_namn()          Ask_str("Namn:");
#define Ask_beskrivning()   Ask_str("Beskrivning:");
#define Ask_pris()          Ask_int("Pris:")
#define Ask_lagerhylla()    ask_question("Lagerhylla ...", valid_shelf, str_to_shelf, true);
#define Ask_antal()         Ask_int("Antal:");

void add_goods(db_t *db)
{
  goods_t g;

  do {
    g.name   = Ask_namn();
    g.desc   = Ask_beskrivning();
    g.price  = Ask_pris();
    g.shelf  = Ask_lagerhylla();
    g.amount = Ask_antal();
    
    char answer = Ask_char("Spara? (ja/nej)");
  } while (strchr("Jj", answer) == false);
    
  db->goods[db->total] = g;
  ++db->total;
}

void edit_goods(db_t *db, goods_t g)
{
  char answer = Ask_char(
    );

      g.name   = Ask_namn();
      g.desc   = Ask_beskrivning();
      g.price  = Ask_pris();
      g.shelf  = Ask_lagerhylla();
      g.amount = Ask_antal();

      char answer = Ask_char("Spara? (ja/nej)");
  } while (strchr("Jj", answer) == false);
    
  db->goods[db->total] = g;
  ++db->total;
}
