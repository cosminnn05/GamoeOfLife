#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIVE 'X'
#define DEAD '+'


//task 1
typedef struct {
    int linie;
    int coloana;
}Celula;

typedef struct Nod {
    Celula *diferente;
    int count_dif;
    struct Nod *urm;
}Nod;



// Funcție pentru a citi matricea din fișier
void citireMatrice(const char *filename, char ***grid, int *N, int *M, int *K, int *task) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Eroare la deschiderea fișierului");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", task);  // Citirea numărului taskului
    fscanf(file, "%d %d", N, M);    //Citirea dimensiunilor N si M
    fscanf(file, "%d", K);  //Citirea nr generatii

    *grid = (char **)malloc(*N * sizeof(char *));
    for (int i = 0; i < *N; i++) {
        (*grid)[i] = (char *)malloc((*M + 1) * sizeof(char)); // +1 pentru caracterul de sfârșit de linie
        fscanf(file, "%100s", (*grid)[i]);
    }

    fclose(file);
}

// Funcție pentru a scrie matricea într-un fișier
void scriereMatrice(const char *filename, char **grid, int N, int M) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Eroare la deschiderea fișierului de output");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; i++) {
        fprintf(file, "%s\n", grid[i]);
    }
    fprintf(file, "\n"); // Linie goală între generații

    fclose(file);
}

// Funcție pentru a calcula numărul de vecini vii
int countAliveNeighbors(char **grid, int N, int M, int x, int y) {
    const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int count = 0;

    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (nx >= 0 && nx < N && ny >= 0 && ny < M && grid[nx][ny] == ALIVE) {  //Verifica limitele superioara, inferioara, la stanga si la dreapta a grilei si daca celula vecina este vie
            count++;
        }
    }

    return count;
}

// Funcție veche pentru a genera următoarea generație
/*void nextGeneration(char **grid, int N, int M) {
    char **newGrid = (char **)malloc(N * sizeof(char *));
    for (int i = 0; i < N; i++) {
        newGrid[i] = (char *)malloc((M + 1) * sizeof(char));
        for (int j = 0; j < M; j++) {
            int aliveNeighbors = countAliveNeighbors(grid, N, M, i, j);
            if (grid[i][j] == ALIVE) {
                newGrid[i][j] = (aliveNeighbors == 2 || aliveNeighbors == 3) ? ALIVE : DEAD;
            } else {
                newGrid[i][j] = (aliveNeighbors == 3) ? ALIVE : DEAD;
            }
        }
        newGrid[i][M] = '\0'; // Adăugăm terminatorul de linie
    }

    // Copiem matricea nouă în cea veche și eliberăm memoria
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            grid[i][j] = newGrid[i][j];
        }
        free(newGrid[i]);
    }
    free(newGrid);
}
*/

//task 2

//Funcție nouă pentru a genera următoarea generație
Celula* nextGenerationWithDiffs(char **grid, int N, int M, int *numDif) {
    char **newGrid = (char **)malloc(N * sizeof(char *));
    Celula *dif = malloc(N * M * sizeof(Celula));
    *numDif = 0;

    for (int i = 0; i < N; i++) {
        newGrid[i] = (char *)malloc((M + 1) * sizeof(char));
        for (int j = 0; j < M; j++) {
            int alive = countAliveNeighbors(grid, N, M, i, j);
            char newState;
            if (grid[i][j] == ALIVE) {
                if (alive == 2 || alive == 3) {
                    newState = ALIVE;  // Celula rămâne vie
                } else {
                    newState = DEAD;   // Celula moare
                }
            } else {
                if (alive == 3) {
                    newState = ALIVE;  // Celula renaște
                } else {
                    newState = DEAD;   // Celula rămâne moartă
                }
            }
            newGrid[i][j] = newState;

            if (newState != grid[i][j]) {
                dif[*numDif].linie = i;
                dif[*numDif].coloana = j;
                (*numDif)++;
            }
        }
        newGrid[i][M] = '\0';
    }

    for (int i = 0; i < N; i++) {
        strcpy(grid[i], newGrid[i]);
        free(newGrid[i]);
    }
    free(newGrid);
    return dif;
}

