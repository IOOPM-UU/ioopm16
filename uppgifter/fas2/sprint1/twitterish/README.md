# Twitterish

Denna uppgift går ut på att skriva klart ett program som är
halvfärdigt (det går att köra, men saker kan bli bättre).
Programmet heter "Twitterish", och är en slags bastard mellan
Twitter och Facebook. Twitterish består av två program som
samverkar genom nätverket: en server och en eller flera klienter.
Varje användare kör en instans av klientprogrammet, mot samma
server. Endast en instans av servern behövs, och endast användare
uppkopplade mot samma server kan se varandra och varandras poster.

Varje användare har en lista av vänner. Man kan lägga till vem som
helst som vän som är uppkopplad mot samma server. Man kan ta bort
vänner, och man kan välja att ignorera dem.

Meddelanden är korta textsträngar och skickas ut i etern (dvs.
till servern) utan någon specifik mottagare. Servern lagrar alla
meddelanden. (Startar man om servern har den inga meddelanden
eller användare.)

När man hämtar ny information från servern får man en kopia av
alla vänners meddelanden. Dessa visas då i omvänd kronologisk
ordning. När man skriver ut meddelanden skall meddelanden från
ignorerade vänner ignoreras.

Så här kan programmet se ut på klientsidan:

```
 _______       _ _   _            _     _
|__   __|     (_) | | |          (_)   | |
   | |_      ___| |_| |_ ___ _ __ _ ___| |__
   | \ \ /\ / / | __| __/ _ \ '__| / __| '_ \
   | |\ V  V /| | |_| ||  __/ |  | \__ \ | | |
   |_| \_/\_/ |_|\__|\__\___|_|  |_|___/_| |_|

Enter your user id (email address): tobias.wrigstad@it.uu.se
Set your password:
Enter your user name: tobias
Logging in new user tobias.wrigstad@it.uu.se...
Received class Account message
Actions:
[P]ost message   |  [U]pdate feed    |  [A]dd friend     |  [R]emove friend  |
[I]gnore friend  |  [L]ist friends   |  [E]dit account   |  [Q]uit
P
Write your message on a single line:
Hej, hopp!
Message sent
Actions:
[P]ost message   |  [U]pdate feed    |  [A]dd friend     |  [R]emove friend  |
[I]gnore friend  |  [L]ist friends   |  [E]dit account   |  [Q]uit
U
Received class SyncResponse message
tobias says:
Hej, hopp!
```

Och så här ser programmet ut på serversidan för motsvarande körning:

```
!! Server listening for connections: 0.0.0.0/0.0.0.0:8080
!! Server got a connection from: /127.0.0.1:62180
<< Account
!! Server listening for connections: 0.0.0.0/0.0.0.0:8080
>> Received: PostMessage
>> Received: SyncRequest
<< SyncResponse
>> Received: Logout
!! tobias.wrigstad@it.uu.se left the building
```


# Att starta programmet

För att starta programmet behöver du två terminalfönster. Ett för
servern och ett för klienten. Servern staras först, så här:

```
$ java Server
```

Detta startar servern på port 8080. Om denna port redan är
upptagen (t.ex. för att någon annan kör samma server på samma
dator) kan man ange ett annat portnummer:

```
$ java Server 4711
```

Klienten startas så här:

```
$ java Twitterish localhost 8080
```

där `localhost` skall bytas ut mot IP-adressen till den dator där
serven kör (om du inte kör klienten på samma dator som servern, i
så fall kan du låta `localhost` stå!) och porten `8080` skall
matcha serverns port. Alltså, om du har dragit igång servern på
port 4711 och på datorn 131.239.44.12 blir det:

```
$ java Twitterish 131.239.44.12 4711
```



# Klasser i systemet

## Account

Håller information om en användare i systemet: namn, lösenord och
unikt it (epostadress).

## Feed

Alla poster i en klients "feed", dvs. de poster som har visats för
klienten. (Denna klass är inte ordentligt integrerad med systemet.)


## Post

Motsvarar ett meddelande i systemet. Varje post har ett globalt id
(ett löpnummer) som ges av servern. En post är knuten till en
avsändare (ett `Account`-objekt). Postens meddelande är en vanlig
Java-sträng.


