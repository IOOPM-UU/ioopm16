Inlupp 2: Utökat lagerhanteringssystem
========================================

Utökningen av lagerhanteringssystemet omfattar flera
olika steg:

1. Använda C:s standardbibliotek (filer, sortering via dynamiska
   arrayer)
2. Stöd för persistens på fil (alltså spara databasen till disk
   och läsa upp den igen)
3. Skriv enhetstester för programmet
4. Generalisering av list och träd-modulerna från förra inlämningsuppgiften
5. Integration med kod från annat kodpar på kursen
6. Vi skall tänka på koddokumentation

En bra idé är att börja med 4 följt av 5 eftersom 5 kräver
interaktion med ett annat kodpar vilket ofta medför några dagars
väntan.


## Användning av C:s standardbibliotek

Vi har använt C:s standardbibliotek förut (`malloc`, `free`,
strängmanipulering, etc.). Nu skall vi utöka detta med användning
av funktioner för filmanipulering. Du kan använda `man stdio` för
att se standardfunktioner för I/O. Vi kommer att behöva ganska få
av dessa: förmodligen räcker det med öppna, läsa, skriva och
stänga. Läs vidare under **Persistens på fil** nedan. Notera att det
finns olika funktioner för att läsa `int`:ar, strängar, etc.

Vidare skall vi använda oss av C:s inbyggda stöd för sortering för
att balansera binärträdet som håller i varudatabasen. Du kan
använda `man qsort` för att läsa om C:s implementation av
quicksort-algoritmen.

Balansering av binärträdet sker på följande sätt:

1. Dumpa trädet i preorder ned i en array. Du kan använda
   `tree_size()` för att ta reda på hur många element som skall
   finnas i arrayen. Du kan också med fördel använda en generell
   preorder-funktion som tar en funktionspekare för att dumpa
   trädet ned i arrayen.
2. Skapa ett nytt träd och fyll det med element från arrayen i
   rätt ordning.

Nedan finns ett litet Python-program som bygger upp trädet från en
sorterad array. Notationen [:] nedan betyder att om `a = [1,2,3,4,5]`
så är `a[middle] == 3`, `a[:middle] == [1,2]` och `a[middle+1:] == [4,5]`.

```python
def build_tree_from_array(a):
    middle = len(a) / 2
    tree_insert(a[middle])
    if middle > 0:
        build_tree_from_array(a[:middle])
    if middle > 1:
        build_tree_from_array(a[middle+1:])
```

Balansering av trädet skall göras tillgänglig från huvudloopen:

    [S]ortera databasen


## Funktionen för att ta bort en vara

Nu är det dags att implementera stöd för att ta bort en vara. När
detta är gjort kan du också implementera stöd för att redigera en
varas namn!

När man tar bort en vara tar man endast bort en enskild lagerplats
för varan. **När samtliga lagerplatser för en vara har tagits bort
skall också varan tas bort ur trädet.**

Exempel:

    Vad vill du göra idag? <T>

    Ta bort vara
    ============

    1. Kritor
    2. Hushållspapper
    3. Minecraftsvärd

    Välj vara att ta bort: <3>

    Minecraftsvärd finns lagrat på följande platser:

    1. A88 (100 stycken)
    2. B2 (150 stycken)
    3. C11 (350 stycken)

    Vilken plats skall tas bort (0 för ingen)? <3>


## Ångra

Det är frivilligt att implementera stöd för att ångra funktionerna
ovan. Valet ångra skall dock bara finnas synligt om det finns
något att ångra/går att ångra något.



## Generalisering av datastrukturerna

I förra versionen av lagerhanteringen bytte vi lagringsformat från
en statisk array till dynamiska träd och listor. Nu skall vi
generalisera träden och listorna på ett sådant sätt att det går
att använda dem i godtyckliga C-program.

Det betyder att du måste ändra på datastrukturerna på följande
sätt:

* Nycklar och poster i trädet/element i listan måste vara av typen
  `void *`
* All jämförelse eller operation på data av typen `void *` måste
  ske med hjälp av funktionspekare

För att arbeta med funktionspekare är `typedef` ovärdeligt för att
hålla koden läsbar. Följande C-kod deklarerar typen `cmp_t` som en
funktion som tar två pekare av okänd typ (`void *`) och returnerar
en `int`. Tanken är att funktionen skall fungera som
biblioteksfunktionen `strcmp` som jämför strängar. Låt `x = strcmp(a,b)`.
Om `x < 0` är `a` före `b` i aktuell sorteringsordning. Om `x > 0` är `b`
före `a`. Om `x == 0` är `a` och `b` exvivalenta.

```c
/// @returns like strcmp
typedef int(*cmp_t)(void *, void *);
```

