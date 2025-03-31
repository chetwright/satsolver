#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SATISFIABLE 1
#define UNSATISFIABLE -1
#define UNCERTAIN 0


int clauseNumber, variableNumber;
int * valuation; // global valuation array
struct Literal {
  struct Literal * next; // points to nextlit in  clause
  int index;
};

struct Clause {
  struct Literal * head; // creates a sort of linked list
  struct Clause * next; 
};

struct Clause * createClause(){
  struct Clause * instance = malloc(sizeof(struct Clause));
  instance->head = NULL;
  instance->next = NULL;
  return instance;
}


struct Literal * createLiteral(){
  struct Literal * instance = malloc(sizeof(struct Literal));
  instance->next = NULL;
  instance->index = 0;			
  return instance;
}

void printValuation(){
  int i;
  for (i = 1; i < variableNumber + 1; i++) {
    printf("%d ", valuation[i]);
  }
  printf("\n");
}

void printClauseSet(struct Clause * root){
  struct Clause* itr = root;
  while (itr != NULL){
    struct Literal * l = itr->head;
    while (l != NULL){
      printf("%d ", l->index);
      l = l->next;
    }
    printf("\n");
    itr = itr->next;
  }
}

int findUnitClause(struct Clause * root){
  struct Clause * itr = root;
  while (itr != NULL){
    if (itr->head == NULL) {
      continue;
    }
    if(itr->head->next == NULL){
      return itr->head->index;
    }
    itr = itr->next;
  }
  //ret 0 if no unit cluase found
  return 0;
}


int sign(int num){
  return (num > 0) - (num < 0);
}

int findPureLiteral(struct Clause * root){
  int * literalLookup = (int*) calloc(variableNumber + 1, sizeof(int));
  struct Clause * itr = root;
  while (itr != NULL){
    struct Literal * l = itr->head;
    while (l != NULL){
      int seen = literalLookup[abs(l->index)];
      if (seen == 0) literalLookup[abs(l->index)] = sign(l->index);
      else if (seen == -1 && sign(l->index) == 1) literalLookup[abs(l->index)] = 2;
      else if (seen == 1 && sign(l->index) == -1) literalLookup[abs(l->index)] = 2;
      l = l->next;
    }
    itr = itr->next;
  }

  int i;
  for (i = 1; i < variableNumber + 1; i++)
  {
    if (literalLookup[i] == -1 || literalLookup[i] == 1) return i * literalLookup[i];
  }
  return 0;
}

int unitPropagation(struct Clause * root)
  {
  int unitLiteralIndex = findUnitClause(root);
  if (unitLiteralIndex == 0) 
  {
	  return 0;
  }
  valuation[abs(unitLiteralIndex)] = unitLiteralIndex > 0 ? 1 : 0;

  struct Clause * itr = root;
  struct Clause * prev;
  while (itr != NULL){
    struct Literal * currentL = itr->head;
    struct Literal * previousL = createLiteral();
    while (currentL != NULL)
	{
      if (currentL->index == unitLiteralIndex) 
	  {
        if (itr == root)
		{
          *root = *(root->next);
          itr = NULL;
        } else {
          prev->next = itr->next;
          itr = prev;
        }
        break;
      } 
	  else if (currentL->index == -unitLiteralIndex) 
	  {
        if (currentL == itr->head) itr->head = currentL->next;
        else
		{
          previousL->next = currentL->next;
        }
        currentL = previousL;
        continue;
      }
      previousL = currentL;
      currentL = currentL->next;
    }
    prev = itr;
    itr = itr == NULL ? root : itr->next;
  }
  return 1;
}

