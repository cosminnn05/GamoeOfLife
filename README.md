# Proiect Game of Life - Analiză Cod (Concise)

Acest document descrie funcționalitatea codului C pentru proiectul "Game of Life", structurat pe task-uri.

## Structura Generală

Codul simulează "Conway's Game of Life" cu diverse funcționalități adăugate.

**Definiții Globale:**
*   `ALIVE ('X')`: Celulă vie.
*   `DEAD ('+')`: Celulă moartă.

**Structuri de Date Cheie:**
*   `Celula`: Stochează coordonatele (linie, coloană) ale unei celule.
*   `Nod` (Task 2): Element de stivă pentru a stoca diferențele (celulele schimbate) între generații.
*   `Nod3` (Task 3 & 4): Nod într-un arbore binar, stocând o configurație a grilei.

**Funcții Utilitare Comune:**
*   `citireMatrice()`: Citește configurația inițială (task, N, M, K, grilă) din fișier.
*   `scriereMatrice()`: Scrie starea grilei în fișier.
*   `countAliveNeighbors()`: Calculează vecinii vii ai unei celule.

---

## Task 1: Simulare Standard Game of Life

**Scop:** Simularea clasică Game of Life pentru K generații și afișarea grilei la fiecare pas.

**Logic:**
1.  Citește datele de intrare.
2.  Scrie grila inițială.
3.  Pentru K pași:
    a.  Calculează următoarea generație folosind regulile standard (o celulă vie cu 2-3 vecini supraviețuiește; o celulă moartă cu 3 vecini devine vie). Funcția `nextGenerationWithDiffs` este folosită, dar diferențele sunt ignorate.
    b.  Scrie noua grilă.

**Output:** Fișier cu starea grilei la fiecare generație, separate de linii goale.

---

## Task 2: Optimizarea Stocării Generațiilor

**Scop:** Stochează doar celulele care își schimbă starea între generații, folosind o stivă.

**Funcții Cheie:**
*   `nextGenerationWithDiffs()`: Generează următoarea stare și returnează un array de celule care și-au schimbat starea.
*   `push()`: Adaugă un set de diferențe în vârful stivei.
*   `scrieDiferente()`: Scrie diferențele din stivă (în ordine cronologică) în fișier.

**Logic:**
1.  Pentru K pași, se calculează următoarea generație și diferențele (`nextGenerationWithDiffs`).
2.  Fiecare set de diferențe este adăugat pe o stivă (`push`).
3.  După K pași, diferențele sunt scrise din stivă în fișier (`scrieDiferente`), inversând ordinea pentru a fi cronologică.

**Output:** Fișier cu linii de forma: `nr_generatie dif1_linie dif1_col dif2_linie dif2_col ...`.

---

## Task 2 Bonus: Reconstrucția Stării Inițiale

**Scop:** Reconstruiește grila inițială folosind stiva de diferențe de la Task 2.

**Funcție Cheie:**
*   `reverseGeneration()`: Aplică inversul schimbărilor stocate în stivă pe grila curentă (aflată la generația K) pentru a reveni la starea inițială.

**Logic:**
1.  După Task 2, grila este la generația K.
2.  `reverseGeneration` parcurge stiva de diferențe (de la K la 1). Pentru fiecare set de diferențe, inversează starea celulelor specificate în grilă.
3.  Grila finală este cea inițială și este afișată la consolă.

**Output (Bonus):** Matricea inițială reconstruită, afișată la STDOUT.

---

## Task 3: Arborele Generațiilor

**Scop:** Construiește un arbore binar de stări ale grilei până la adâncimea K. De la fiecare nod, copilul stâng e generat cu o regulă "B", iar cel drept cu regula standard.

**Funcții Cheie:**
*   `applyRuleB()`: O celulă devine vie dacă are 2 vecini vii, altfel starea nu se schimbă.
*   `applyStandardRule()`: Regula standard Game of Life.
*   `createTree()`: Construiește recursiv arborele.
*   `scriePreordine()`: Scrie matricile din arbore în parcurgere preordine.
*   `freeTree()`: Eliberează memoria arborelui.

**Logic:**
1.  `createTree` construiește arborele: rădăcina este grila inițială. La fiecare nod, se generează două stări copil (stânga cu Regula B, dreapta cu Regula Standard) până la adâncimea K.
2.  `scriePreordine` parcurge arborele și scrie fiecare matrice în fișier.

**Output:** Fișier cu matricile din arbore (parcurgere preordine), separate de linii goale.

---

## Task 4: Căutarea Lanțului Hamiltonian

**Scop:** Pentru fiecare matrice din arborele de la Task 3, găsește cel mai lung lanț Hamiltonian format din celulele vii adiacente.

**Funcții Cheie:**
*   `dfsComponenta()`: Găsește componentele conexe ale celulelor vii.
*   `dfsHamiltonian()`: Caută (DFS) cel mai lung lanț Hamiltonian într-o componentă conexă, prioritizând lexicografic.
*   `processNodeHamiltonian()`: Procesează o matrice: extrage celule vii, construiește graf de adiacență, găsește componente conexe, și pentru fiecare, caută lanțul Hamiltonian. Selectează cel mai bun lanț global (lungime maximă, apoi lexicografic minim).
*   `parcurgereArboreTask4()`: Parcurge arborele și aplică `processNodeHamiltonian` pe fiecare nod.

**Logic:**
1.  Se construiește arborele ca la Task 3.
2.  `parcurgereArboreTask4` iterează prin nodurile arborelui (preordine).
3.  Pentru fiecare matrice:
    a.  `processNodeHamiltonian` identifică celulele vii și adiacențele lor.
    b.  Găsește componentele conexe.
    c.  În fiecare componentă, `dfsHamiltonian` caută cel mai lung lanț. Se ține cont de constrângerile lexicografice pentru a alege între lanțuri de aceeași lungime.
    d.  Rezultatul (lungimea lanțului - 1 și coordonatele) este scris în fișier.

**Output:** Pentru fiecare matrice din arbore (preordine):
*   O linie cu lungimea celui mai lung lanț (nr. muchii) sau -1.
*   O linie cu coordonatele `(linie,coloana)` ale celulelor din lanț.

---

**Eliberarea Memoriei:** Codul include funcții pentru eliberarea memoriei alocate dinamic pentru grile, stivă și arbore.