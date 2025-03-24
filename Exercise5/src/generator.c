#include "vslc.h"

// This header defines a bunch of macros we can use to emit assembly to stdout
#include "emit.h"

// In the System V calling convention, the first 6 integer parameters are passed in registers

static const char *REGISTER_PARAMS[6] = {RDI, RSI, RDX, RCX, R8, R9};
#define NUM_REGISTER_PARAMS (sizeof(REGISTER_PARAMS) / sizeof(*REGISTER_PARAMS))

// Takes in a symbol of type SYMBOL_FUNCTION, and returns how many parameters the function takes
#define FUNC_PARAM_COUNT(func) ((func)->node->children[1]->n_children)

static void generate_stringtable(void);
static void generate_global_variables(void);
static void generate_function(symbol_t *function);
static void generate_expression(node_t *expression);
static void generate_statement(node_t *node);
static void generate_main(symbol_t *first);
static void generate_assignment_statement(node_t *statement);

// Entry point for code generation
void generate_program(void)
{
  generate_stringtable();
  generate_global_variables();

  // This directive announces that the following assembly belongs to the .text section,
  // which is the section where all executable assembly lives
  DIRECTIVE(".text");

  // TODO: (Part of 2.3)
  // For each function in global_symbols, generate it using generate_function ()
  for (size_t i = 0; i < global_symbols->n_symbols; ++i)
  {
    if (global_symbols->symbols[i]->type == SYMBOL_FUNCTION)
    {
      generate_function(global_symbols->symbols[i]);
    }
  }

  // TODO: (Also part of 2.3)
  // In VSL, the topmost function in a program is its entry point.
  // We want to be able to take parameters from the command line,
  // and have them be sent into the entry point function.
  //
  // Due to the fact that parameters are all passed as strings,
  // and passed as the (argc, argv)-pair, we need to make a wrapper for our entry function.
  // This wrapper handles string -> int64_t conversion, and is already implemented.
  // call generate_main ( <entry point function symbol> );
  for (size_t i = 0; i < global_symbols->n_symbols; ++i)
  {
    if (global_symbols->symbols[i]->type == SYMBOL_FUNCTION)
    {
      return generate_main(global_symbols->symbols[i]);
    }
  }
}

// Prints one .asciz entry for each string in the global string_list
static void generate_stringtable(void)
{
  // This section is where read-only string data is stored
  // It is called .rodata on Linux, and "__TEXT, __cstring" on macOS
  DIRECTIVE(".section %s", ASM_STRING_SECTION);

  // These strings are used by printf
  DIRECTIVE("intout: .asciz \"%s\"", "%ld");
  DIRECTIVE("strout: .asciz \"%s\"", "%s");
  // This string is used by the entry point-wrapper
  DIRECTIVE("errout: .asciz \"%s\"", "Wrong number of arguments");

  // TODO 2.1: Print all strings in the program here, with labels you can refer to later
  // You have access to the global variables string_list and string_list_len from symbols.c
  for (size_t i = 0; i < string_list_len; ++i)
  {
    DIRECTIVE(".string%zu: .asciz %s", i, string_list[i]);
  }
}

// Prints .zero entries in the .bss section to allocate room for global variables and arrays
static void generate_global_variables(void)
{
  // This section is where zero-initialized global variables lives
  // It is called .bss on linux, and "__DATA, __bss" on macOS
  DIRECTIVE(".section %s", ASM_BSS_SECTION);
  DIRECTIVE(".align 8");

  // TODO 2.2: Fill this section with all global variables and global arrays
  // Give each a label you can find later, and the appropriate size.
  // Regular variables are 8 bytes, while arrays are 8 bytes per element.
  // Remember to mangle the name in some way, to avoid collisions with labels
  // (for example, put a '.' in front of the symbol name)

  // As an example, to set aside 16 bytes and label it .myBytes, write:
  // DIRECTIVE(".myBytes: .zero 16")
  for (size_t i = 0; i < global_symbols->n_symbols; ++i)
  {
    if (global_symbols->symbols[i]->type == SYMBOL_GLOBAL_VAR)
    {
      DIRECTIVE("%s: .zero 8", global_symbols->symbols[i]->name);
    }
    if (global_symbols->symbols[i]->type == SYMBOL_GLOBAL_ARRAY)
    {
      DIRECTIVE("%s: .zero %ld", global_symbols->symbols[i]->name, global_symbols->symbols[i]->node->children[1]->data.number_literal * 8);
    }
  }
}