## Twitterish

Detta är main-klassen på klientsidan. Den kopplar upp sig mot en
server och skapar sedan en instans av klassen `Client` som är
själv programmet. 


## Client

Här ligger huvudlogiken på klientsidan. Klassen innehåller flera
metoder som direkt motsvarar handlingar i systemet:

* `postMessage()`
* `addFriend()`
* `removeFriend()`
* `ignoreFriend()`
* `quit()`
* `editAccount()`
* `listFriends()`
* `updateFeed()`

Endast en instans av klienten skall finnas per uppkopplad
användare. Klienten har en huvudloop som frågar användaren efter
vad som skall göras.


## Server

Detta är main-klassen på serversidan. När detta program körs
startas en server som lyssnar efter inkommande uppkopplingar. När
en klient kopplar upp sig startas en `ClientProxy` som alltså är
serverns bild av vad klienten har för sig. Det är genom detta
objekt som all interaktion med servern sker.


## ClientProxy

Här ligger huvudlogiken på serversidan. Denna klass har metoder
som motsvarar handlingar på klientsidan: 

* `logout(Account a)` -- loggar ut en klient
* `postMessage(String msg)` -- klienten skickar ett meddelande
* `addFriend(Account a)` -- klienten lägger till en vän
* `removeFriend(Account a)` -- klienten tar bort en vän
* `updateAccount(Account old, Account neu)` -- klienten ändrar sin kontoinformation
* `sync()` -- klienten vill synka sin bild av systemet med serverns bild


## Meddelandeklasser

Följande klasser beskriver meddelanden som skickas mellan servern
och klienterna. Namnen på klasserna ger en bra fingervisning om
vilken typ av meddelande som avses. 

* `AddFriend`
* `RemoveFriend`
* `Login`
* `Logout`
* `PostMessage`
* `SyncRequest`
* `SyncResponse`

Objekt av klassen `PostMessage` innehåller en Java-sträng som
innehåller ett meddelanden från klienten. På server-sidan görs
detta meddelande om till ett `Post`-objekt, med ett globalt
id-nummer, etc.

Objekt av klassen `SyncRequest` avser en begäran från klienten att
få skickat till sig alla nya meddelanden och alla konton som
servern känner till. Dessa kommer tillbaka inuti ett objekt av
klassen `SyncResponse`. 


# Uppgifter

Denna uppgift består av 10 deluppgifter där tanken är att man
skall göra en om dagen under sprinten. (Varje uppgift är
**absolut** inte beräknad att ta en dag att göra.) Det kommer
naturligtvis ta olika tid för olika personer -- vissa blir klara
med allt första dagen, andra tar mer än 10 dagar på sig. Tanken
med 10 deluppgifter är iallafall att försöka dela upp en uppgift i
diskreta delar som kan lösas för sig. 

**Observera att uppgiften går ut på att göra MINST SJU av dessa 10.** 
Man måste alltså inte göra alla 10 uppgifter om man inte vill. 

Deluppgifterna kommer inte i någon särskild ordning nedan. Uppgift
3, 4 och 5 i den ordningen är bra startuppgifter som man kan
använda för att komma in i koden.


## Deluppgift 1: Filtrera meddelanden på servern

Servern känner till vilka användare som finns. De är lagrade som
element i variabeln `knownAccounts` som är en mängd av
`Account`-objekt, "konton". Varje konto motsvarar en användare i
systemet, och varje konto har en lista över den användarens
vänner.

När servern tar emot en `SyncRequest` från en användare svarar den
för närvarande med alla meddelanden och alla användares konton.
Detta är implementerat i metoden `sync()` i klassen `ClientProxy`
på serversidan.

Denna uppgift har två delar som kan lösas separat:

1. De meddelanden som skickas som resultat av en `SyncRequest`
   från användare A skall **inte** innhålla meddelanden från användare
   som inte är vänner med A.
2. De meddelanden som skickas som resultat av en `SyncRequest`
   från användare A skall **inte** innhålla meddelanden äldre än den
   senaste `SyncRequest`:en från A.