// implements pure literal elimination algorithm
// returns 0 if it's unable to perform the algorithm in case there are no pure literals
int pureLiteralElimination(struct Clause * root)
{
  int pureLiteralIndex = findPureLiteral(root);
  
  if (pureLiteralIndex == 0)
  {
	  return 0;
  }
  
  valuation[abs(pureLiteralIndex)] = pureLiteralIndex > 0 ? 1 : 0;

  struct Clause * itr = root;
  struct Clause * prev;
  while (itr != NULL){
    struct Literal * l = itr->head;
    while (l != NULL){
      if (l->index == pureLiteralIndex) {
        if (itr == root){
          *root = *(root->next);
          itr = NULL;
        } else {
          prev->next = itr->next;
          itr = prev;
        }
        break;
      }
      l = l->next;
    }
    prev = itr;
    itr = itr == NULL ? root : itr->next;
  }
  return 1;
}

struct Clause * readClauseSet(char * filename){
  FILE * fp;
  char line[256];
  size_t len = 0;

  fp = fopen(filename, "r");
  if (fp == NULL) exit(1);

  char * token;
  struct Clause * root = NULL, * currentClause = NULL, * previousClause = NULL;
  struct Literal * currentLiteral = NULL, * previousLiteral = NULL;

  while(fgets(line, sizeof(line), fp)){
	  
    if (line[0] == 'c')
	{		
		continue; // handles comments
    }
    if (line[0] == 'p') 
	{
      sscanf(line, "p cnf %d %d", &variableNumber, &clauseNumber);
      valuation = (int*) calloc(variableNumber + 1, sizeof(int));
      int i;
      for (i = 0; i < variableNumber + 1; i++) valuation[i] = -1;
    } 
	else 
	{
		
      currentClause = createClause();
      if (root == NULL)
	  {
        root = currentClause;
	  }
      if (previousClause != NULL)
	  {
        previousClause->next = currentClause;
      }

      token = strtok(line, " ");
      while(token != NULL){
        int literalIndex = atoi(token);
        currentLiteral = createLiteral();
        currentLiteral->index = literalIndex;
        if (literalIndex != 0){
          if (currentClause->head == NULL){
            currentClause->head = currentLiteral;
          }

          if (previousLiteral != NULL){
            previousLiteral->next = currentLiteral;
          }
        }

        previousLiteral = currentLiteral;

        token = strtok(NULL, " ");
      }
      previousClause = currentClause;
    }
  }
  fclose(fp);

  return root;
}

int areAllClausesUnit(struct Clause * root){
  int * literalLookup = (int*) calloc(variableNumber + 1, sizeof(int));

  struct Clause* itr = root;
  while (itr != NULL){
    struct Literal * l = itr->head;
    while (l != NULL){
      int seen = literalLookup[abs(l->index)];
      if (seen == 0) literalLookup[abs(l->index)] = sign(l->index);
      // if we previously have seen this literal with the opposite sign, return false
      else if (seen == -1 && sign(l->index) == 1) return 0;
      else if (seen == 1 && sign(l->index) == -1) return 0;
      l = l->next;
    }
    itr = itr->next;
  }

  itr = root;
  while (itr != NULL){
    struct Literal * l = itr->head;
    while (l != NULL){
      valuation[abs(l->index)] = l->index > 0 ? 1 : 0;
      l = l->next;
    }
    itr = itr->next;
  }

  // return true to terminate dpll
  return 1;
}

int containsEmptyClause(struct Clause * root){
  struct Clause* itr = root;
  while (itr != NULL){
    // if the head pointer is null, no literals
    if(itr->head == NULL) return 1;
    itr = itr->next;
  }
  return 0;
}

// checks if the current state of the clause set represents a solution
int checkSolution(struct Clause * root)
{
  if (containsEmptyClause(root)) return UNSATISFIABLE;
  if (areAllClausesUnit(root)) return SATISFIABLE;
  return UNCERTAIN;
}

int chooseLiteral(struct Clause * root)
{
  //return the first literal
  return root->head->index;
}

// deep clones a clause constructing a new clause and literal structs
struct Clause * cloneClause(struct Clause * origin){
  struct Clause * cloneClause = createClause();
  struct Literal * iteratorLiteral = origin->head;
  struct Literal * previousLiteral = NULL;

