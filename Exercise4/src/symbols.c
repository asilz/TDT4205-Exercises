#include <vslc.h>

// Declaration of global symbol table
symbol_table_t* global_symbols;

// Declarations of helper functions defined further down in this file
static void find_globals(void);
static void bind_names(symbol_table_t* local_symbols, node_t* root);
static void print_symbol_table(symbol_table_t* table, int nesting);
static void destroy_symbol_tables(void);

static size_t add_string(char* string);
static void print_string_list(void);
static void destroy_string_list(void);

/* External interface */

// Creates a global symbol table, and local symbol tables for each function.
// All usages of symbols are bound to their symbol table entries.
// All strings are entered into the string_list
void create_tables(void)
{
  // TODO:
  // First use find_globals() to create the global symbol table.
  // As global symbols are added, function symbols get their own local symbol tables as well.
  //
  // Once all global symbols are added, go through all functions bodies and bind references.
  //
  // Binding should performed by bind_names(function symbol table, function body AST node).
  // IDENTIFIERs that reference declarations should point to the symbol_t they reference.
  // It handles pushing and popping scopes, and adding variables to the local symbol table.
  // A final task performed by bind_names(...), is adding strings to the global string list.
}

// Prints the global symbol table, and the local symbol tables for each function.
// Also prints the global string list.
// Finally prints out the AST again, with bound symbols.
void print_tables(void)
{
  print_symbol_table(global_symbols, 0);
  printf("\n == STRING LIST == \n");
  print_string_list();
  printf("\n == BOUND SYNTAX TREE == \n");
  print_syntax_tree();
}

// Cleans up all memory owned by symbol tables and the global string list
void destroy_tables(void)
{
  destroy_symbol_tables();
  destroy_string_list();
}

/* Internal matters */

// Goes through all global declarations, adding them to the global symbol table.
// When adding functions, a local symbol table with symbols for its parameters are created.
static void find_globals(void)
{
  global_symbols = symbol_table_init();

  // TODO: Create symbols for all global defintions (global variables, arrays and functions),
  // and add them to the global symbol table. See the symtype_t enum in "symbols.h"

  // When creating a symbol for a function, also create a local symbol_table_t for it.
  // Store this local symbol table in the function symbol's function_symtable field.
  // Any parameters the function may have should be added to this local symbol table.

  // TIP: create symbols using malloc(sizeof(symbol_t)), and assigning the relevant fields.
  // Use symbol_table_insert() (from "symbol_table.h") to insert new symbols into tables.

  // If a symbol already exists with the same name, the insertion will return INSERT_COLLISION.
  // Feel free to print an error message and abort using exit(EXIT_FAILURE),
  // but we will not be testing your compiler on invalid VSL.
}

// A recursive function that traverses the body of a function, and:
//  - Adds variable declarations to the function's local symbol table.
//  - Pushes and pops local variable scopes when entering and leaving blocks.
//  - Binds all IDENTIFIER nodes that are not declarations, to the symbol it references.
//  - Moves STRING_LITERAL nodes' data into the global string list,
//    and replaces the node with a STRING_LIST_REFERENCE node.
//    Overwrites the node's data.string_list_index field with with string list index
static void bind_names(symbol_table_t* local_symbols, node_t* node)
{
  // TODO: Implement bind_names, doing all the things described above
  // Tip: See symbol_hashmap_init() in symbol_table.h, to make new hashmaps for new scopes.
  // Remember the symbol_hashmap_t's backup pointer, forming a linked list of backup hashmaps.
  // Can you use this linked list to implement a stack of hash maps?

  // Tip: Local variables can only be defined in BLOCK nodes.
  // Not all BLOCK nodes define local variables, some only contain a single LIST of statements.
  // Any IDENTIFIER that is not a local variable declaration, must be a symbol usage.

  // Tip: Strings can be added to the string list using add_string(). It returns its index.

  // Note: If an IDENTIFIER has a name that does not correspond to any symbol in the current scope,
  // a parent scope, or in the global symbol table, that is an error.
  // Feel free to print a nice error message and abort.
  // We will not test your compiler on incorrect VSL.
}

// Prints the given symbol table, with sequence number, symbol names and types.
// When printing function symbols, its local symbol table is recursively printed, with indentation.
static void print_symbol_table(symbol_table_t* table, int nesting)
{
  for (size_t i = 0; i < table->n_symbols; i++)
  {
    symbol_t* symbol = table->symbols[i];

    printf(
        "%*s%ld: %s(%s)\n",
        nesting * 4,
        "",
        symbol->sequence_number,
        SYMBOL_TYPE_NAMES[symbol->type],
        symbol->name);

    // If the symbol is a function, print its local symbol table as well
    if (symbol->type == SYMBOL_FUNCTION)
      print_symbol_table(symbol->function_symtable, nesting + 1);
  }
}

// Frees up the memory used by the global symbol table, all local symbol tables, and their symbols
static void destroy_symbol_tables(void)
{
  // TODO: Implement cleanup. All symbols in the program are owned by exactly one symbol table.

  // TIP: Using symbol_table_destroy() goes a long way, but it only cleans up the given table.
  // Try cleaning up all local symbol tables before cleaning up the global one.
}

// Declaration of global string list
char** string_list;
size_t string_list_len;
static size_t string_list_capacity;

// Adds the given string to the global string list, resizing if needed.
// Takes ownership of the string, and returns its position in the string list.
static size_t add_string(char* string)
{
  // TODO: Write a helper function you can use during bind_names(),
  // to easily add a string into the dynamically growing string_list.

  // The length of the string list should be stored in string_list_len.

  // The variable string_list_capacity should contain the maximum number of char*
  // that can fit in the current string_list before we need to allocate a larger array.
  // If length is about to surpass capacity, create a larger allocation first.
  // Tip: See the realloc function from the standard library

  // Return the position the added string gets in the list.
}

// Prints all strings added to the global string list
static void print_string_list(void)
{
  for (size_t i = 0; i < string_list_len; i++)
    printf("%ld: %s\n", i, string_list[i]);
}

// Frees all strings in the global string list, and the string list itself
static void destroy_string_list(void)
{
  // TODO: Called during cleanup, free strings, and the memory used by the string list itself
}
