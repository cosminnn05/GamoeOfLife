# Proiect Game of Life - Analiză Cod

Acest document descrie funcționalitatea codului C furnizat pentru proiectul "Game of Life". Proiectul este împărțit în mai multe task-uri, fiecare cu obiective specifice.

## Structura Generală a Codului

Codul implementează diverse aspecte ale jocului "Conway's Game of Life", de la simularea simplă la stocarea optimizată a generațiilor și analiza configurațiilor de celule.

**Definiții Globale:**
*   `ALIVE ('X')`: Caracterul folosit pentru a reprezenta o celulă vie.
*   `DEAD ('+')`: Caracterul folosit pentru a reprezenta o celulă moartă.

**Structuri de Date Principale:**
*   `Celula`:
    ```c
    typedef struct {
        int linie;
        int coloana;
    } Celula;
    ```
    Reprezintă coordonatele (linie, coloană) ale unei celule pe grilă.

*   `Nod` (pentru Task 2):
    ```c
    typedef struct Nod {
        Celula *diferente; // Array de celule care și-au schimbat starea
        int count_dif;     // Numărul de celule din array-ul 'diferente'
        struct Nod *urm;   // Pointer către următorul nod (folosit pentru stivă)
    } Nod;
    ```
    Folosit pentru a stoca diferențele (celulele care își schimbă starea) între o generație și următoarea, sub formă de listă înlănțuită (stivă).

*   `Nod3` (pentru Task 3 și 4):
    ```c
    typedef struct Nod3 {
        char **matrice;     // Matricea (starea grilei) la un moment dat
        struct Nod3 *left;  // Pointer către copilul stâng în arbore
        struct Nod3 *right; // Pointer către copilul drept în arbore
    } Nod3;
    ```
    Folosit pentru a construi un arbore binar unde fiecare nod conține o configurație a grilei.

**Funcții Utilitare Comune:**
*   `citireMatrice(const char *filename, char ***grid, int *N, int *M, int *K, int *task)`:
    *   Citește din fișierul `filename` numărul task-ului, dimensiunile grilei (N, M), numărul de generații de simulat (K) și starea inițială a grilei.
    *   Alocă dinamic memorie pentru grilă.
*   `scriereMatrice(const char *filename, char **grid, int N, int M)`:
    *   Scrie starea curentă a grilei `grid` (N linii, M coloane) în fișierul `filename`.
    *   Adaugă o linie goală după fiecare matrice pentru separare vizuală.
*   `countAliveNeighbors(char **grid, int N, int M, int x, int y)`:
    *   Calculează și returnează numărul de vecini vii ai celulei de la coordonatele `(x, y)` pe grila `grid`. Verifică cei 8 vecini posibili (vecinătate Moore).

---

## Task 1: Simulare Standard Game of Life

**Scopul Task-ului:**
Simularea clasică a jocului "Game of Life" pentru un număr dat de pași (K) și afișarea stării grilei la fiecare pas.

**Funcții Principale Implicate:**
*   `citireMatrice()`: Pentru a încărca starea inițială.
*   `scriereMatrice()`: Pentru a scrie starea grilei după fiecare generație.
*   `nextGenerationWithDiffs()`: Deși această funcție este proiectată pentru Task 2 (pentru a returna și diferențele), Task 1 o folosește pentru a calcula următoarea generație. Diferențele returnate sunt ignorate și eliberate imediat.
*   `countAliveNeighbors()`: Utilizată indirect de `nextGenerationWithDiffs()`.

**Descriere Detaliată:**
1.  Programul citește datele de intrare (task = 1, dimensiuni N, M, număr de pași K, și grila inițială) folosind `citireMatrice`.
2.  Grila inițială este scrisă în fișierul de output folosind `scriereMatrice`.
3.  Se intră într-o buclă care se execută de K ori:
    a.  Se calculează starea următoarei generații folosind `nextGenerationWithDiffs()`. Această funcție aplică regulile standard ale Game of Life:
        *   O celulă vie cu 2 sau 3 vecini vii rămâne vie.
        *   O celulă moartă cu exact 3 vecini vii devine vie.
        *   În toate celelalte cazuri, celula devine (sau rămâne) moartă.
    b.  Funcția `nextGenerationWithDiffs()` creează intern o nouă grilă (`newGrid`), o populează conform regulilor, apoi copiază conținutul `newGrid` înapoi în `grid`-ul original. `newGrid` este apoi eliberată.
    c.  Deși `nextGenerationWithDiffs()` returnează un array de celule care și-au schimbat starea, acest array este eliberat imediat (`free(dif)`) deoarece Task 1 nu necesită stocarea acestor diferențe.
    d.  Noua stare a grilei (`grid`) este scrisă în fișierul de output.