// Global variable used to make the functon currently being generated accessible from anywhere
static symbol_t *current_function;

// Prints the entry point. preamble, statements and epilouge of the given function
static void generate_function(symbol_t *function)
{
  current_function = function;
  // TODO: 2.3

  // TODO: 2.3.1 Do the prologue, including call frame building and parameter pushing
  // Tip: use the definitions REGISTER_PARAMS and NUM_REGISTER_PARAMS at the top of this file
  LABEL(".%s", function->name);

  PUSHQ(RBP);
  MOVQ(RSP, RBP);

  for (size_t i = 0; i < NUM_REGISTER_PARAMS && i < FUNC_PARAM_COUNT(function); ++i)
  {
    PUSHQ(REGISTER_PARAMS[i]);
  }

  for (size_t i = 0; i < function->function_symtable->n_symbols; ++i)
  {
    symbol_t *sym = function->function_symtable->symbols[i];
    if (function->function_symtable->symbols[i]->type == SYMBOL_LOCAL_VAR)
    {
      PUSHQ("$0");
    }
  }

  // TODO: 2.4 the function body can be sent to generate_statement()
  for (size_t i = 0; i < function->node->n_children; ++i)
  {
    generate_statement(function->node->children[i]);
  }

  // TODO: 2.3.2
  LABEL(".%s.epilogue", function->name);
  MOVQ(RBP, RSP);
  POPQ(RBP);
  RET;
}

// Generates code for a function call, which can either be a statement or an expression
static void generate_function_call(node_t *call)
{
  // TODO 2.4.3

  for (int i = call->children[1]->n_children - 1; i >= 0; --i)
  {
    generate_expression(call->children[1]->children[i]);
    PUSHQ(RAX);
  }
  for (size_t i = 0; i < NUM_REGISTER_PARAMS && i < call->children[1]->n_children; ++i)
  {
    POPQ(REGISTER_PARAMS[i]);
  }

  EMIT("call .%s", call->children[0]->symbol->name);
}

// Generates code to evaluate the expression, and place the result in %rax
static void generate_expression(node_t *expression)
{
  // TODO: 2.4.1 Generate code for evaluating the given expression.
  // (The candidates are NUMBER_LITERAL, IDENTIFIER, ARRAY_INDEXING, OPERATOR and FUNCTION_CALL)

  switch (expression->type)
  {
  case NUMBER_LITERAL:
    EMIT("movq $%ld, %s", expression->data.number_literal, RAX);
    break;
  case IDENTIFIER:
    switch (expression->symbol->type)
    {
    case SYMBOL_LOCAL_VAR:
      size_t offset = expression->symbol->sequence_number * 8 + 8;
      if (FUNC_PARAM_COUNT(current_function) > 6)
      {
        offset = (6 + (expression->symbol->sequence_number - FUNC_PARAM_COUNT(current_function))) * 8 + 8;
      }
      EMIT("movq -%zu(%s), %s", offset, RBP, RAX);
      break;
    case SYMBOL_PARAMETER:
      if (expression->symbol->sequence_number > 5)
      {
        EMIT("movq %zu(%s), %s", (expression->symbol->sequence_number - 6) * 8 + 16, RBP, RAX);
        break;
      }
      EMIT("movq -%zu(%s), %s", expression->symbol->sequence_number * 8 + 8, RBP, RAX);
      break;
    case SYMBOL_GLOBAL_VAR:
      EMIT("leaq %s%s, %s", expression->symbol->name, MEM(RIP), RAX);
      MOVQ(MEM(RAX), RAX);
      break;
    case SYMBOL_GLOBAL_ARRAY:
      EMIT("leaq %s%s, %s", expression->symbol->name, MEM(RIP), RAX);
      break;
    default:
      assert(0);
      break;
    }
    break;
  case ARRAY_INDEXING:

    generate_expression(expression->children[1]);
    EMIT("leaq %s%s, %s", expression->children[0]->symbol->name, MEM(RIP), RCX);
    EMIT("leaq (%s, %s, 8), %s", RCX, RAX, RCX);
    MOVQ(MEM(RCX), RAX);
    break;
  case OPERATOR:

    if (expression->n_children == 2)
    {
      generate_expression(expression->children[1]);
      PUSHQ(RAX);
      generate_expression(expression->children[0]);
      POPQ(RCX);
      switch (*(expression->data.operator))
      {
      case '/':
        CQO;
        IDIVQ(RCX);
        break;
      case '*':
        IMULQ(RCX, RAX);
        break;
      case '-':
        SUBQ(RCX, RAX);
        break;
      case '+':
        ADDQ(RCX, RAX);
        break;
      case '<':
        CMPQ(RCX, RAX);
        if (expression->data.operator[1] == '=')
        {
          SETLE(AL);
        }
        else
        {
          SETL(AL);
        }
        MOVZBQ(AL, RAX);
        break;
      case '>':
        CMPQ(RCX, RAX);
        if (expression->data.operator[1] == '=')
        {
          SETGE(AL);
        }
        else
        {
          SETG(AL);
        }
        MOVZBQ(AL, RAX);
        break;
      case '=':
        CMPQ(RCX, RAX);
        SETE(AL);
        MOVZBQ(AL, RAX);
        break;
      case '!':
        CMPQ(RCX, RAX);
        SETNE(AL);
        MOVZBQ(AL, RAX);
        break;
      default:
        assert(0);
        break;
      }
    }
    else
    {
      generate_expression(expression->children[0]);

      if (*(expression->data.operator) == '!')
      {
        EMIT("not %s", RAX);
      }
      else if (*(expression->data.operator) == '-')
      {
        NEGQ(RAX);
      }
    }

    break;
  case FUNCTION_CALL:
    generate_function_call(expression);
    break;
  default:
    assert(0);
    break;
  }
}

