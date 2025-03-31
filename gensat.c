#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <stdbool.h>

#define SATISFIABLE 1
#define UNSATISFIABLE -1
#define UNCERTAIN 0
#define MAX_ATTEMPTS 100000


satcount = 0;
int clauseNumber, variableNumber;
int * valuation; // global valuation array for ease of access during recursion
int CSAT;
struct Literal {
  struct Literal * next; // points to the next literal in the clause
  int index;
};

struct Clause {
  struct Literal * head; // creates a sort of linked list
  struct Clause * next; 
};

// creates, initializes and returns an empty Clause
struct Clause * createClause(){
  struct Clause * instance = malloc(sizeof(struct Clause));
  instance->head = NULL;
  instance->next = NULL;
  return instance;
}

// creates, initializes and returns an empty Literal
struct Literal * createLiteral(){
  struct Literal * instance = malloc(sizeof(struct Literal));
  instance->next = NULL;
  instance->index = 0;
  return instance;
}

// prints the current state of the valuation array
void printValuation(){
  int i;
  for (i = 1; i < variableNumber + 1; i++) {
    printf("%d ", valuation[i]);
  }
  printf("\n");
}

void print(struct Clause * root){
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


// signal function
int sign(int num){
  return (num > 0) - (num < 0);
}



struct Clause * readCNF(char * filename){
  FILE * fp;
  char line[256];
  size_t len = 0;

  fp = fopen(filename, "r");
  if (fp == NULL) exit(1);

  // define loop variables
  char * token;
  struct Clause * root = NULL, * currentClause = NULL, * previousClause = NULL;
  struct Literal * currentLiteral = NULL, * previousLiteral = NULL;

  while(fgets(line, sizeof(line), fp)){
    
    if (line[0] == 'c') continue; // comment line handling
    
    if (line[0] == 'p') {
      sscanf(line, "p cnf %d %d", &variableNumber, &clauseNumber);
      valuation = (int*) calloc(variableNumber + 1, sizeof(int));
      int i;
      for (i = 0; i < variableNumber + 1; i++)
	  {		  
		  valuation[i] = rand() % 2; // rand() % 2; or 0
	  }
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
        if (literalIndex != 0)
		{
          if (currentClause->head == NULL)
		  {
            currentClause->head = currentLiteral;
          }

          if (previousLiteral != NULL)
		  {
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




int greedyLocalSearch(struct Clause * root)
{
	int flipCount = 0;
	int attempts = 0;
	while (attempts < MAX_ATTEMPTS) 
	{
		attempts++;
		
		for (int i = 1; i <= variableNumber; i++)
		{
			valuation[i] = rand() % 2;
		}
		
		flipCount = 0;
		bool improvementFound = true;
		
		while (improvementFound == true) 
		{
			improvementFound = false;
			int bestSat = countSatisfiedClauses(root);
			int bestLiteral = 0;
			
			for (int i = 1; i <= variableNumber; i++)
			{
				flipLiteral(i);
				int newSat = countSatisfiedClauses(root);
				
				if (newSat > bestSat)
				{
					bestSat = newSat;
					bestLiteral = i;
				}
				
				flipLiteral(i);
			}
			
			if (bestLiteral != 0)
			{
				flipLiteral(bestLiteral);
				flipCount++;
				improvementFound = true;
			}
		}
		if (isSatisfiable(root) == SATISFIABLE) 
		{
			return SATISFIABLE;
		}
	}
	return UNSATISFIABLE;
}

int countSatisfiedClauses(struct Clause * root)
{
	int satCount = 0;
	struct Clause *itr = root;
	while (itr != NULL)
	{
		struct Literal *l = itr->head;
		bool cSat = false;
		while (l != NULL)
		{
			if (valuation[abs(l->index)] == (l->index > 0 ? 1 : 0))
			{
				cSat = true;
				break;
			}
			l = l->next;
		}
		if (cSat == true)
		{
			satCount++;
		}
		itr = itr->next;
	}
	return satCount;
}
// deep clones a clause set and injects a new unit clause with the given literal index
// this is how branching is performed
int isSatisfiable(struct Clause *root)
{
	struct Clause *itr = root;
	while (itr != NULL)
	{
		struct Literal *l = itr->head;
		int satisfied = 0;
		while (l != NULL)
		{	//printValuation();
			int litVal = valuation[abs(l->index)];
			
			if (litVal != 0 && litVal == (l->index > 0 ? 1 : 0))
			{
				satisfied = 1;
				CSAT + 1;
				break;
			}
			//if (valuation[abs(l->index)] == (l->index > 0 ? 1 : 0))
			//{
			//	satisfied = 1;
			//	break;
			//}
			l = l->next;
		}
		if(!satisfied) return 0;
		itr = itr->next;
	}
	return 1;
}
int flipLiteral(int index) 
	{
	if (index < 0) {
        valuation[abs(index)] = (valuation[abs(index)] == 1) ? 0 : 1;
    } 
	else {
        valuation[abs(index)] = (valuation[abs(index)] == 1) ? 0 : 1;
    }
	return index;
}
int chooseRandomLiteral(struct Clause * root)
{
	struct Clause *itr = root;
	int clauseCount = 0;
	struct Clause *unsatisfiedClause = NULL;
	
	while (itr != NULL)
	{
		struct Literal *l = itr->head;
		int sat = 0;
		while (l != NULL) 
		{
			if (valuation[abs(l->index)] == (l->index > 0 ? 1 : 0)) 
			{
				sat = 1;
				break;
			}
			
			l = l->next;
		}
		if (!sat)
		{
			unsatisfiedClause = itr;
			break;
		}
		itr = itr -> next;
	}
	if(!unsatisfiedClause) 
	{
		return 0;
	}
		struct Literal *lit = unsatisfiedClause->head;
		int randomint = rand() % 2;
		return randomint == 0 ? lit->index: -lit->index;
}
int walksat(struct Clause * root) 
{
	int attempts = 0;
	while (attempts < MAX_ATTEMPTS) 
	{
		if (isSatisfiable(root) == 1)
		{
			return SATISFIABLE;
		}
		
		int littoflip = chooseRandomLiteral(root);
		if (littoflip == 0)
		{
			break; // this implies that all sat
		}
		
		valuation[abs(littoflip)] = (littoflip > 0) ? 0 : 1;
		attempts++;
	}
	return UNSATISFIABLE;
}
		//



// writes the solution to the given file
void writeSolution(struct Clause * root, char * filename){
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
	clock_t start_time = clock();
	struct Clause *root = readCNF(argv[1]);
  if (greedyLocalSearch(root) == SATISFIABLE) {
        clock_t end_time = clock();
        double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
         printf("CPU time used: %f seconds\n", cpu_time_used);
		printf("%d",CSAT);
		printf("SATISFIABLE\n");
       
        // Print the result
        for (int i = 1; i <= variableNumber; i++) {
            printf("%d %d\n", i, valuation[i]);
        }
    } 
	else 
	{
        clock_t end_time = clock();
        double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
         printf("CPU time used: %f seconds\n", cpu_time_used);
			printf("%d",CSAT);
		printf("UNSATISFIABLE\n");
	}

  return 0;
}
  