void push(Nod **varf, Celula *dif, int count_dif){
    Nod *nou= (Nod *)malloc(sizeof(Nod));
    nou->diferente= dif;
    nou->count_dif=count_dif;
    nou->urm= *varf;
    *varf= nou;
}

void scrieDiferente(const char *filename, Nod *top) {
    if (!top) return;

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Eroare la scrierea diferențelor");
        exit(EXIT_FAILURE);
    }

    // Stivă temporară pentru inversare
    Nod *rev = NULL;
    Nod *current= top;
    while (current) {
        push(&rev, current->diferente, current->count_dif);
        current = current->urm;
    }
    int temp=1;
    Nod *it = rev;
    while (it) {
        fprintf (file, "%d", temp++);
        for (int i = 0; i < it->count_dif; i++) {
            fprintf(file, " %d %d", it->diferente[i].linie, it->diferente[i].coloana);
        }
        fprintf(file, "\n");
        it = it->urm;
    }
    
    while (rev){
        Nod *aux = rev;
        rev = rev->urm;
        free(aux);
    }

    fclose(file);
}

//task 2 bonus
void reverseGeneration(char **grid, int N, int M, Nod *top){
    Nod *curent = top;

    while (curent){
        for (int i=0; i<curent->count_dif; i++){
            int linie= curent->diferente[i].linie;
            int coloana= curent->diferente[i].coloana;

            //Inversare stare celula
            if(linie >= 0 && linie <N && coloana >=0 && coloana <M){
                if(grid[linie][coloana] == ALIVE)
                    grid[linie][coloana] = DEAD;
                else
                    grid[linie][coloana]= ALIVE;
            }       
        }
        curent = curent->urm;
    }
}



//task 3

typedef struct Nod3 {
    char **matrice;
    struct Nod3 *left;
    struct Nod3 *right;
}Nod3;

char **copyMatrix(char **sursa, int N, int M){
    char **destinatie = (char **)malloc(N* sizeof(char *));
    for (int i=0; i<N; i++){
        destinatie[i]= (char *)malloc((M+1)* sizeof(char));
        strcpy(destinatie[i], sursa[i]);
    }
    return destinatie;
}

char **applyRuleB(char **grid, int N, int M){
    char **newGrid = (char **)malloc(N * sizeof(char *));
    for (int i = 0; i < N; i++) {
        newGrid[i] = (char *)malloc((M + 1) * sizeof(char));
        for (int j = 0; j < M; j++) {
            int aliveNeighbors = countAliveNeighbors(grid, N, M, i, j);
            if (aliveNeighbors == 2)
                newGrid[i][j] = ALIVE;
            else
                newGrid[i][j] = grid[i][j];  // păstrează starea curentă
        }
        newGrid[i][M] = '\0';
    }
    return newGrid;
}


char **applyStandardRule (char **grid, int N, int M) {
    char **new = (char **)malloc(N * sizeof(char *));
    for (int i = 0; i < N; i++) {
        new[i] = (char *)malloc((M + 1) * sizeof(char));
        for (int j = 0; j < M; j++) {
            int vecini = countAliveNeighbors(grid, N, M, i, j);
            if (grid[i][j] == ALIVE) {
                if (vecini == 2 || vecini == 3) {
                    new[i][j] = ALIVE;
                } else {
                    new[i][j] = DEAD;
                }
            } else {
                if (vecini == 3) {
                    new[i][j] = ALIVE;
                } else {
                    new[i][j] = DEAD;
                }
            }
        }
        new[i][M] = '\0';
    }
    return new;
}