static void generate_assignment_statement(node_t *statement)
{
  // TODO: 2.4.2
  // You can assign to both local variables, global variables and function parameters.
  // Use the IDENTIFIER's symbol to find out what kind of symbol you are assigning to.
  // The left hand side of an assignment statement may also be an ARRAY_INDEXING node.
  // In that case, you must also emit code for evaluating the index being stored to

  if (statement->children[0]->type == ARRAY_INDEXING)
  {

    generate_expression(statement->children[0]->children[1]);
    EMIT("leaq %s(%s), %s", statement->children[0]->children[0]->symbol->name, RIP, RCX);
    EMIT("leaq (%s, %s, 8), %s", RCX, RAX, RCX);
    PUSHQ(RCX);
    generate_expression(statement->children[1]);
    POPQ(RCX);
    MOVQ(RAX, MEM(RCX));
    return;
  }

  generate_expression(statement->children[1]);

  switch (statement->children[0]->symbol->type)
  {
  case SYMBOL_LOCAL_VAR:
    size_t offset = statement->children[0]->symbol->sequence_number * 8 + 8;
    if (FUNC_PARAM_COUNT(current_function) > 6)
    {
      offset = (6 + (statement->children[0]->symbol->sequence_number - FUNC_PARAM_COUNT(current_function))) * 8 + 8;
    }
    EMIT("movq %s, -%zu(%s)", RAX, offset, RBP);
    break;
  case SYMBOL_PARAMETER:
    if (statement->children[0]->symbol->sequence_number > 5)
    {
      EMIT("movq %s, %zu(%s)", RAX, (statement->children[0]->symbol->sequence_number - 6) * 8 + 16, RBP);
      break;
    }
    EMIT("movq %s, -%zu(%s)", RAX, statement->children[0]->symbol->sequence_number * 8 + 8, RBP);
    break;
  case SYMBOL_GLOBAL_VAR:
  case SYMBOL_GLOBAL_ARRAY:
    EMIT("leaq %s(%s), %s", statement->children[0]->symbol->name, RIP, RCX);
    MOVQ(RAX, MEM(RCX));
    break;
  default:
    assert(0);
    break;
  }
}

static void generate_print_statement(node_t *statement)
{
  // TODO: 2.4.4
  // Remember to call safe_printf instead of printf

  for (size_t i = 0; i < statement->children[0]->n_children; ++i)
  {
    if (statement->children[0]->children[i]->type == STRING_LIST_REFERENCE)
    {
      EMIT("leaq .string%ld(%s), %s", statement->children[0]->children[i]->data.string_list_index, RIP, RSI);
      EMIT("leaq strout(%s), %s", RIP, RDI);
    }
    else
    {
      generate_expression(statement->children[0]->children[i]);
      MOVQ(RAX, RSI);
      EMIT("leaq intout(%s), %s", RIP, RDI);
    }

    EMIT("call safe_printf");
  }
  MOVQ("$'\\n'", RDI);
  EMIT("call putchar");
}

