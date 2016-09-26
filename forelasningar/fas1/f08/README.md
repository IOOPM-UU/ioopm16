Här är lite kod från dagens föreläsning, nu uppdaterad med
void-pekare. Observera att detta inte är ett komplett exempel.

Vi utgick från ett träd enligt följande (nu uppdaterad för
void-pekare):

I .h-filen har vi följande typ:

```c
/// Något för användaren att hålla i
typedef struct tree tree_t;

/// Jämförelsefunktionen
typedef int(*cmp_func)(void *, void *);
```

I .c-filen återfinns resten:

```c
/// Noderna i trädet
typedef struct node node_t;

struct tree
{
  node_t *root;
  cmp_func cmp;
};
```
En uppdaterad tree_new() som också tar emot en funktionspekare
till jämförelsefunktionen som sparas i trädet:
```c
tree_t *tree_new(cmp_func cmp)
{
  assert(cmp); 

  tree_t *t = calloc(1, sizeof(struct tree));
  t->cmp = cmp;
}
```
De nya funktionerna för att hitta något som finns i trädet.
Observera att find() returnerar en pekare till den plats i
trädet där vi återfinner den nod som håller det värde vi
letar efter -- om den finns. Finns den inte returnerar find
platsen där denna nod borde ha funnits. Vi kan alltså använda
find både för att söka och sätta in saker i trädet!
```c
node_t **find(node_t **n, int k, cmp_func cmp)
{
  if (*n == NULL) return n;

  int key_comparison = cmp(k, (*n)->key);
  
  if (key_comparison < 0)
    {
      return find(&((*n)->left), k, cmp);
    }
  else if (key_comparison > 0)
    {
      return find(&((*n)->right), k, cmp);
    }
  else
    {
      return n;
    }
}
```
Och så här använder vi den:
```c
bool tree_insert(tree_t *t, void *k, void *v)
{
  node_t **n_ptr = find(n, k, t->cmp);

  if (*n_ptr == NULL)
    {
      *n_ptr = node_new(k, v);
      return true;
    }
  else
    {
      return false;
    }
}
```
Det som är speciellt med denna kod -- i jämförelse med vad vi
tidigare sett på kursen, är användandet av pekare till pekare.

Funktionen find() pekar opererar på en pekare _i_ datastrukturen
-- inte en pekare _till_ någon nod i datastrukturen. I och med att
vi hela tiden pekar ut en plats där en pekare finns har vi *både*
möjlighet att läsa pekaren och därmed komma åt värdet, eller
skriva pekare, och därmed ändra i datastrukturen.

`t->root` är och `n->right` (el. `left`) har typen `node_t *` där
`t` är en `tree_t *` och `n` en `node_t *`. De är pekare till
noder i datastrukturen.

`&(t->root)` och `&(n->right)` är av typen `node_t **`. De är
pekare till fält i noder i datastrukturen som pekare ut andra
noder i datastrukturen, ev. NULL.

Om `n` har typen `node_t *` så är `n->key` nyckeln och &(n->key)
adressen den plats där `n`:s pekare till dess nyckel är sparad.

Om `n` har typen `node_t **` så är `(*n)->key` nyckeln och
&((*n)->key) adressen den plats där `n`:s pekare till dess nyckel
är sparad.