Nod3* createTree (char **matrice, int N, int M, int nivelCurent, int nivelMaxim){
    if (nivelCurent > nivelMaxim)
    return NULL;

    Nod3 *nod = (Nod3 *)malloc(sizeof(Nod3));
    nod->matrice = copyMatrix(matrice, N, M);
    nod->left = NULL;
    nod->right = NULL;

    if (nivelCurent < nivelMaxim) {
        char **leftMatrix = applyRuleB(nod->matrice, N, M);             //Aplicam, in stanga, regula noua B
        char **rightMatrix = applyStandardRule(nod->matrice, N, M);     //Aplicam, in dreapta, regula obisnuita

        nod->left = createTree(leftMatrix, N, M, nivelCurent+1, nivelMaxim);    //Cream matricea noua in nodul din stanga
        nod->right = createTree(rightMatrix, N, M, nivelCurent+1, nivelMaxim);  //Cream matricea noua in nodul din dreapta

        //Eliberam matricile
        for (int i=0; i<N; i++){
            free(leftMatrix[i]);
            free(rightMatrix[i]);
        } 
        free(leftMatrix);
        free(rightMatrix);
    }
    return nod;
}

void freeTree(Nod3 *nod, int N) {
    if (!nod) return;

    for (int i=0; i<N; i++){
        free(nod->matrice[i]);
    }
    free(nod->matrice);

    freeTree(nod->left, N);
    freeTree(nod->right, N);

    free(nod);
}


void scriePreordine(FILE *file, Nod3 *nod, int N, int M){
    if (!nod) return;

    for (int i = 0; i < N; i++) {
        fprintf(file, "%s\n", nod->matrice[i]);
    }
    fprintf(file, "\n");  // linie goală între matrici

    scriePreordine(file, nod->left, N, M);
    scriePreordine(file, nod->right, N, M);
}


//task 4
//structura auxiloar pentru sortarea vecinilor dupa coordonate
typedef struct {
    int index;
    Celula coord;
} VecinAux;

//Compara celulele dupa linie, apoi dupa coloana
int compareCells(const void *a, const void *b){
    const Celula *ca = (const Celula *)a;
    const Celula *cb = (const Celula *)b;
    if (ca->linie != cb->linie)
        return ca->linie - cb->linie;
    return ca->coloana - cb->coloana;
}

//Compara 2 vecini (VecinAux) folosind coordonatele
int cmpVecinAux(const void *a, const void *b){
    const VecinAux *va = (const VecinAux *)a;
    const VecinAux *vb = (const VecinAux *)b;
    return compareCells(&va->coord, &vb->coord);
}

//DFS pentru a extrage o componenta conexa
void dfsComponenta(int **adj, int *visited, int node, int n, int *componenta, int *size){
    visited[node] =1;
    componenta[(*size)++]= node;
    for (int i=0; i<n; i++)
        if (adj[node][i] && !visited[i])
            dfsComponenta(adj, visited, i, n, componenta, size);
}

//DFS pentru cautare Hamiltoniana
void dfsHamiltonian(int **adj, int *visited, int *path, int adancime, int n, int *maxLenght, int *bestPath, int *foundFull, Celula *cells, int *component){
    if (*foundFull) return;
    //Caz de baza : am gasit un lant complet
    if (adancime==n){
        *maxLenght =adancime;
        memcpy(bestPath, path, adancime * sizeof(int));     //Functia memcpy copiaza un block de memorie dintr-o parte in alta, din path in bestPath
        *foundFull = 1;
        return;
    }

    int current = path[adancime - 1];
    //Construim lista de vecini nevizitati
    VecinAux *vecini = malloc(n * sizeof(VecinAux));
    int nrvecini = 0;

    for (int i = 0; i < n; i++) {
        if (adj[current][i] && !visited[i]) {
            vecini[nrvecini].index =i;
            vecini[nrvecini].coord =cells[component[i]];
            nrvecini++;
        }
    }

    //Sortam vecinii lexicografic
    qsort(vecini, nrvecini, sizeof(VecinAux), cmpVecinAux);     //qsort adica quicksort, sorteaza "nrvecini" elemente din "vecini",  de marimea sizeof(VecinAux), dupa criteriul de comparatie din cmpVecinAuz

    //DFS recursiv pentru fiecare vecin
    for (int i = 0; i < nrvecini && !(*foundFull); i++) {
        int idx = vecini[i].index;
        visited[idx] = 1;
        path[adancime] = idx;
        dfsHamiltonian(adj, visited, path, adancime + 1, n, maxLenght, bestPath, foundFull, cells, component);
        visited[idx] = 0;
    }

    free(vecini);
}

