# Frivillig extrauppgift: Primtalshåv med PThreads

Denna uppgift går ut på att göra en parallell implementation av
"Eratosthenes håv" ([Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes))
i C med hjälp av ptrådar ([Pthreads](https://computing.llnl.gov/tutorials/pthreads/)).
Uppgiften rekommenderas till dig som inte har problem med att
själv förkovra dig i trådning och pthreads.

Eratosthenes håv är en algoritm för att räkna fram alla primtal
mellan 2 och ett givet tal T. Först beskriver vi den sekventiella
implementationen innan vi pratar om hur algoritmen kan
parallelliseras.

**Notera:** Implementationen av den sekventiella håven är ganska
enkel -- några futtiga rader. Du kommer inte att skriva så många
rader kod i denna uppgift, svårigheten ligger i att koordinera
flera trådar, och i att optimera och mäta.


## Eratosthenes sekventiella håv

Låt `A` vara en array av booleans med längden `T`. För programmet
gäller att `A[i] == true` betyder att `i+1` är ett primtal. Vid
programmets början är `A[i] == true` för `0 < i <= T-1`.

Låt `i` vara en indexvariabel som vi använder för att stega oss
igenom arrayen. I början av programmet är `i == 0`.

För att ta fram alla primtal mellan 2 och T, gör:

1. Stega fram `i` tills `A[i] == true`, eller `i == T`.
   * `i == T`, gå till 4.
   * `A[i] == true`, `i+1` är nu förklarat som ett primtal.
2. Sätt alla multipler (alltså `2 * i`, `3 * i`, etc.) av `i` i `A` till `false`.
3. Gå till 1.
4. Gå igenom `A[i]` från `i == 0` till `i == T-1` och skriv ut `i+1` för alla `A[i] == true`.


## En enkel optimering

Steg 4 ovan går igenom hela arrayen en gång till. Pröva om det går
att göra programmet snabbare genom att skriva ut eller på annat
sätt spara undan alla primtal för utskrift utan att man måste gå
igenom hela arrayen igen.

Använd UNIX-programmet `time` för att mäta programmets körtid. T.ex.

```
$ time --verbose ./my-sieve
    Command being timed: "./my-sieve"
    User time (seconds): 4.07
    System time (seconds): 0.09
    Percent of CPU this job got: 100%
    Elapsed (wall clock) time (h:mm:ss or m:ss): 0:04.16
    Average shared text size (kbytes): 0
    Average unshared data size (kbytes): 0
    Average stack size (kbytes): 0
    Average total size (kbytes): 0
    Maximum resident set size (kbytes): 642465792
    Average resident set size (kbytes): 0
    Major (requiring I/O) page faults: 0
    Minor (reclaiming a frame) page faults: 39312
    Voluntary context switches: 0
    Involuntary context switches: 1086
    Swaps: 0
    File system inputs: 0
    File system outputs: 0
    Socket messages sent: 0
    Socket messages received: 0
    Signals delivered: 0
    Page size (bytes): 4096
    Exit status: 0
```

De viktigaste bitarna i outputen ovan är:

```
    User time (seconds): 4.07
    System time (seconds): 0.09
    Percent of CPU this job got: 100%
    Elapsed (wall clock) time (h:mm:ss or m:ss): 0:04.16
```

Ta reda på hur du skall tolka denna output. Du kan börja t.ex.
[här](https://en.wikipedia.org/wiki/Time_(Unix)).


## En något mer komplicerad optimering?

En array av booleaner använder `sizeof(bool)` *bytes* att spara
något som ryms i en enskild *bit*. För att kunna räkna fram
riktigt stora primtal kan vi använda en mer effektiv
representation: en så-kallad bitvektor (bitmanipulering, se
föreläsning 9).

Eftersom C tillåter manipulering av enskilda bitar i en byte är
det ganska enkelt att skriva en datastruktur som fungerar precis
som en array, men som manipulerar bitar istället för bytes.

Att manipulera enskilda bitar är i regel något mer invecklat än
att manipulera enskilda bytes, och kan därför introducera extra
steg i beräkningen. Å andra sidan blir minnesåtkomsterna mer
effektiva för mindre primtal, och det blir möjligt att räkna fram
större primtal på grund av den mer kompakta representationen.

Använd UNIX-programmet `time` för att mäta körtiden för ett
program som använder en bitvektor istället för en array av
booleans.


## Eratosthenes parallella håv

Det finns många sätt att parallellisera denna algoritm. Ett enkelt
sätt är detta:

Dela upp arrayen `A` i `W` delar, där `W` är antalet trådar i
programmet. Varje tråd `T1`--`TW` skall arbeta på en egen del av
arrayen. `T1` är den "vänstraste delen", dvs. startar med `i == 0`.

`T1` utför steg 1--3 i den sekventiella algoritmen på sin del av
arrayen med ett utökat steg 2: utöver att sätta alla multipler av
i till `false` skall alla andra trådar `T2`--`TW` få reda på att `i` är
ett primtal så att de kan sätta alla multipler av `i` till `false` i
sina delar av arrayen.

Exempel: vi har 10 trådar och `A` har intervallet [0..99]. Tråd 1 har
då intervallet 0..9 och när 2 förklaras som primtal skall trådarna
2--9 få i uppgift att sätta alla multipler av två (10, 12, ... i
`T2`; 20, 22, ... i `T3`, etc.) till `false` i arrayen.

Hur kommunikationen mellan trådarna ska gå till väljer du själv.
Du kan antingen låta trådarna `T2`--`TW` jobba parallellt med
`T1`, eller vänta tills `T1` är helt klar innan de andra startar.
I det första fallet måste det finnas något sätt att signalera när
nästa primtal är redo så att ingen tråd försöker läsa ett värde
för tidigt.

Notera att för att partitioneringen av arrayen [0,N] skall fungera
så måste den del som hanteras av `T1`, [O,X] vara sådan att X*X > N.
(Varför?)

När Tråd 1 når `i == 10` och övriga trådar är klara med att göra
sina uppgifter kan vi utföra steg 4. på hela arrayen. **Notera att
från och med nu är vi inte intresserade av att skriva ut
primtalen, bara skriva ut hur många de är.** Detta är för att vi
ska slippa ha med den sekventiella (och beräkningsmässigt
ointressanta) utskriften när vi mäter prestanda. Tänk på att
modifiera den sekventiella versionen på detta sätt också så att
jämförelsen blir rättvis!

Använd UNIX-programmet `time` för att mäta körtiden för den
parallella primtalshåven. För vilka värden på T går den
sekventiella versionen fortare? Hur mycket snabbare är en lösning
som använder 2 trådar? 4? 8? **Notera att du inte vinner något på
att ha fler trådar i ditt program än du har kärnor i din dator.
Institutionen har flera datorer med 16 kärnor som du kan logga in
på och pröva på.**



## Optimeringar

Den parallella algoritmen ovan har två steg: det första är
parallellt, och det andra (steg 4. på hela arrayen) är
sekventiellt. Vi vill gärna dra nytta av parallellism även i det
sista steget.

Ett sätt att göra det är att låta varje tråd utföra steg 4. på sin
del av arrayen, dvs. för varje del av arrayen räkna fram antalet
primtal. Sedan kan man låta en tråd summera resultaten. Spara alla
olika versioner av programmet så att du kan se vilka val som ger
bäst prestanda för olika antal trådar.


## Mäta prestanda

Vad prestanda är är inte helt enkelt. Man talar t.ex. om
*throughput* -- hur mycket arbete utfört per tidsenhet -- och
*latency* -- hur lång tid tar det innan man får (del)svar.

I detta fall är througput ett mer relevant mått än latency och kan
mätas som hur lång tid det tar att hitta alla primtal i ett visst
intervall.

Om tiden för att hitta alla primtal i [0,N] med en tråd är 2T, och
tiden för två trådar är T säger vi att vi har en faktor 2 speedup
(2X speedup). Det är ovanligt att program *skalar* så bra. Om vi
håller storleken på indatan konstant (alltså [0,N]) och ökar
antalet trådar och mäter körtiden mäter vi **strong scalability**.

Om vi dubblar storleken på indatan samtidigt som vi dubblar
antalet trådar (notera, inte fler trådar än kärnor i maskinen!)
och mäter körtiden mäter vi **weak scalability**.

Mät både strong och weak scalability för din parallella
primtalshåv för både stora och små (vad betyder det?) värden på N
och för 1, 2, 4, 8 och 16 trådar (du hittar information på
[institutionens webb](http://www.it.uu.se/datordrift/maskinpark)
om maskinerna).

Rita en graf med datat i ett lämpligt program (t.ex. gnuplot, R,
Numbers.app (OSX), Excel (OSX, Windows)). Du kan välja t.ex. att
ha körtiden på Y-axeln och antal trådar (kärnor) på X-axeln och en
graf per storlek på indata. Ett annat alternativ är att ange
prestanda på Y-axeln i relation till prestanda för den
sekventiella algoritmen. om den sekventiella algoritmen
tar 12 sekunder och den parallella tar 4 sekunder för 2 kärnor och
6 sekunder för 4 kärnor får vi ungefär detta i de olika utförandena:

```
time (sec)                       speedup
14 |                              5 |
12 |  x                           4 |
10 |                              3 |      x
 8 |                              2 |          x
 6 |          x                   1 |  x
 4 |      x                         '---------------- cores
 2 |                                   1   2   4
 0 |
   '---------------- cores
      1   2   4
```

En vanlig metod för att motverka det jitter som uppstår naturligt
på grund av aktiviteter i resten av systemet är att göra varje
körning många gångar, t.ex. 10, och ta bort den sämsta och den
bästa, och sedan ta snittet av de resterande. Om man skall mäta på
den dator vars övriga belastning man inte har kontroll över kan
det vara bra att ta reda på den aktuella belastningen innan man
mäter. Du kan använda t.ex. UNIX-programmet `top` för det.


## Enhetstester

Testning av parallella program är så invecklat att det finns en
alldeles särskild kurs om detta på senare årskurser (valbar på år
4/5). Här skall vi endast ägna oss åt att testa programmets
sekventiella delar.

Du måste skriva enhetstester för alla funktioner i ditt program.
Om du t.ex. har en funktion som givet ett tal `i` och en array `A`
av längden `l` gör `A[j] = false` för alla `j` som är multipler av
`i` i [0,l] skall det finnas ett test som prövar detta.


## Automatisering

Att automatisera byggning och testning i en makefil är viktigt
smidigt. Vidare skall du också automatisera prestandamätningarna.
Din makefil skall ha ett direktiv `make bench` som kör alla
versioner av alla program med alla konfigurationer (storlek på
indata, antal trådar) och sparar resultatet. Du kan med fördel
använda shell scripting (eller varför inte rikare skriptspråk som
Python, Ruby eller Perl?) för detta ändamål.

Här är ett litet Rubyprogram som kör alla program i arrayen
`programs` med alla parametrar i `parameters`. Raden som faktiskt
kör program är utkommenterad just nu, istället skrivs kommandona
ut på skärmen. Det är relativt enkelt att utöka programmet så
att det också kör t.ex. "time" och sparar resultatet på fil.

```ruby
cores = [1, 2, 4, 8]
input = cores.collect { |c| c * 1024 * 1024 }
parameters = cores.zip(input)
programs = ["./ps"]

programs.each do |prg|
  parameters.each do |params|
    # Ta bort kommentartecknet # framför raden nedan för att
    # utföra kommandon, inte bara skriva ut dem på skärmen
    # `#{prg} #{params.join(' ')}`
    puts("#{prg} #{params.join(' ')}")
  end
end
```

Här är motsvarande program i Python, som ni stiftade bekantskap med på
introkursen för ett drygt år sedan.

```python
import commands

cores = [1, 2, 4, 8]
input = [c * 1024 * 1024 for c in cores]
parameters = zip(cores, input)
programs = ["./ps"]

for prg in programs:
  for params in parameters:
    cmd = "%(p)s %(c)i %(i)i" % { 'p' : prg, 'c' : params[0], 'i' : params[1] }
    # commands.getstatusoutput(cmd) # kör kommandot
    print cmd                       # skriver ut kommandot på terminalen
```


# Utökningar

1. Implementera stöd för att hitta alla primtal för väldigt stora
   arrayer. Det kan du göra med ett stratifierat angrepssätt -- om
   arrayen har storleken [0, 10N] kan du börja med att räkna ut
   [0,N], spara alla primtal i en "lista" och sedan arbeta på
   [N,2N] med primtalen i listan som input.
2. Pröva att inte partitionera arrayen utan istället låta alla
   trådar arbeta från 0 och framåt och markera multipler av olika
   primtal. Detta kräver någon form av koordination för att
   undvika att två trådar arbetar med samma primtal. Jämför
   prestandan -- är detta en bättre eller en sämre lösning?
   Experimentera med olika storlekar och olika antal trådar
   som vanligt.
3. Att man inte kan få ut mer prestanda av att använda fler trådar
   än det finns fysiska kärnor är sant i teorin, men inte
   nödvändigtvis i praktiken. Om man kör på ett system med
   *time-sharing*, där flera användare (och trådar) delar på samma
   hårdvaruresurser kan man ibland se en liten fortsatt
   prestandaökning när man ökar antalet trådar förbi antalet
   kärnor. Fundera på varför det är så, och prova om det gäller
   för ditt program.
4. Utöka skripten som kör prestandamätningarna till att inte
   bara samla in kördata (t.ex. spara på fil) utan också sammanställa
   datat så att output från `make bench` är alla siffror som behövs
   och inga filer har skapats.
5. Utöka skripten som kör prestandamätningarna till att också
   skapa de grafer som efterfrågats. Du kan med fördel använda
   `gnuplot` för detta, läs t.ex. [här](http://mathewpeet.org/computing/software/gnuplot/tutorial2/).


# Förslag på 4/5-mål att redovisa

3. [O43](http://auportal.herokuapp.com/achievements/43) -- duger bra att bara titta på det sekventiella för detta