4.  După K pași, simularea se încheie.

**Output:**
Fișierul de output va conține starea inițială a grilei, urmată de stările grilei după fiecare dintre cele K generații simulate, fiecare separată de o linie goală.

---

## Task 2: Optimizarea Stocării Generațiilor

**Scopul Task-ului:**
În loc să se stocheze întreaga grilă la fiecare pas, se stochează doar celulele care își schimbă starea între o generație și următoarea. Aceste diferențe sunt salvate într-o stivă.

**Funcții Principale Implicate:**
*   `nextGenerationWithDiffs(char **grid, int N, int M, int *numDif)`:
    *   Similar cu funcția de bază pentru generarea următoarei stări, dar în plus identifică și colectează celulele care își schimbă starea (din vie în moartă sau invers).
    *   Returnează un array de structuri `Celula` (`dif`) conținând coordonatele acestor celule și actualizează `*numDif` cu numărul lor.
*   `push(Nod **varf, Celula *dif, int count_dif)`:
    *   Adaugă un nou `Nod` în vârful stivei (reprezentată de `*varf`).
    *   Noul nod stochează array-ul de diferențe `dif` și numărul acestora `count_dif`.
*   `scrieDiferente(const char *filename, Nod *top)`:
    *   Scrie în fișierul `filename` diferențele stocate în stiva `top`.
    *   Pentru a scrie diferențele în ordine cronologică (de la generația 1 la K), stiva originală este mai întâi inversată într-o stivă temporară (`rev`), apoi parcursă.
    *   Formatul output-ului este: `numar_generatie diferenta1_linie diferenta1_coloana diferenta2_linie diferenta2_coloana ...`

**Descriere Detaliată:**
1.  Programul citește datele de intrare (task = 2, N, M, K, grila inițială).
2.  Se inițializează o stivă `stiva` ca fiind goală (`NULL`).
3.  Se intră într-o buclă care se execută de K ori:
    a.  Se calculează următoarea generație și, simultan, se colectează celulele care și-au schimbat starea folosind `nextGenerationWithDiffs()`. Funcția returnează array-ul `dif` și numărul `nrDif`.
    b.  Array-ul `dif` și `nrDif` sunt adăugate în vârful stivei `stiva` folosind funcția `push()`. Astfel, diferențele de la ultima generație (K) vor fi în vârful stivei, iar cele de la prima generație (1) vor fi la bază.
4.  După K pași, funcția `scrieDiferente()` este apelată:
    a.  Creează o stivă temporară `rev`.
    b.  Parcurge stiva originală `stiva` de la vârf la bază și adaugă fiecare element în `rev`. Astfel, `rev` va conține diferențele în ordine cronologică (generația 1 în vârf, K la bază).
    c.  Parcurge stiva `rev`, scriind pentru fiecare nod numărul generației (un contor `temp` incrementat) și coordonatele celulelor din `it->diferente`.
    d.  Eliberează memoria alocată pentru stiva temporară `rev`.
5.  La final, stiva originală `stiva` este eliberată (inclusiv array-urile `diferente` din fiecare nod).

**Output:**
Fișierul de output va conține, pentru fiecare dintre cele K generații simulate, o linie cu numărul generației urmat de perechi de coordonate (linie, coloană) pentru fiecare celulă care și-a schimbat starea în acea tranziție.

---

## Task 2 Bonus: Reconstrucția Stării Inițiale

**Scopul Task-ului:**
Folosind stiva de diferențe creată la Task 2, se reconstruiește starea grilei de la generația K înapoi la starea inițială (generația 0).