//Functie care proceseaza o singura matrice, cauta cel mai lung lant hamiltonian
void processNodeHamiltonian(FILE *file, char **matrice, int N, int M){
    int count = 0;
    //Extragem celulele vii
    Celula *vii =malloc(N * M * sizeof(Celula));
    for (int i=0; i<N; i++)
        for (int j=0; j<M; j++)
            if (matrice[i][j] == ALIVE)
                vii[count++] = (Celula){i, j};
    //Nu exista celule vii
    if (count == 0){
        fprintf(file, "-1");
        free(vii);
        return;
    }
    //Sortam celulele vii pentru consistenta
    qsort(vii, count, sizeof(Celula), compareCells);    //Le sortam pentru consistenta, pentru a fi mai usor de parcurs, fiind sortate mai intai dupa linie,a apoi dupa coloana

    //Construim matricea de adiacenta
    int **adj =malloc(count * sizeof(int *));
    for (int i=0; i<count; i++){
        adj[i] =calloc(count, sizeof(int));
        for (int j=0; j<count; j++) {
            if (i != j && abs(vii[i].linie - vii[j].linie) <= 1 && abs(vii[i].coloana - vii[j].coloana) <= 1)   //Conditie ca celulele sa fie diferite sa fie vecine (aceasi linie sau coloana daca diferenta este 0, pe linii adiacente daca este 1)
                adj[i][j] = 1;
        }
    }

    //Extragem componentele conexe
    int *visited =calloc(count, sizeof(int));
    int **components =malloc(count * sizeof(int *));
    int *compSizes =calloc(count, sizeof(int));
    int numComponents =0;

    
    for (int i=0; i<count; i++) {
        if (!visited[i]) {
            components[numComponents] = malloc(count * sizeof(int));
            dfsComponenta(adj, visited, i, count, components[numComponents], &compSizes[numComponents]);
            numComponents++;
        }
    }

    int bestLenght =-1;
    int bestIdx =-1;
    int *bestPath =malloc(count * sizeof(int));

    //Parcurgem fiecare componenta conexa pt a gasi cel mai lung lant
    for (int c=0; c<numComponents; c++) {
        int n =compSizes[c];

        //Componenta cu un singur nod
        if (n==1) {
            if (bestLenght<1) {
                bestLenght =1;
                bestIdx =c;
                bestPath[0] =0;
            }
            continue;
        }

        //Construim sub-matricea de adiacenta pt componenta curenta
        int **adjC =malloc(n * sizeof(int *));
        for (int i=0; i<n; i++) {
            adjC[i] =malloc(n * sizeof(int));
            for (int j = 0; j < n; j++) {
                int u =components[c][i];
                int v =components[c][j];
                adjC[i][j] =adj[u][v];
            }
        }

        //Cautam cel mai lung lant Hamiltonian
        int *visitedC =calloc(n, sizeof(int));
        int *path =malloc(n * sizeof(int));
        int *localBest =malloc(n * sizeof(int));
        int maxL =-1;
        int foundFull =0;

        for (int start=0; start<n && !foundFull; start++) {
            visitedC[start] =1;
            path[0] =start;
            dfsHamiltonian(adjC, visitedC, path, 1, n, &maxL, localBest, &foundFull, vii, components[c]);
            visitedC[start] =0;
        }

        //Verificam daca aceasta componenta are un lant mai bun
        if (maxL > bestLenght) {
            bestLenght =maxL;
            bestIdx =c;
            memcpy(bestPath, localBest, maxL * sizeof(int));
        } else if (maxL ==bestLenght && bestLenght > 0) {
            int isBetter=0;
            for (int i=0; i<bestLenght; i++) {
                Celula a =vii[components[c][localBest[i]]];
                Celula b =vii[components[bestIdx][bestPath[i]]];
                int cmp =compareCells(&a, &b);
                if (cmp < 0) {
                    isBetter = 1;
                    break;
                } else if (cmp > 0) {
                    break;
                }
            }
            if (isBetter) {
                bestIdx = c;
                memcpy(bestPath, localBest, maxL * sizeof(int));
            }
        }

        for (int i = 0; i < n; i++) free(adjC[i]);
        free(adjC);
        free(visitedC);
        free(path);
        free(localBest);
    }

    //Scriem rezultatul in fisier
    if (bestLenght == -1) {
        fprintf(file, "-1\n");
    } else {
        fprintf(file, "%d\n", bestLenght - 1);  //-1 pentru ca lungimea este nr de noduri -1
        for (int i = 0; i < bestLenght; i++) {
            int idx = components[bestIdx][bestPath[i]];
            fprintf(file, "(%d,%d)", vii[idx].linie, vii[idx].coloana);
            if (i < bestLenght - 1)
                fprintf(file, " ");
        }
        fprintf(file, "\n");
    }

    for (int i = 0; i < count; i++) free(adj[i]);
    free(adj);
    for (int i = 0; i < numComponents; i++) free(components[i]);
    free(components);
    free(compSizes);
    free(visited);
    free(bestPath);
    free(vii);
}