Nu kan vi modifiera funktionen som skapar en lista till att ta en pekare till en
funktion för att jämföra elementen:

```c
list_t *mk_list(cmp_t *cmp)
{
  list_t *result = malloc(sizeof(list_t));
  if (result)
  {
    *result = { .cmp_f = cmp };
  }
  return result;
}
```

Och sedan använda denna funktion när vi behöver jämföra element,
t.ex. när vi vill göra insättning i sorteringsordning:

```c
void list_insert(list_t *l, void *elem)
{
  node_t **c = &(l->first);
  while (*c && l->cmp_f((*c)->elem, elem)
    {
      c = &((*c)->next);
    }
  *c = mk_node(elem, *c);
}
```

**TIPS!**

Om listan/trädet behöver kopiera element behövs en funktion för
det. Om listan/trädet behöver ta bort ett element ur minnet behövs
en funktion för det (eftersom elementet kan innehålla pekare till
andra strukturer som måste frigöras explicit). Och så vidare.

**Koden för listan eller trädet får inte innehålla någon som helst
kod som är specifik för lagerhanteraren!**

Om du t.ex. behöver skriva ut alla element i en lista kan du lösa
det på två sätt:

1. Implementera en extern iterator
2. Implementera en intern iterator

En extern iterator är ett protokoll som låter en användare vandra
igenom en datastruktur. Så här:

```c
iter_t *iter = list_iterator(mylist);
while (iter_has_next(iter))
  {
    T *elem = (T *) iter_next(iter);
    ... // print elem
  }
```

Här är magin att vi vet att listan innehåller element av typen `T`
så att vi kan typomvandla resultatet av `iter_next`. Iteratorer
tas upp på lektion 2.

En intern iterator tar en funktionspekare och applicerar den på
alla element i turordning:

```c
void list_iterate(list_t *l, iter_t *f)
{
  for (elem_t *c = l->first; c; c = c->next)
    {
        f(c->elem);
    }
}
```

Här är `iter_t` en typ för en funktion som tar en `void *` och
inte returnerar något utan bara har sido-effekted. T.ex. kunde
vi ha en funktion:

```c
void print_element(void *p)
{
  T *elem = p;
  ... // print elem
}
```

som kan användas så här:

```c
list_iterate(mylist, print_element);
```

**När ni är klara med denna del, flytta trädet och listan egna
kataloger som inte ligger tillsammans med kod från
lagerhanteringssystemet.**


## Integration med andras kod

Denna del av uppgiften går ut på att uppleva integration med kod
som man inte själv har skrivit. För denna uppgift skall ni para
ihop er med ett annat kodpar på kursen. Använd Piazza för att
hitta ett annat par.

Nu skall ni byta träd och lista med varandra. Ni klonar (eller
forkar) deras repository och kompilerar och länkar mot den koden
istället för er egen list/trädkod. Eftersom listans gränssnitt var
givet samma till alla torde detta vara lätt att byta ut. Jämför
det med utbyte av trädet vars gränssnitt inte var
"standardiserat".

Använd deras dokumentation och headerfiler men försök undvika att
läsa deras kod. Programmera mot *gränssnittet* som det är tänkt
att man skall göra -- man har inte alltid tillgång till koden. Med
mindre än att ni kan påvisa buggar i det andra kodparets kod får
ni **INTE** ändra deras kod -- ni måste anpassa ert program att
fungera med de nya träd- och listmodulerna. Använd issue trackern
på GitHub för att få det andra kodparet att laga sina fel.

Skriv en kort rapport om era erfarenheter som en issue på GitHub
för det andra kodparets repository.

* Hur stora var skillnaderna mellan implementationerna?
* Var dokumentationen välskriven och användbar?
* Även om gränssnitten var lika, fanns det ibland skillnader i
  beteenden?
* Hur upplevde ni kvaliteten på namn i den andra koden?
* Saknades någon funktion?
* Fanns det något som var extra bra i den andra koden som saknades
  i er?


## Enhetstester

Nu skall vi på allvar enhetstesta vår kod (se föreläsningen om
testning). Egentligen skall all kod enhetstestas, men det räcker
med en delmängd i detta fall.

**Alla** funktioner i list- och trädmodulerna skall testas. Det
kommer att vara bra att ha när ni skall använda kod från ett annat
par. Vidare skall ni testa åtminstone *alla funktioner* som
används för att lägga till och ta bort en vara ur lagrets databas,
samt undo-funktionen. Med alla funktioner avses här alla
funktioner som ni har skrivit i ert program som som anropas i
samband med tillägg av vara, etc.