**Funcții Principale Implicate:**
*   `reverseGeneration(char **grid, int N, int M, Nod *top)`:
    *   Primește grila în starea ei de la generația K și stiva `top` (care are diferențele pentru tranziția K-1 -> K în vârf).
    *   Iterează prin stivă de la vârf la bază. Pentru fiecare set de diferențe (corespunzând unei tranziții `i-1 -> i`):
        *   Pentru fiecare celulă din `curent->diferente`, inversează starea ei în `grid`. Dacă era `ALIVE`, devine `DEAD`, și invers.
    *   Aplicând aceste inversări în ordine invers cronologică (de la K la 1), grila este transformată pas cu pas înapoi la starea anterioară, ajungând la starea inițială.

**Descriere Detaliată:**
1.  Acest task se execută ca o continuare a Task-ului 2, după ce stiva `stiva` a fost populată și diferențele au fost scrise în fișier.
2.  Grila `grid` se află în starea corespunzătoare generației K.
3.  Se apelează `reverseGeneration(grid, N, M, stiva)`.
4.  Funcția parcurge stiva `stiva`. La fiecare nod (care reprezintă schimbările dintre generația `i-1` și `i`), ea iterează prin array-ul `diferente`. Pentru fiecare `Celula` din acest array, starea corespunzătoare din `grid` este inversată.
    *   Exemplu: Dacă stiva este (de la vârf la bază): [Diferențe G2->G3], [Diferențe G1->G2], [Diferențe G0->G1].
    *   Inițial, `grid` este G3.
    *   Se procesează [Diferențe G2->G3]: se inversează celulele schimbate pentru a obține G2.
    *   Se procesează [Diferențe G1->G2]: se inversează celulele schimbate pentru a obține G1.
    *   Se procesează [Diferențe G0->G1]: se inversează celulele schimbate pentru a obține G0.
5.  După ce funcția se încheie, grila `grid` va conține starea inițială.
6.  Această matrice reconstruită este afișată la consolă (STDOUT).

**Output (Bonus):**
Pe lângă fișierul generat de Task 2, se afișează la consolă matricea inițială reconstruită.

---

## Task 3: Arborele Generațiilor

**Scopul Task-ului:**
Construirea unui arbore binar unde fiecare nod reprezintă o stare a grilei. De la fiecare nod, se pot genera două stări copil:
*   Copilul stâng: aplicând o regulă specifică "B".
*   Copilul dreapta: aplicând regula standard Game of Life.
Arborele este construit până la o adâncime K. Stările grilelor din arbore sunt apoi scrise în fișier în parcurgere preordine.

**Funcții Principale Implicate:**
*   `copyMatrix(char **sursa, int N, int M)`: Creează o copie profundă a unei matrici.
*   `applyRuleB(char **grid, int N, int M)`:
    *   Aplică o regulă alternativă: o celulă devine `ALIVE` dacă are exact 2 vecini vii. Altfel, își păstrează starea curentă.
    *   Returnează o nouă matrice cu starea rezultată.
*   `applyStandardRule(char **grid, int N, int M)`:
    *   Aplică regulile standard Game of Life.
    *   Returnează o nouă matrice cu starea rezultată.
*   `createTree(char **matrice, int N, int M, int nivelCurent, int nivelMaxim)`:
    *   Funcție recursivă care construiește arborele.
    *   Creează un nod `Nod3`, copiază `matrice` în el.
    *   Dacă `nivelCurent < nivelMaxim`:
        *   Generează `leftMatrix` folosind `applyRuleB`.
        *   Generează `rightMatrix` folosind `applyStandardRule`.
        *   Apelează recursiv `createTree` pentru a construi subarborele stâng și drept.
        *   Eliberează `leftMatrix` și `rightMatrix` (deoarece copiii lor le-au copiat).
    *   Returnează nodul creat.
*   `scriePreordine(FILE *file, Nod3 *nod, int N, int M)`:
    *   Parcurge arborele în preordine (Rădăcină, Stânga, Dreapta).
    *   Pentru fiecare nod vizitat, scrie matricea stocată în el în fișier.
*   `freeTree(Nod3 *nod, int N)`: Eliberează recursiv memoria alocată pentru întregul arbore.

