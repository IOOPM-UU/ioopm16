# Bra kodexempel på separation/generalisering

På förekommen anledning -- här är några exempel på bra kod som
löser olika problem i uppgiften. 

Flera tycker att det är krångligt att separera t.ex. listan och
programmet. "Vad skall ligga var?!" Listans uppgift är att hålla i
data, men inte veta vad det är för data. Programmet som använder
listan vet inte hur listan är representerad, men vet hur datat
ser ut. 

Nedan går jag igenom två tekniker för att hålla denna separation
ren. Det finns utrymme för båda i uppgiften om man så vill. 


## Visitor-funktion

Vi börjar med en visitor-funktion. En sådan funktion ligger i
listan och drar nytta av att listan vet hur den är representerad.
Funktionen tar som argument en lista, en funktion _f_, samt ett
data _d_ och itererar sedan genom listan och för varje element
_e_, anropar _f(e,d)_. Den kan t.ex. skrivas så här, där
`visit_func` är typen för funktionen _f_:

```c
/// list.h
typedef void(*visit_func)(void *, void *);
void list_visit(list_t *l, visit_func action, void *data);

/// list.c
void list_visit(list_t *l, visit_func action, void *data)
{
  for (node_t *n = l->first; n; n = n->next)
    {
      action(n->element, data);
    }
}
```

Med hjälp av denna funktion blir det nu enkelt att utföra många
operationer som kräver att man går igenom listan. Här är ett
exempel på hur man kan skriva en funktion, `total_amount()` som
givet en databas (träd) och namnet på en vara går igenom samtliga
hyllplatser för varan och summerar deras antal:

```c
/// main.c (eller motsvarande)
int total_amount(db_t *db, char *item_name)
{
  int sum = 0;
  item_t *i = tree_find(item_name);

  if (i)
    {
      /// Här är anropet till list_visit
      list_visit(i->shelves, (visit_func) add_amount, &sum);
    }

  return sum;
}
```

I detta fall använder vi en funktion `add_amount()` som skall
appliceras på varje element i listan. Denna får vid varje anrop in
ett element, samt en pekare till en int (`sum` ovan), och summerar
i `sum`. Så här ser `add_amount()` ut:

```c
/// main.c (eller motsvarande) -- hjälpfunktion
void add_amount(shelf_t *s, int *sum)
{
  *sum += s->amount;
}
```

Vi kan skriva en liknande funktion för att skriva ut alla hyllor, 
och antalet varor på hyllan för något item. Här skickar jag med 
namnet på varan, men kunde lika gärna skriva `NULL` istället för
`i->name` om jag inte behöver den i utskriften. 

```c
/// main.c (eller motsvarande) -- hjälpfunktion
void print_shelf(shelf_t *s, char *item_name)
{
  printf("Shelf %s holds %d %s\n", s->shelf_name, s->amount, item_name);
}

/// main.c (eller motsvarande) 
void print_shelves(item_t *i)
{
  list_visit(i->shelves, (visit_func) print_shelf, i->name);
}
```


## Iterator

En iterator är ett smidigt hjälpmedel för att på ett abstrakt sätt
iterera över elementen i en datasamling, utan att behöva veta hur
elementen är organiserade i datasamlingen. Eftersom koden för
iteratorn ligger i `list.c` kan den enkelt stega framåt med
`next`-pekaren utan att behöva exponera förekomsten av en `node_t`
för användaren. 

Vi utökar `list.h` enligt följande:
```c
/// i list.h
typedef struct iter iter_t;

iter_t list_iterator(list_t *l);
bool iter_has_next(iter_t iter);
void *iter_next(iter_t *iter);
```

Och utökar `list.c` enligt följande:
```c
/// list.c
struct iter
{
  node_t *current;
};

/// Skapar en ny iterator för en given lista
iter_t list_iterator(list_t *l)
{
  return (iter_t) { .current = l->first };
}

/// Kontrollerar om det finns mer att hämta ur listan
bool iter_has_next(iter_t iter)
{
  return iter.current->next != NULL;
}

/// Hämtar nästa element i listan
void *iter_next(iter_t *iter)
{
  void *elem = iter->current->elem;

  iter->current = iter->current->next;

  return elem;
}
```

