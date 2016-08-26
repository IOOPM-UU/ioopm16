# Introduktionslaboration i Java-programmering

Målsättningen med denna uppgift är att komma igång med Java och
bekanta sig med några grundläggande Java-idiom. Du kan inte
redovisa mål med den här uppgiften, och inte heller använda den
för att redovisa Z103 eller Z104.

## Die.java

I filen [`Die.java`](Die.java) ligger en klass som representerar
en tärning. Kompilera programmet med kommandot `javac Die.java`.
Detta skapar en fil i den aktuella katalogen; använd `ls` för att
ta reda på dess namn. Prova sedan att köra programmet med
kommandot `java Die`.

Här kommer en kort beskrivning av koden i filen:

```java
import java.util.Scanner;
```

För att kunna använda klassen `Scanner` måste vi importera den
från biblioteket `java.util`. Jämför med C där vi skrev `#include <stdio.h>`
för att komma åt funktionerna i biblioteket för I/O.

```java
public class Die {
```

Här börjar själva klassen `Die`. Nyckelordet `public` betyder här
att klassen är synlig för alla

```java
  private int numberOfSides;
  private int value;
```

Det här är klassens *attribut* (eng. attributes) eller *fält*
(eng. fields). Det är den interna data som representerar objektet
självt. Jämför med motsvarande strukt i C:

  ```c
  struct die {
    int number_of_sides;
    int value;
  }
  ```

Nyckelordet `private` betyder att attributen inte går att läsa
utifrån. Den som håller i en tärning kan alltså inte se direkt hur
många sidor den har eller vilket värde som är uppåt (men det går
att fråga tärningen om dess värde, se nedan om metoder). Jämför
med C om `struct die` inte var definierad i `.h`-filen.

```java
  public Die() {
      numberOfSides = 6;
  }

  public Die(int _numberOfSides) {
      numberOfSides = _numberOfSides;
  }
```

Det här är tärningens två konstruktorer, metoderna som anropas när
ett nytt objekt skapas. Notera att det inte finns någon returtyp,
vilket är hur man känner igen en konstruktor. Anropet `new Die()`
kommer att köra den första konstruktorn (och alltså sätta antalet
sidor till 6), medan `new Die(42)` kommer att köra den andra. När
det finns flera konstruktorer så här pratar man om *överlagrade*
konstruktorer.

```java
  public int roll() {
    return value =  (int) (Math.random()*numberOfSides) + 1;
  }

  public int get() {
      return value;
  }
```

Här är tärningsklassens två metoder. `roll` slår tärningen och
returnerar det resulterande värdet, medan `get` returnerar värdet
utan att slå tärningen. Notera att båda metoderna är `public` och
alltså kan anropas av vem som helst.

Notera också hur attributen beskriver vad ett objekt *är*, medan
metoderna beskriver vad ett objekt *kan göra* (En tärning har ett
antal sidor och ett värde. Man kan slå en tärning och läsa av dess
värde). Konstruktorerna beskriver hur ett nytt objekt skapas.

```java
  public static void main(String [] args) {
      Scanner sc = new Scanner(System.in);
      System.out.print("Number of sides: ");
      int sides = sc.nextInt()
      Die d = new Die(sides);
      System.out.println("Alea iacta est: " + d.roll());
  }
}
```

Det här är klassens `main`-metod och den som körs när man skriver
`java Die`. Alla `main`-metoder måste ha precis den här
signaturen. Nyckelordet `static` betyder att metoden hör till
*klassen* och inte till *varje enskilt objekt*. Varje tärning har
ett eget antal sidor och ett värde, men det finns bara en enda
`main`-metod oavsett hur många tärningar som skapas.

## Fortsätt själv

1. Skriv en annan klass med namnet `MyDieTest` (i en fil som heter
   `MyDieTest.java` i samma katalog). Klassen skall (endast)
   innehålla en `main`-metod som frågar användaren om önskat antal
   sidor och sedan skapar ett sådant `Die`-objekt, slår det 10
   gånger och beräknar och skriver ut summan i terminalen. Se
   `Die.java` för exempel på hur man skriver en `main`-metod och
   läser data från terminalen.