**Descriere Detaliată:**
1.  Programul citește datele de intrare (task = 3, N, M, K = adâncimea maximă a arborelui, grila inițială).
2.  Se apelează `createTree(grid, N, M, 0, K)` pentru a construi arborele. Rădăcina arborelui (nivel 0) va conține grila inițială.
3.  Funcția `createTree` construiește arborele recursiv:
    a.  La fiecare nod, starea curentă a grilei este copiată.
    b.  Dacă nu s-a atins adâncimea maximă K:
        i.  Se generează o nouă grilă (`leftMatrix`) aplicând `applyRuleB` pe grila nodului curent. Aceasta devine baza pentru subarborele stâng.
        ii. Se generează o nouă grilă (`rightMatrix`) aplicând `applyStandardRule` pe grila nodului curent. Aceasta devine baza pentru subarborele drept.
        iii.Se fac apeluri recursive pentru a crea copiii stâng și drept.
4.  După ce arborele este complet construit, se deschide fișierul de output și se apelează `scriePreordine(out, root, N, M)`.
5.  `scriePreordine` scrie matricea din nodul curent, apoi vizitează recursiv subarborele stâng, apoi subarborele drept.
6.  La final, se apelează `freeTree(root, N)` pentru a elibera toată memoria alocată pentru nodurile arborelui și matricile stocate în ele.

**Output:**
Fișierul de output va conține matricile (stările grilelor) din nodurile arborelui, listate în ordinea unei parcurgeri în preordine. Fiecare matrice este separată de următoarea printr-o linie goală.

---

## Task 4: Căutarea Lanțului Hamiltonian în Stările Arborelui

**Scopul Task-ului:**
Pentru fiecare stare a grilei (fiecare matrice) stocată în arborele construit la Task 3, se caută cel mai lung lanț Hamiltonian format din celulele vii adiacente. Un lanț Hamiltonian este un drum într-un graf care vizitează fiecare nod exact o dată. Aici, "nodurile" sunt celulele vii, iar "muchiile" există între celule vii adiacente (vecinătate Moore).

**Structuri și Funcții Auxiliare pentru Task 4:**
*   `VecinAux`:
    ```c
    typedef struct {
        int index;     // Indexul vecinului în lista de celule vii
        Celula coord;  // Coordonatele vecinului
    } VecinAux;
    ```
    Folosită pentru a sorta vecinii unei celule vii în `dfsHamiltonian`.
*   `compareCells(const void *a, const void *b)`: Funcție de comparație pentru `qsort`, sortează `Celula` lexicografic (întâi după linie, apoi după coloană).
*   `cmpVecinAux(const void *a, const void *b)`: Funcție de comparație pentru `qsort`, sortează `VecinAux` pe baza coordonatelor celulelor.
*   `dfsComponenta(int **adj, int *visited, int node, int n, int *componenta, int *size)`:
    *   Efectuează o parcurgere DFS (Depth First Search) pentru a găsi toate nodurile (celulele vii) dintr-o componentă conexă, pornind de la `node`.
    *   Stochează indicii nodurilor componentei în `componenta` și actualizează `*size`.
*   `dfsHamiltonian(int **adj, int *visited, int *path, int adancime, int n, int *maxLenght, int *bestPath, int *foundFull, Celula *cells, int *component)`:
    *   Funcție recursivă DFS pentru a găsi cel mai lung lanț Hamiltonian (sau un lanț Hamiltonian complet dacă există) într-o componentă conexă dată.
    *   `adj`: Matricea de adiacență pentru componenta curentă.
    *   `path`: Lanțul curent în construcție.
    *   `adancime`: Lungimea curentă a lanțului.
    *   `n`: Numărul de noduri în componenta curentă.
    *   `maxLenght`, `bestPath`: Stochează cel mai lung lanț găsit până acum.
    *   `foundFull`: Flag setat la 1 dacă s-a găsit un lanț care acoperă toate nodurile componentei.
    *   `cells`: Array-ul global al tuturor celulelor vii din grilă (pentru a obține coordonatele).
    *   `component`: Array cu indicii (din `cells`) nodurilor din componenta curentă.
    *   **Logic:**
        1.  Dacă s-a găsit un lanț complet (`adancime == n`), se actualizează `maxLenght`, `bestPath` și se setează `foundFull`.
        2.  Se construiește o listă cu vecinii nevizitați ai nodului curent din `path`.
        3.  Vecinii sunt sortați lexicografic folosind `cmpVecinAux` (prioritizează celulele cu linie mai mică, apoi coloană mai mică).
        4.  Pentru fiecare vecin sortat, se adaugă la `path`, se marchează ca vizitat și se apelează recursiv `dfsHamiltonian`.
