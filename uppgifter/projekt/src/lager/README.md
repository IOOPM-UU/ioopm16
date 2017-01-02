# Program to test your GC 

Note: lots of things isn't implemented in this program, mostly to
make things simpler for you to integrate with your GC. 

Mostly, there is no shelf support. 

Notably, the program supports multiple steps of undo.

This program is not guaranteed to be bug free, although no bugs
have been added on purpose.


# Building the program

    make db
    
# Running the program

    ./db
    
# Other notes

The file `gc_perf_test.c` is not part of the warehouse program.


## Integration with your GC

If you are having trouble integrating with your GC, try integrating
the program from the leafs downward. For example, start with the
list (`list.c` and `iterator.c`) and then write a small program that
is able to use the list with your GC. Oh, and to save you some time, 
there is already some integration code written for e.g. the list and
iterator, as witnessed by grep:

    list.c:55:  return h_alloc_struct(h, "**i");
    list.c:64:  node *result = h_alloc_struct(h, "**");
    iterator.c:26:  iter *result = h_alloc_struct("*");

Hopefully this integration is correct. If you've changed something in
the specification, please make changes accordingly. 

Note that we're cheating. Code like this is found in e.g. `iterator.c`:

    #ifdef GC_TEST
    extern heap_t *h; // Assume single heap for this program for simplicity
    #endif

If your main program has a global variable `h` which is a pointer
to a heap, you don't have to pass the heap around. This makes it a
lot easier to have one version of the program which supports both
your GC and e.g., malloc. This is a fine simplification, so no
need to change that, even if you (like I do) have a pathological
dislike for global variables. Use macros for controlling whether
to use malloc or your own GC. Here is an example from `iterator.c`:

    iter *iterator_new(void *current)
    {
    #ifdef GC_TEST
      iter *result = h_alloc_struct("*");
    #else
      iter *result = malloc(sizeof(*result));
    #endif
      result->current = (node *)current;
      return result;
    }