2. Följande kod är nonsenskod som inte producerar ett vettigt
   resultat (försök att fundera ut varför innan du skriver ett
   program som utför uttrycken nedan och ser resultatet):

   ```
   Die die = new Die();
   System.out.println(die.get());
   ```

   Vad är problemet/felet? Ändra i `Die.java` så att ovanstående
   kod blir meningsfull ("vettig")!
3. Följande kod är heller inte vettig:

   ```
   Die die = new Die(-12);
   ```

   Ändra i `Die.java` så att det inte går att skapa tärningsobjekt
   med ett orimligt antal sidor!
4. Vad ger koden

   ```
   Die d = new Die();
   System.out.println(d);
   ```

   för utskrift? Lägg till följande metod i Die-klassen:

   ```
   public String toString() {
       return "Die(" + value + ")";
   }
   ```

   och se vad det då blir för utskrift. Fundera över varför!

5. Lägg till en metod med signaturen `boolean equals(Die otherDie)`
som returnerar `true` om tärningarna "är likadana" (vad betyder
det?), annars `false`. Tänk också på skillnaden mellan "samma
tärning" och "likadana tärningar".

6. Skriv en klass `PairOfDice` som representerar ett tärningspar.
Klassen skall använda sig av ("aggregera" med OO-terminologi)
klassen `Die`, d.v.s. den skall ha två attribut av typen `Die` och
metoderna i `PairOfDice` skall använda metoder i klassen `Die`.
Operationer som skall finnas:
  * Skapa ett tärningspar med givet antal sidor (samma för båda
  tärningar),
  * slå ett tärningspar,
  * avläsa varje individuell tärning samt
  * en `toString`-metod.

7. Gå till
   [online-dokumentationen](https://docs.oracle.com/javase/8/docs/api/)
   för Javas klass-API (JDK) och skumma dokumentationen för
   `String`-klassen. Läs om `compareTo`-metoden och skriv sedan
   ett program som läser in två namn i strängform (med hjälp av
   `Scanner`-klassen) och skriver ut dem i bokstavsordning.

8. *Frivilligt, men rekommenderas (Speciellt till dig som redan
    kan programmera Java)!* - Skriv ett lämpligt driver-program
    som kapslar in nedanstående kod i en main-metod, och förklara
    beteendet (ledning: Tänk på skillnaden mellan identitet och
    ekvivalens!).

    ```java
    String a = "Beefheart";
    String b = "Beefheart";
    String c = "heart";
    String d = "Beef" + c;
    int i = 1;
    int j = 1;
    Integer k = 2;
    Integer l = i + j;
    System.out.println(a == b);
    System.out.println(a.equals(b));
    System.out.println(a == d);
    System.out.println(a.equals(d));
    System.out.println(i == j);
    System.out.println(k == i + j);
    System.out.println(k.equals(i+j));
    System.out.println(k == l);
    System.out.println(k.equals(l));
    ```
## Om arv

Nedanstående övning blandar in arv i uppgiften. Du kan vänta med
den här delen tills arv har tagits upp på en föreläsning.

9. Skriv en klass `ColoredDie` som representerar en tärning som har
   en viss färg (låt färgen representeras som en sträng). Låt den
   ärva av klassen Die. Vilka metoder bör du specialisera (eng.
   "override")? Vilka nya metoder är vettiga att ha? Ändra sedan i
   klassen `PairOfDice` så att en av de skapade tärningarna är röd.
10. Metoden `equals` i steg 5 ovan har problemet att den bara kan
   anropas med en tärning som argument. Med andra argument anropas
   istället motsvarande metod ur klassen `Object` (alla klasser
   ärver av `Object`), och den metoden kanske inte gör vad vi
   vill. För att specialisera `equals` *måste* den ha typen
   `boolean equals(Object other)`. Skriv om `equals` så att den
   kan anropas med argument av godtycklig typ. Du kan använda
   operatorn `instanceof` för att ta reda på om ett värde är (en
   subtyp av) en viss klass (men det här är en av få vettiga
   tillfällen att använda `instanceof`!).