  // iterate over the clause to clone literals as well
  while (iteratorLiteral != NULL){
    struct Literal * literalClone = createLiteral();
    literalClone->index = iteratorLiteral->index;
    if (cloneClause->head == NULL) {
      cloneClause->head = literalClone;
    }
    if (previousLiteral != NULL) {
      previousLiteral->next = literalClone;
    }
    previousLiteral = literalClone;
    iteratorLiteral = iteratorLiteral->next;
  }
  return cloneClause;
}

struct Clause * branch(struct Clause * root, int literalIndex){
  
  valuation[abs(literalIndex)] = literalIndex > 0 ? 1 : 0;

  struct Clause * newClone = NULL,
                * currentClause = NULL,
                * previousClause = NULL,
                * iterator = root;
  while (iterator != NULL){
    struct Clause * clone = cloneClause(iterator);
    if (newClone == NULL) {
      newClone = clone;
    }
    if (previousClause != NULL) {
      previousClause->next = clone;
    }
    previousClause = clone;
    iterator = iterator->next;
  }
  struct Clause * addedClause = createClause();
  struct Literal * addedLiteral = createLiteral();
  addedLiteral->index = literalIndex;
  addedClause->head = addedLiteral;

  addedClause->next = newClone;
  return addedClause;
}

void removeLiteral(struct Literal * literal){
  while (literal != NULL) {
    struct Literal * next = literal->next;
    free(literal);
    literal = next;
  }
}

void removeClause(struct Clause * root){
  while (root != NULL) {
    struct Clause * next = root->next;
    if (root->head != NULL) removeLiteral(root->head);
    free(root);
    root = next;
  }
}

// DPLL algorithm with recursive backtracking
int dpll(struct Clause * root)
{
  // first check if we are already in a solved state
  int solution = checkSolution(root);
  if (solution != UNCERTAIN){
    removeClause(root);
    return solution;
  }

  
  while(1) //unit propogate
  {
    solution = checkSolution(root);
    if (solution != UNCERTAIN){
      removeClause(root);
      return solution;
    }
    if (!unitPropagation(root)) break;
  }

  while(1){ //Pure elim
    int solution = checkSolution(root);
    if (solution != UNCERTAIN) {
      removeClause(root);
      return solution;
    }
    if (!pureLiteralElimination(root)) {

		break;
	}
  }

   int literalIndex = chooseLiteral(root);
  
   if (dpll(branch(root, literalIndex)) == SATISFIABLE) 
  {
	  return SATISFIABLE;
  }

  // if it doesn't yield a solution try negated literal
  return dpll(branch(root, -literalIndex)); // recursive element of dpll
}

// writes solution to given file
void writeSolution(struct Clause * root, char * filename)

{
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

   int i;
  for (i = 1; i < variableNumber + 1; i++) {
    fprintf(f, "%d %d\n", i, valuation[i]);
  }

  fclose(f);
}

int main(int argc, char *argv[])
{
  
	
  struct Clause * root = readClauseSet(argv[1]);
  clock_t start_time = clock(); //startting after file read so that file read time does not affect time
  if (dpll(root) == SATISFIABLE) 
  {
	clock_t end_time = clock();
	double cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

	printf("CPU time used: %f seconds\n", cpu_time_used);
	
	// We are doing this here to prevent write time being calculated in

    printf("SATISFIABLE\n");
    //writeSolution(root, argv[2]);
	int i;
	for (i = 1; i < variableNumber + 1; i++) 
	{
		printf("%d %d\n", i, valuation[i]);
	}
	writeSolution(root, argv[2]);
  } 
  else 
  {
	clock_t end_time = clock();
	double cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

	printf("CPU time used: %f seconds\n", cpu_time_used);
	// See above
    printf("UNSATISFIABLE\n");
  }
  removeClause(root);
  return 0;
}
  