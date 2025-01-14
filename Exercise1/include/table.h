#ifndef TABLE_H
#define TABLE_H

// The number of states in your table
#define NSTATES 14

// The starting state, at the beginning of each line
#define START 0

// The state to go to after a valid line
// All lines end with the newline character '\n'
#define ACCEPT 12

// The state to jump to as soon as a line is invalid
#define ERROR 13

static int table[NSTATES][256];

void fillTable() {

    // Make all states lead to ERROR by default
    for (int i = 0; i < NSTATES; i++) {
        for (int c = 0; c < 256; c++) {
            table[i][c] = ERROR;
        }
    }

    // Skip whitespace
    table[START][' '] = START;

    // If we reach a newline, and are not in the middle of a statement, accept
    table[START]['\n'] = ACCEPT;

    // Accept the statement "go"
    table[START]['g'] = 1;
    table[1]['o'] = 2;
    table[2]['\n'] = ACCEPT;


    // TODO Expand the table to pass (and fail) the described syntax
    // table[...][...] = ...

}

#endif