**Observera:** Ni skall inte testa funktioner som läser in data
från användaren. Om det är omöjligt att göra det (för att det är
inläsningar överallt ... hm ...) behöver ert program
*refaktoreras*, så att inläsningslogik och applikationslogik
separeras från varandra. Exempelvis, om det finns en `add_goods`
som ser ut så här (pseudokod):

```python
def add_goods(DB):
  name = input_string("Namn: ")
  desc = input_string("Beskrivning: ")
  cost = input_int("Pris: ")
  # logik för att kolla att indatat är OK (utelämnad här)
  ware = new_ware(name, desc, cost)
  if ask_save_true():
    DB.add_ware(ware)
```

så skall den ändras till två funktioner:

```python
def add_goods_ui(DB):
  name = input_string("Namn: ")
  desc = input_string("Beskrivning: ")
  cost = input_int("Pris: ")
  if ask_save_true():
    add_goods(DB, name, desc, cost)

def add_goods(DB, name, desc, cost):
  # logik för att kolla att indatat är OK (utelämnad här)
  ware = new_ware(name, desc, cost)
  DB.add_ware(ware)
```

Här ser vi att alla inläsningar (`input_...` och `ask_...`) är i
funktionen som heter `add_goods_ui`. Denna funktion behöver inte
testas -- det räcker med den där logiken för att kolla att indatat
är OK ligger, tillsammans med den faktiska tilläggningen. 

Skriv ett enhetstest för varje funktion, alltså testa exempelvis
skapa ny lista, stoppa in och ta bort separat -- alltså i tre
olika test.

Börja varje test med att skapa en ny "ren" testsituation. I fallet
den länkade listan handlar det alltså om att skapa en ny lista det
första vi gör i varje test. Syftet är att minska de möjliga
felkällorna när vi sedan skall leta efter varför ett test inte
passerar. I slutet av varje test skall vi också ta bort alla
temporära data.






## Persistens på fil

Nu skall vi lägga till stöd för att spara databasen på fil och
läsa upp den igen. Börja med att bekanta dig med hur funktionerna
`fopen`, `fread` och `fclose` fungerar, lämpligen genom att
implementera en egen version av UNIX-programmet `cat` (skriv `man
cat` om du inte känner till programmet).

Programmet skall ta som kommandoradsargument, namnet på den fil
som innehåller databasen. Om denna fil inte finns skall den skapas
och databasen skall vara tom. När programmet avslutas skall
databasen sparas ned på filen så att den kan läsas upp igen nästa
gång programmet startar. Det finns inga kommandon i programmet för
att spara eller ladda databasen -- det sker automatiskt.

För att spara datat i databasen har du två val:

1. Spara datat som radorienterad text där varje vara tar upp 4
   rader (namn, beskrivning, pris, AL -- antal lagerplatser) och
   en lagerplats tar upp två rader (lagerplats, kvantitet).
   Inläsning läser först 4 rader och skapar en post i databasen
   från den. Sedan läser man AL * 2 rader för att läsa in
   lagerplatserna och stoppar in dem för den aktuella varan i
   databasen. När man når `EOF` -- end of file -- är man klar.
2. Definiera ett binärt filformat på motsvarande sätt. Ett hyfsat
   bra filformat är detta:

```c
int siz;             // antal bytes i posten
int name_siz;        // antal tecken i nästföljande sträng
char[name_siz] name; // namnet
int desc_siz;        // antal tecken i nästföljande sträng
char[desc_siz] desc; // beskrivningen
int price;           // pris
int storage_siz;     // antal lagerplatser
lagerplats_t storage[storage_siz]; // lagerplatserna
```

Egentligen är `storage_siz` onödig, men det blir något enklare på
detta vis.

Man kan till och med tänka sig att själva filen startar med en
`int` som berättar hur många poster den innehåller, men det går
förstås också bra att leta efter `EOF` -- end of file.


## Namngivning och refaktorering

När du skrev den förra lagerhanteraren var ditt fokus kanske på
att få programmet att fungera. Det är lika viktigt att programmet
fungerar som att det är **läsbart** och **förändringsbart**.

Gå igenom ditt program och titta på alla namn. Vad heter
funktioner? De bör ha namn som förklarar vad de gör t.ex.
`save_product_to_db` eller `ask_yes_no_question`. Detsamma gäller
för variabler! Variabelnamnet `amount` är mindre informativt än
`number_of_fish` såvida det inte är fullständigt uppenbart från
det kontext där `amount` används att det är antalet *fiskar* som
avses.

En bra tumregel är att ju längre variablernas räckvidd är -- desto
längre namn skall de ha. Globala variabler bör alltså ha långa
namn, och en loopvariabel för ett index in i en array kan heta
`i`.