*   `processNodeHamiltonian(FILE *file, char **matrice, int N, int M)`:
    *   Procesează o singură matrice (stare a grilei).
    1.  Extrage toate celulele vii (`vii`) din `matrice` și le sortează lexicografic.
    2.  Dacă nu există celule vii, scrie "-1" și returnează.
    3.  Construiește o matrice de adiacență (`adj`) pentru celulele vii: `adj[i][j] = 1` dacă celula `vii[i]` și `vii[j]` sunt adiacente (vecinătate Moore).
    4.  Găsește toate componentele conexe ale grafului celulelor vii folosind `dfsComponenta`.
    5.  Pentru fiecare componentă conexă:
        a.  Construiește o sub-matrice de adiacență (`adjC`) specifică componentei.
        b.  Caută cel mai lung lanț Hamiltonian în această componentă folosind `dfsHamiltonian`, pornind căutarea de la fiecare nod al componentei.
        c.  Se reține cel mai lung lanț (`maxL`, `localBest`). Dacă un lanț complet (`n` noduri) este găsit, căutarea pentru acea componentă se oprește (`foundFull`).
    6.  Compară lanțurile găsite în toate componentele:
        a.  Se alege cel cu lungimea maximă (`bestLenght`).
        b.  Dacă mai multe componente au lanțuri de aceeași lungime maximă, se alege cel care este lexicografic mai mic (comparând coordonatele celulelor din lanț, pas cu pas).
    7.  Scrie în fișier lungimea drumului (numărul de muchii = `bestLenght - 1`) și coordonatele celulelor din cel mai bun lanț Hamiltonian găsit pentru acea matrice.
*   `parcurgereArboreTask4(FILE *file, Nod3 *nod, int N, int M)`:
    *   Parcurge arborele în preordine.
    *   Pentru fiecare nod (matrice) din arbore, apelează `processNodeHamiltonian` pentru a analiza și scrie rezultatul.

**Descriere Detaliată:**
1.  Programul citește datele de intrare (task = 4, N, M, K, grila inițială).
2.  Construiește același arbore de generații ca la Task 3, folosind `createTree()`.
3.  Se deschide fișierul de output.
4.  Se apelează `parcurgereArboreTask4(out, root, N, M)`.
5.  Această funcție va parcurge arborele în preordine. Pentru fiecare `nod->matrice`:
    a.  Se apelează `processNodeHamiltonian()`.
    b.  Aceasta găsește celulele vii, construiește graful lor de adiacență, identifică componentele conexe.
    c.  Pentru fiecare componentă conexă, încearcă să găsească cel mai lung lanț Hamiltonian. Strategia de explorare în `dfsHamiltonian` (sortarea vecinilor) asigură că, dacă există mai multe lanțuri de aceeași lungime maximă, se alege cel lexicografic minim.
    d.  Rezultatul (lungimea lanțului și coordonatele celulelor) pentru matricea curentă este scris în fișierul de output.
6.  După parcurgerea întregului arbore, se închide fișierul și se eliberează memoria arborelui cu `freeTree()`.

**Output:**
Fișierul de output va conține, pentru fiecare matrice din arbore (în ordinea parcurgerii preordine):
*   O linie cu lungimea celui mai bun lanț Hamiltonian găsit (numărul de muchii, adică numărul de celule din lanț minus 1). Dacă nu există celule vii sau lanțuri, se scrie -1.
*   O a doua linie (dacă lungimea este >= 0) cu coordonatele celulelor din lanț, formatate ca `(linie,coloana)`, separate prin spații.
Fiecare pereche de linii (lungime + lanț) corespunde unei matrici din arbore.

---

**Eliberarea Memoriei:**
La finalul funcției `main`, memoria alocată pentru grila inițială (`grid`) este eliberată. Funcțiile `freeTree` și eliberarea stivei din Task 2 se ocupă de memoria specifică task-urilor respective.