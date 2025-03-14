#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

typdef struct 
{
	char literals[];
	int numLiterals;
} Clause;   // Clause struct

typedef struct 
{
	Clause clauses[]
	int numClauses;
	int numVariables;
} CNF;  //Formula struct

bool clauseSatisfactionCheck(const CNF *cnf, int clauseIndex, int *assignment) 
{
	
	//Will iterate through all literals in a clause with a for loop using clause index, 	// returns bool saying whether it is satisfied by the assignment
	// 

}

bool CNFSat(const CNF *cnf, int *assignment)
{
	//Iterates through clauses, check is satisfied vi clause satisfaction function above
	// returns bool if all clauses satisfied

}
{
// Iterates through all
bool SAT(CNF *cnf, int *assignment) 
{
	// Recursive algorithm which implements DPLL, back tracks when required, assigns to var assigning either true or false, returns a bool value

}

Clause clauseParser(file)
{
//takes file or input stream and parses into clauses and counts num literals etc
// returns list of separate clauses
}

void main(input)
{
 // takes input and parses it via parser
	// calls solver
//returns nothing//*
}