Sök också igenom ditt program efter repetitioner. Om du ser att
samma följd av instruktioner förekommer flera gånger för att
åstadkomma samma sak men kanske med olika input är det läge att
skapa en funktion för det. Till exempel, om denna kod förekommer
på flera ställen:

```c
int merch_id = ask_selection("Välj vara", db);
bool remove = ask_yes_no_question("Ta bort vara?");
if (remove)
  {
     db_remove(db, merch_id);
  }
```

Kan man tänka sig att skapa funktionen `remove_merch_from_db`:

```c
void remove_merch_from_db(db_t *db)
{
  // koden ovan
}
```

och anropa den. Då blir det enklare att ändra på interaktionen med
databasen eftersom man bara behöver ändra på ett ställe.

Ofta kan det vara så att man använder olika parameterar på olika
platser i koden, t.ex. olika frågesträngar till `ask_selection`.
Då kan de bli extra parametrar till funktionen:

```c
void remove_merch_from_db(db_t *db, char *query, char *conf_query)
{
  int merch_id = ask_selection(query, db);
  bool remove = ask_yes_no_question(conf_query);
  if (remove)
    {
       db_remove(db, merch_id);
    }
}
```


## Koddokumentation

Använd verktyget doxygen för att dokumentera din kod. Du kan börja
med att titta på exempel i headerfilen för den länkade listan.
Målet med koddokumentation är att förklara för en användare av
*koden* (inte programmet) hur den skall användas.

Tänk dig att vi plötsligt skulle byta ut din implementation av
lagerhanteraren mot en annan grupps implementation, som du aldrig
har sett. Förmodligen är många funktioner väldigt lika, men det
finns säkert också stora skillnader. Funktioner som du aldrig har
sett blir betydligt enklare att använda om de har en beskrivande
text och funktioner som du också har, men som skiljer sig litet,
är ännu viktigare att ha dokumentation för eftersom din
förförståelse för hur de fungerar kommer att vara fel.

Dokumentation är med fördel kort för enkla saker och längre för
komplexa saker. För komplexa funktioner kan man t.ex. ha ett
exempel på hur man kan använda den för att göra något.

Tänk på att dokumentation kostar! När programmet ändras måste
dokumentationen också ändras för att fortfarande vara korrekt.
Felaktig dokumentation är en klassisk felkälla.




# Åtta möjliga utökningar

1. Möjlighet att spara och ladda en annan databasfil under körning
   av programmet
2. Automatisk balansering av databasträdet istället för att
   användaren måste göra det manuellt
3. Utöka makefilen med ett test-direktiv som kör alla tester
4. Använd ett "coverage-verktyg" typ `gcov` för att ta reda på hur
   stor del av er kod testerna testar
5. Konfigurera GitHub för att köra enhetstester automatiskt när
   kod checkas in
6. Använd listmodul och trädmodul från två olika kodpar
7. Utöka lagerplatsinformation med datum -- om datumet är före
   dagens datum finns varan, annars avser datumet
8. Tillämpa defensiv programmering på hela programmet (se
   föreläsning 12)



# Förslag på 3-mål att redovisa

1. [Inluppsmål 2](http://auportal.herokuapp.com/achievements/101)
2. [E10](http://auportal.herokuapp.com/achievements/10) -- om du implementerar databasen t.ex. med ett binärt
   sökträd eller en länkad lista som är kan lagra godtyckligt data
2. [F13](http://auportal.herokuapp.com/achievements/13) -- finns flera lämpliga delar av programmet där man t.ex. letar efter data eller räknar ut totalsumman
3. [I22](http://auportal.herokuapp.com/achievements/22) -- indata både från användaren och andra delar av
   programmet att tänka på
3. [J26](http://auportal.herokuapp.com/achievements/26) -- vad behöver sparas på heapen, och varför?
4. [D9](http://auportal.herokuapp.com/achievements/9) -- ett direkt krav ovan
7. Fortsätt med [C7](http://auportal.herokuapp.com/achievements/7) -- du vill inte behöva efterskapa saker sedan (**Du kan alltså inte redovisa detta redan nu.**)


# Förslag på 4/5-mål att redovisa

1. [A3](http://auportal.herokuapp.com/achievements/3) -- om du delar upp programmet i olika moduler som antytts
   ovan
1. [K31](http://auportal.herokuapp.com/achievements/31) -- motivera din modularisering
2. [M39](http://auportal.herokuapp.com/achievements/39) -- vid insättning/uttag ur länkade strukturer
2. [P47](http://auportal.herokuapp.com/achievements/47) -- hjälp en annan grupp förbättra sin kod med en pull request
3. [Q50](http://auportal.herokuapp.com/achievements/50) -- försök avgöra hur bra dina tester är
