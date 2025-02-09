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

void fillTable()
{

    // Make all states lead to ERROR by default
    for (int i = 0; i < NSTATES; i++)
    {
        for (int c = 0; c < 256; c++)
        {
            if (i == 10)
            {
                table[i][c] = 10;
                continue;
            }
            table[i][c] = ERROR;
        }
        table[i]['/'] = 9;
    }

    table[9]['/'] = 10;
    table[10]['\n'] = ACCEPT;

    for (char i = '0'; i <= '9'; ++i)
    {
        table[START][i] = 8;
        table[8][i] = 8;
    }
    table[8][':'] = START;

    // Skip whitespace
    table[START][' '] = START;

    // If we reach a newline, and are not in the middle of a statement, accept
    table[START]['\n'] = ACCEPT;

    // Accept the statement "go"
    table[START]['g'] = 1;
    table[1]['o'] = 2;
    table[2]['\n'] = ACCEPT;

    table[2][' '] = START;

    table[START]['d'] = 3;
    table[3]['x'] = 4;
    table[3]['y'] = 4;
    table[4]['='] = 5;
    table[5]['-'] = 6;

    for (char i = '0'; i <= '9'; ++i)
    {
        table[5][i] = 7;
        table[6][i] = 7;
        table[7][i] = 7;
    }

    table[7][' '] = START;
    table[7]['\n'] = ACCEPT;

    // TODO Expand the table to pass (and fail) the described syntax
    // table[...][...] = ...
}

#endif
