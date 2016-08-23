#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#ifndef __reading_h__
#define __reading_h__

#define STRIP_NEWLINE true
#define KEEP_NEWLINE  false
#define VALIDATE      true

/// Reads a line from the keyboard, puts it in the len-sized
/// memory space pointed to by buf, and optionally removed newlines
char *read_string_with_buffer(char *buf, size_t len, bool strip_newline);

/// Reads a line from the keyboard, puts it in a string on the
/// heap, and returns a pointer to it.
#define read_string() read_string_with_buffer(NULL, 0, STRIP_NEWLINE)

/// Reads a line from the keyboard, converts it to an int if it contains
/// only digits, otherwise, keeps asking. 
int read_int(bool repeat_until_valid_int);

/// Asks a question and reads a string in response
int ask_int_question(char *question);

/// Asks a question and reads a char in response
char *ask_string_question(char *question);

#endif
