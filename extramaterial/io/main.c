#include "reading.h"

// Kompilera med gcc -Wall main.c reading.c -o main
// Kör sedan med ./main

int main(int argc, char *argv[])
{
  printf("'%d'\n", ask_int_question("Give me an int: "));
  printf("'%s'\n", ask_string_question("Give me a string:"));
  return 0;
}