Del 2. ovan kan lösas genom att man t.ex. sparar storleken på
`posts` varje gång man tar emot en `SyncRequest`. Nästa gång man
tar emot en `SyncRequest` från samma användare tittar man var
storleken var på `posts` vid tidpunkten för senaste requesten. Om
storleken inte har ändrats finns inga nya meddelanden. Om
storleken har ökat från *N* till *M* är de *M-N* nya meddelandena
som skall skickas. 

Del 1. ovan kan lösas genom att implementera en funktion
`filterMessages()` som tar emot en lista av meddelanden och
returnerar en ny lista med endast de meddelanden vars avsändare
finns i `a`:s vänlista.

Titta på dokumentationen för [listor i Java](https://docs.oracle.com/javase/8/docs/api/java/util/List.html) som visar hur man lägger till och tar bort saker ur en lista.
Det finns också gott om exempel i källkoden för programmet redan. 


## Deluppgift 2: Filtrera meddelanden på klientsidan

Klienten har möjlighet att ignorera vissa vänner. Poster från
vänner som är i "ignorera-listan" skall inte visas. Detta är för
närvarande inte implementerat.

Lösningen till denna uppgift är liknande den för filtrering på
serversidan. Logiken skall läggas i klassen `Feed`, i metoden
`render()`. Tanken bakom att lägga logiken där är att om en
användare ber om att få gamla poster visade igen, skall de vänner
som är i ignorera listan *nu* användas för att avgöra om en post
skall visas eller ej -- inte de som var vid då posten
ursprungligen visades.

Observera att klassen `Feed` inte är korrekt integrerad i
systemet. I funktionen `newPost()` läggs poster in i feeden, men
feeden används inte för att skriva ut, och vad värre är --
`newPost()` anropas aldrig. En lämplig plats att anropa
`newPost()` på är i `sync()`.


## Deluppgift 3: Unignore friend

Man kan förtillfället lägga till vänner i ignorera-listan, men
inte ta bort. Utöka funktionaliteten i programmet med stöd för att
ta bort någon från ignorera-listan. Du behöver ändra i `action()`
i `Client` som ligger i `Twitterish.java`, titta på t.ex. hur
`ignoreFriend()` är implementerad som ledning.


## Deluppgift 4: Stöd för att skriva ut meddelande

För tillfället finns det inte något stöd för att skriva ut alla
meddelanden på skärmen. De skrivs ut som en sido-effekt av att
`syncWithServer()` körs. Ändra så att `syncWithServer()` inte
skriver ut meddelanden på skärmen, och lägg till ett separat
alternativ, `[U]pdate feed`, som skriver ut meddelanden med hjälp
av det `Feed`-objekt som redan finns i klienten och som också är
en del av deluppgiften "Filtrera meddelanden på klienstidan". 

Observera också mängden *delegering* som görs vid utskrifter av en
feed. `myFeed.renderAll()` bygger en sträng med alla meddelanden
genom att be varje meddelande att bygga en sträng av sig själv. 



## Deluppgift 5: Uppdatera namn

Varje konto har ett namn knutet till sig. Detta kan användaren
ändra när hen vill. Eftersom varje uppkopplad klient har sin egen
kopia av alla användares konton uppstår en *inconsistency* varje
gång en användare ändrar sitt namn. Exempel:

Användare A och B är uppkopplade mot servern S. A och B har båda
synkat med servern. A postar ett meddelande M, som skickas till S.
A byter sedan namn till X. Detta medför att A:s konto skickas
till S igen. Nästa gång B synkar med servern visas M, men A står
fortfarande som avsändare -- inte X.

Varje gång en användare synkar med servern skickar servern alla
meddelanden och alla konton. Observera att nya kopior av alla
objekt skickas -- ingenting delas någonsin mellan servern och dess
klienter, och alla objekt som skickas till en klient är nya objekt
som klienten aldrig har sett, även om de kan ha exakt samma
innehåll som objekt som klienten redan har.

En lösning på problemet med inkonsekvens mellan namn är att
ersätta alla gamla konton med nya. Då uppstår ett annat problem,
nämligen att alla vänner som det aktuella kontot (`loggedInUser`)
har pekar på de gamla kopiorna. Detta problem kan lösas genom att
också ersätta det aktuella kontot med ett som kommer från servern,
men observera att det konto som kommer från servern inte vet något
om vilka vänner som för närvarande ignoreras, och inte har något
lösenord.


## Deluppgift 6: Kopiering av lösenord

Varje gång servern skickar en kopia av ett konto till en klient
kopieras all informationen i detta konto, inklusive lösenord.
Lösenordet borde vara en hemlighet mellan servern och varje klient
och inte skickas vidare som en del av informationen om vilka
användare som för närvarande är uppkopplade.

Se till att inte lösenordet skickas vidare till andra klienter.
Detta sker för närvarande i `sync()` i klassen `ClientProxy`,
satsen:

```java
new HashSet<Account>(this.server.getAccounts())
```

skapar en ny mängd med alla konton med i serverns konto-lista.
Kopieringen av dessa sker sedan i metoden `writeObject`.

Ett sätt att lösa denna uppgift är att skapa en egen mängd av
konton, `accountsNoPasswords`, och använda denna istället för
`this.server.getAccounts()` ovan. För att skapa en egen sådan
mängd av konton kan man börja med att skapa en tom mängd:

```java
Set<account> accountsNoPasswords = new HashSet<Account>();
```

och sedan göra `accountsNoPasswords.add(...)` på nya konton som
man skapar själv via en metod `safeCopy()` i klassen `Account` som
man själv måste lägga till. Denna kan i stort sett implementeras
så här:

```java
public safeCopy() {
  return new Account(...); 
}
```

Ett annat sätt att lösa denna uppgift, som är bättre men mer
avancerat, är att skilja `Account` från lösenordet. Flytta
lösenordet till klassen `Login` istället och hantera allt som har
med lösenordet att göra mot denna klass. Denna lösning på
uppgiften medger att konton kopieras fritt eftersom de inte
innehåller något lösenord.

Lösenordet skickas från klienten till servern vid tre tillfällen:

1. Första gången en klient kopplar upp sig mot servern.
2. När en klient som redan har kopplat upp sig tidigare vill
   koppla upp sig igen.
3. När en uppkopplad klient byter sitt lösenord.

I fallen 1 och 2 ovan registreras helt enkelt lösenordet hos
servern. Det görs automatiskt i och med att servern sparar alla
konton, men om man bryter ut lösenord från konton måste lösenordet
sparas på annat sätt.

I fallet 2 ovan skickar klienten ett lösenord till servern som
kontrollerar att det är korrekt.


## Deluppgift 7: Fungerande vänförfrågan

Denna deluppgift är något större och involverar förändringar både
på klienten och servern samtidigre. Hur vänförfrågningar sköts är
för närvarande ganska trasigt. En klient A som känner till en
klient B kan lägga till B som vän, utan att B har något att säga
till om. Vän-konceptet antyder något dubbelriktat, dvs. något mer
i stil med hur vänskap fungerar på t.ex. Facebook.

Denna deluppgift går ut på att implementera vänförfrågningar
enligt följande schema:

1. A skickar en förfrågan till B om att bli vänner till servern.
2. Vid nästa sync från B skickar servern vänförfrågan vidare till B.
   Servern håller en vänförfrågan i 24 timmar. Om B inte synkat med
   servern innan dess kommer A att få ett meddelande om att 
   vänförfrågan inte kunde levereras. Detta meddelande till A hålls
   också i 24 timmar. Om A inte synkar med servern innan dess kastar
   servern bort svaret också, och A får helt sonika börja om. 
3. Om B synkar och får en vänförfrågan från A skall/får B svara 
   ja eller nej. B kan direkt flagga A som vän vid ja. Svaret skickas 
   till servern för leverans till A vid nästa synk. A och B läggs också
   till i varandras respektive vänlistor. Om A inte synkar
   inom 24 timmar kastas svaret och A (B) tas bort ut B:s (A:s) vänlista på
   servern så att A försvinner ut B-klientens vänlista vid nästa
   synk. 

Denna deluppgift kräver att man implementerar några nya klasser:

* `FriendRequest` (A vill bli vän med B)
* `FriendRequestResponse` (B svarar ja/nej på vänförfrågan från A)

Servern måste också spara vilka vänförfrågningar som är pågående
just nu så att den kan hålla koll på när de påbörjades för att
kasta bort förfrågningarna när de legat för länge (se ovan). Ett
sätt att göra detta är att skapa en ny klass
`OutstandingFriendRequest` som sparar en starttid, vem som frågar
och vem som blir tillfrågad, och om svar har inkommit, och vad det
i så fall var. Man kan spara en lista av sådana objekt på servern,
Varje gång en klient (t.ex. A) gör en synk kan servern gå igenom
listan av vänförfrågningar för att se om något har hänt -- t.ex.
om B har svarat ja på A:s tidigare vänförfrågan, eller om en
helt orelaterad förfrågan har legat för länge.

Nedan är några diagram i ASCII-notation som visar interaktionen
som beskrevs ovan på ett mer översiktligt vis:

Framgångrik vänförfrågan:

```
                    A                           Server                         B
                    |                             |                            |
                    | Can A befriend B?           |                            |
                    +---------------------------->|                            |
                    |                             |                            |
                    |                             | Can A befriend B?          |
                    |                             +--------------------------->|
                    |                             |                            |
                    |                             |                            | Update friendships
                    |                             | Yes                        |
                    |                             |<---------------------------+
                    |                             |                            |
                    | Yes                         |                            |
                    |<----------------------------+                            |
                    |                             |                            |
 Update friendships |                             |                            |
                    |                             |                            |
                    |                             |                            |
```

Att säga nej till vänförfrågan:

```
                    A                           Server                         B
                    |                             |                            |
                    | Can A befriend B?           |                            |
                    +---------------------------->|                            |
                    |                             |                            |
                    |                             | Can A befriend B?          |
                    |                             +--------------------------->|
                    |                             |                            |
                    |                             |                            | 
                    |                             | No                         |
                    |                             |<---------------------------+
                    |                             |                            |
                    | No                          |                            |
                    |<----------------------------+                            |
                    |                             |                            |
                    |                             |                            |
                    |                             |                            |
```

Vänförfrågan some inte lyckas i tid:

```
                    A                           Server                         B
                    |                             |                            |
                    |                             | Disconnect                 |
                    |                             |<---------------------------+
                    |                             |                            |
                    |                             |                            |
                    | Can A befriend B?           |                            |
                    +---------------------------->|                            |
                    |                             | Store request              |
                    |                             |                            |
                    
                                          ... 24 h passes

                    | sync                        |                            |
                    +---------------------------->|                            |
                    |                             | Go through requests        |
                    |                             |                            |
                    | Request timeout             |                            |
                    |<----------------------------+                            |
                    |                             |                            |
                    |                             |                            |
                    |                             |                            |

```


## Deluppgift 8: Kommentarer

Skall vara möjligt att ge kommentarer.

Denna deluppgift går ut på att lägga till stöd för att
*kommentera* och *gilla* poster i programmet. Det kan tänkas
se ut så här:

```
Broman säger:
En gentleman är en man som kan spela dragspel, men avstår.
(17 personer gillar detta)

    |  Sigge:
    |  +1
    |
    |  Gunilla:
    |  Vad är det för fel på dragspel, det är ju halva Kivik?!
    '--------------------
```

Du får själv bestämma hur gränssnittet för att lägga in
kommentarer skall se ut. T.ex. så här:

1. Användaren får välja att kommentera ett meddelande
2. Alla meddelande listas i en numrerad lista
3. Användaren väljer vad som skall kommenteras genom att mata in ett nummer
4. Användaren mata in sin kommentar
5. Skärmen ritas om med kommentarer

(Och motsvarande för att gilla.)

Implementationen av detta kan göras t.ex. genom att man utökar
`Post`-klassen med en lista av kommentarer. Dessa kan antingen
vara `Post`-objekt vars kommentarlistor alltid är tomma (alt. --
implementera stöd för nästlade kommentarer, etc.) eller så kan man
skapa en särskild klass för kommentarer, t.ex. `Comment` med
erforderlig information, som avsändare, etc. Alla meddelanden och
poster **skall** ha ett globalt id. Om du använder en separat
`Comment`-klass skall denna få globala id:n på samma sätt som
`Post`-objekt.

Gillanden kan med fördel implementeras som en räknare i
`Post`-klassen (alltså, det är inget krav på att kunna visa vem
som gillar vad). För att skicka ett gillande från klienten till
servern kan man t.ex. använda en separat `Like`-klass (alt.
använda `Comment`-klassen). Använd det globala post id:t för att
identifiera vilket meddelande som en kommentar/ett gillande avser.

Du kan med fördel ändra i `render()` i `Post` så för att skriva
ut kommentarerna som en del av att posten skrivs ut. 


## Deluppgift 9: Throttling 

Throttling innebär att man begränsar tillgången till någon resurs,
t.ex. nätverk eller CPU. I detta fall vill vi begränsa antalet
gånger en klient får skicka meddelanden till servern under en
given tidsperiod. Detta skall implementeras i två steg, dels på
servern och dels på klienten.

Om klienten märker att den kommer att skicka mer än 10 meddelanden
per sekund, eller fler än 60 meddelanden per minut skall den sluta
skicka meddelandena och istället lägga dem i en kö efter vilket
klienten skall skicka ett meddelande i sekunden tills kön är tom.
Om kön växer till fler än 3600 element skall klienten avslutas.

En trevlig implementation gör detta i en separat tråd, men
eftersom vi inte går igenom trådbegreppet på denna kurs i någon
större detalj är det inte på något sätt ett krav.

På serversidan vill vi också monitorera hur ivrigt klienter
skickar meddelanden. Servern skall logga ut klienter som bryter
mot volymreglerna ovan (tolkningsutrymme medges). För att undvika
att servern går in i en oändlig loop och spyr ut meddelanden på
nätverket skall servern avsluta sig själv om den märker att den
skickar mer än 1000 meddelanden per 1 minut.

Vidare skall servern inte tillåta fler än 64 samtidiga uppkopplade
klienter.


## Deluppgift 10: Refaktorera och Dokumentera koden med JavaDoc

Som ett led i att förstå koden, gå igenom den och skriv
JavaDoc-kommentarer för varje metod och klass och generera
HTML-dokumentation.

I samband med detta, gör ett refaktoreringspass för att se om det
inte går att hitta bättre namn på klasser och variabler.


## Rekommenderade mål på nivå tre

1. [Inluppsmål 4](http://auportal.herokuapp.com/achievements/103)
2. [A2](http://auportal.herokuapp.com/achievements/2) - Fundera på
   vad varje klass behöver veta om de andra klasserna de använder.
   Hur hade du skrivit motsvarande kod i C?
3. [E11](http://auportal.herokuapp.com/achievements/11) - Används
   redan på ett flertal ställen i koden. 
4. [G16](http://auportal.herokuapp.com/achievements/16) - Vilka
   attribut (om några) ska vara synliga utifrån? Jämför att ha
   privata attribut med getters och setters med att bara ha
   publika attribut.
5. [H19](http://auportal.herokuapp.com/achievements/19) - kommer in
   direkt på grund av alla kopior av allt, t.ex. när vi vill ändra namn.
6. [N40](http://auportal.herokuapp.com/achievements/40) - Vad
   händer när vi kompilerar vårt program? Vilka filer skapas? Vad
   händer om man byter ut någon av filerna och kör programmet? Hur
   skulle motsvarande program (med samma modularisering) fungera i
   C?

## Rekommenderade mål på nivå fyra/fem

1. [H21](http://auportal.herokuapp.com/achievements/21) - Om du
   refaktorerar meddelandeklasserna till en klasshierarki. (Detta
   gås igenom i nästa sprint!)
2. [G17](http://auportal.herokuapp.com/achievements/17) - Används 
   redan. Finns det fler möjligheter?
3. [I24](http://auportal.herokuapp.com/achievements/24) och [I25](http://auportal.herokuapp.com/achievements/25) - finns mycket som kan gå fel i detta program.
4. [J28](http://auportal.herokuapp.com/achievements/28) - När
   frigörs kund-objekten i programmet?