static void generate_return_statement(node_t *statement)
{
  // TODO: 2.4.5 Evaluate the return value, store it in %rax and jump to the function epilogue
  assert(statement->n_children == 1);
  generate_expression(statement->children[0]);
  EMIT("jmp .%s.epilogue", current_function->name);
}

// Recursively generate the given statement node, and all sub-statements.
static void generate_statement(node_t *node)
{
  if (node == NULL)
    return;

  // TODO: 2.4 Generate instructions for statements.
  // The candidates are BLOCK, ASSIGNMENT_STATEMENT, PRINT_STATEMENT, RETURN_STATEMENT,
  // FUNCTION_CALL
  switch (node->type)
  {
  case ASSIGNMENT_STATEMENT:
    generate_assignment_statement(node);
    break;
  case PRINT_STATEMENT:
    generate_print_statement(node);
    break;
  case RETURN_STATEMENT:
    generate_return_statement(node);
    break;
  case FUNCTION_CALL:
    generate_function_call(node);
    break;
  case BLOCK:

  default:
    for (size_t i = 0; i < node->n_children; ++i)
    {
      generate_statement(node->children[i]);
    }
    break;
  }
}

static void generate_safe_printf(void)
{
  LABEL("safe_printf");

  PUSHQ(RBP);
  MOVQ(RSP, RBP);
  // This is a bitmask that abuses how negative numbers work, to clear the last 4 bits
  // A stack pointer that is not 16-byte aligned, will be moved down to a 16-byte boundary
  ANDQ("$-16", RSP);
  EMIT("call printf");
  // Cleanup the stack back to how it was
  MOVQ(RBP, RSP);
  POPQ(RBP);
  RET;
}

// Generates the scaffolding for parsing integers from the command line, and passing them to the
// entry point of the VSL program. The VSL entry function is specified using the parameter "first".
static void generate_main(symbol_t *first)
{
  // Make the globally available main function
  LABEL("main");

  // Save old base pointer, and set new base pointer
  PUSHQ(RBP);
  MOVQ(RSP, RBP);

  // Which registers argc and argv are passed in
  const char *argc = RDI;
  const char *argv = RSI;

  const size_t expected_args = FUNC_PARAM_COUNT(first);

  SUBQ("$1", argc); // argc counts the name of the binary, so subtract that
  EMIT("cmpq $%ld, %s", expected_args, argc);
  JNE("ABORT"); // If the provdied number of arguments is not equal, go to the abort label

  if (expected_args == 0)
    goto skip_args; // No need to parse argv

  // Now we emit a loop to parse all parameters, and push them to the stack,
  // in right-to-left order

  // First move the argv pointer to the vert rightmost parameter
  EMIT("addq $%ld, %s", expected_args * 8, argv);

  // We use rcx as a counter, starting at the number of arguments
  MOVQ(argc, RCX);
  LABEL("PARSE_ARGV"); // A loop to parse all parameters
  PUSHQ(argv);         // push registers to caller save them
  PUSHQ(RCX);

  // Now call strtol to parse the argument
  EMIT("movq (%s), %s", argv, RDI); // 1st argument, the char *
  MOVQ("$0", RSI);                  // 2nd argument, a null pointer
  MOVQ("$10", RDX);                 // 3rd argument, we want base 10
  EMIT("call strtol");

  // Restore caller saved registers
  POPQ(RCX);
  POPQ(argv);
  PUSHQ(RAX); // Store the parsed argument on the stack

  SUBQ("$8", argv);        // Point to the previous char*
  EMIT("loop PARSE_ARGV"); // Loop uses RCX as a counter automatically

  // Now, pop up to 6 arguments into registers instead of stack
  for (size_t i = 0; i < expected_args && i < NUM_REGISTER_PARAMS; i++)
    POPQ(REGISTER_PARAMS[i]);

skip_args:

  EMIT("call .%s", first->name);
  MOVQ(RAX, RDI);    // Move the return value of the function into RDI
  EMIT("call exit"); // Exit with the return value as exit code

  LABEL("ABORT"); // In case of incorrect number of arguments
  EMIT("leaq errout(%s), %s", RIP, RDI);
  EMIT("call puts"); // print the errout string
  MOVQ("$1", RDI);
  EMIT("call exit"); // Exit with return code 1

  generate_safe_printf();

  // Declares global symbols we use or emit, such as main, printf and putchar
  DIRECTIVE("%s", ASM_DECLARE_SYMBOLS);
}