//Parcurge arborele binar in preordine si aplica Hamiltonianul pe fiecare nod
void parcurgereArboreTask4(FILE *file, Nod3 *nod, int N, int M){
    if (!nod) return;
    processNodeHamiltonian(file, nod->matrice, N, M);
    parcurgereArboreTask4(file, nod->left, N, M);
    parcurgereArboreTask4(file, nod->right, N, M);
}

//main
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Utilizare: %s input.txt output.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    char **grid;
    int N, M, K, task;

    citireMatrice(argv[1], &grid, &N, &M, &K, &task);
    printf("Debug, task = %d, N= %d, M=%d, K=%d\n", task, N, M, K);

    FILE *outFile = fopen(argv[2], "w");
    fclose(outFile); // Șterge conținutul fișierului dacă există
    
    if (task == 1){
    scriereMatrice(argv[2], grid, N, M); // Scriem prima generație

    for (int k = 0; k < K; k++) {
        int temp;
        Celula *dif=nextGenerationWithDiffs(grid, N, M, &temp);
        scriereMatrice(argv[2], grid, N, M);
        free(dif);
    }

}
    
    else if (task == 2){
        Nod *stiva= NULL;
        for (int k=0; k < K; k++){
            int nrDif;
            Celula *dif = nextGenerationWithDiffs(grid, N, M, &nrDif);
            push(&stiva, dif, nrDif);
        }
        scrieDiferente(argv[2], stiva);

        //task2 bonus:
        reverseGeneration(grid, N, M, stiva);
        printf("Bonus: Matricea initiala reconstruita:\n");
        for(int i=0; i< N; i++)
            printf("%s\n", grid[i]);
        //Eliberare memorie
        while(stiva){
            Nod *temp = stiva;
            stiva=stiva->urm;
            free(temp->diferente);
            free(temp);
        }

    }

    else if(task == 3){
        Nod3 *root = createTree(grid, N, M, 0, K);
        FILE *out = fopen(argv[2], "w");
        scriePreordine(out, root, N, M);
        fclose(out);
        freeTree(root, N);
    }

    else if(task==4){
        Nod3 *root=createTree(grid, N, M, 0, K);
        FILE *out=fopen(argv[2], "w");
        if(!out){
            perror("Eroare la deschiderea fisierului de output");
            exit(EXIT_FAILURE);
        }
        parcurgereArboreTask4(out, root, N, M);
        fclose(out);
        freeTree(root, N);
    }

    // Eliberăm memoria
    for (int i = 0; i < N; i++) {
        free(grid[i]);
    }
    free(grid);
    
    return 0;
}