Observera att iteratorn använder värdesemantik, dvs. den kopieras
runt, och endast när den skall förändras (i `iter_next()`) måste
vi ta emot en pekare till den.

Med hjälp av iteratorn kan vi skriva mycket elegant kod för att
använda den generella listan. Låt oss återbesöka funktionerna
från ovan. Vi kan anväda iteratorn internt i listan för att 
göra koden trevligare:

```c
/// i list.c
void list_visit(list_t *l, visit_func action, void *data)
{
  iter_t iter = list_iterator(l);

  while (iter_has_next(iter))
    {
      action(iter_next(&iter), data);
    }
}
```

Eller varför inte använda iteratorn externt för implementationen
av `total_amount()` direkt?

```c
/// main.c (eller motsvarande)
int totala_amount(db_t *db, char *item_name)
{
  int sum = 0;
  item_t *i = tree_find(item_name);

  if (i)
    {
      /// här är den nya delen
      iter_t iter = list_iterator(l);

      while (iter_has_next(iter))
        {
          item_t *i = iter_next(&iter);
          sum += i->amount;
        }
    }

  return sum;
}
```

Som synes blir implementationen med iteratorn något längre, men
det beror på att iteratorn är mer flexibel och därmed något mer
"primitiv". Eftersom vi måste aktivt välja att gå vidare blir det
lättare att skriva en funktion som t.ex. söker efter ett visst
föremål:

```c
bool uses_shelf(item_t *i, char *shelf_name)
{
  iter_t iter = list_iterator(l);

  while (iter_has_next(iter))
    {
      shelf_t *s = iter_next(&iter);

      if (strcmp(s->name, shelf_name) == 0) return true;
    }

  return false;
}
```

Iteratorgränssnittet kan också utökas med t.ex. insättning och
borttagning. Dessa operationer blir mycket lättare om man ändrar
så att iteratorn inte pekar ut en nod, utan är en pekare på
pekaren till nästa nod.

```c
/// i list.c
struct iter
{
  node_t **current;
};

iter_t list_iterator(list_t *l)
{
  return (iter_t) { .current = &(l->first) };
}

bool iter_has_next(iter_t iter)
{
  return *(iter.current)->next != NULL;
}

void *iter_next(iter_t *iter)
{
  void *elem = *(iter->current)->elem;

  *(iter->current) = &(*(iter->current)->next);

  return elem;
}

/// Ta bort elementet som iteratorn står på
void *iter_remove(iter_t iter)
{
  node_t *next = *(iter.current);
  void *elem = next->elem;

  *(iter.current) = *(iter.current)->next;

  free(next);
  return elem;
}

/// Stoppa in ett element där iteratorn står
void iter_insert(iter_t iter, void *elem)
{
  *(iter.current) = node_new(elem, *(iter.current));
}
```

Med den utökade iteratorn blir det enkelt att skriva funktion som
antingen uppdaterar ett antal i en existerande hyllplats om
hyllplatsen redan var i bruk, eller annars lägger till en ny
hyllplats:

```c
void add_or_update_shelf(item_t *i, char *shelf_name, int amount)
{
  iter_t iter = list_iterator(l);

  while (iter_has_next(iter))
    {
      item_t *i = iter_next(&iter);

      if (strcmp(i->name, shelf_name) == 0)
        {
          i->amount += amount;
          return; // we are done
        }
    }

  // if we get here, we did not update, so we should add

  iter_insert(iter, shelf_new(shelf_name, amount));
}
```

Observera att eftersom iteratorn ligger på stacken så behövs ingen
kod för frigöra den. Väldigt skönt! Dock medför detta att vi måste
skriva `&iter` ibland och `iter` ibland. Detta kan lösas med hjälp
av ett makro. Låt säga att vi döper om `iter_next()` till
`_iter_next()`. Nu kan vi skapa ett makro så här:

```c
/// i list.h
#define iter_next(i) _iter_next(&i)
```

Nu går det bra att skriva `iter_next(iter)` eftersom det översätts
till `_iter_next(&iter)`. Koden blir mer uniform, och användaren 
av iteratorn behöver inte fundera på när överföring skall ske med
värdesemantik respektive med pekarsemantik.
