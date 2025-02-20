#include "vslc.h"

// Global root for abstract syntax tree
node_t *root;

// Declarations of helper functions defined further down in this file
static void node_print(node_t *node, int nesting);
static node_t *constant_fold_subtree(node_t *node);
static bool remove_unreachable_code(node_t *node);
static void destroy_subtree(node_t *discard);
static void node_finalize(node_t *discard);

// Initialize a node with the given type and children
node_t *node_create(node_type_t type, size_t n_children, ...)
{
  node_t *result = malloc(sizeof(node_t));

  // Initialize every field in the struct
  *result = (node_t){
      .type = type,
      .n_children = n_children,
      .children = malloc(n_children * sizeof(node_t *)),
  };

  // Read each child node from the va_list
  va_list child_list;
  va_start(child_list, n_children);
  for (size_t i = 0; i < n_children; i++)
  {
    result->children[i] = va_arg(child_list, node_t *);
  }
  va_end(child_list);

  return result;
}

// Append an element to the given LIST node, returns the list node
node_t *append_to_list_node(node_t *list_node, node_t *element)
{
  assert(list_node->type == LIST);

  // Calculate the minimum size of the new allocation
  size_t min_allocation_size = list_node->n_children + 1;

  // Round up to the next power of two
  size_t new_allocation_size = 1;
  while (new_allocation_size < min_allocation_size)
    new_allocation_size *= 2;

  // Resize the allocation
  list_node->children = realloc(list_node->children, new_allocation_size * sizeof(node_t *));

  // Insert the new element and increase child count by 1
  list_node->children[list_node->n_children] = element;
  list_node->n_children++;

  return list_node;
}

// Outputs the entire syntax tree to the terminal
void print_syntax_tree(void)
{
  // If the environment variable GRAPHVIZ_OUTPUT is set, print a GraphViz graph in the dot format
  if (getenv("GRAPHVIZ_OUTPUT") != NULL)
    graphviz_node_print(root);
  else
    node_print(root, 0);
}

// Performs constant folding and removes unconditional conditional branches
void constant_fold_syntax_tree(void)
{
  root = constant_fold_subtree(root);
}

// Removes code that is never reached due to return and break statements.
// Also ensures execution never reaches the end of a function without reaching a return statement.
void remove_unreachable_code_syntax_tree(void)
{
  for (size_t i = 0; i < root->n_children; i++)
  {
    node_t *child = root->children[i];
    if (child->type != FUNCTION)
      continue;

    node_t *function_body = child->children[2];

    bool has_return = remove_unreachable_code(function_body);

    // If the function body is not guaranteed to call return, we wrap it in a BLOCK like so:
    // {
    //   original_function_body
    //   return 0
    // }
    if (!has_return)
    {
      node_t *zero_node = node_create(NUMBER_LITERAL, 0);
      zero_node->data.number_literal = 0;
      node_t *return_node = node_create(RETURN_STATEMENT, 1, zero_node);
      node_t *statement_list = node_create(LIST, 2, function_body, return_node);
      node_t *new_function_body = node_create(BLOCK, 1, statement_list);
      child->children[2] = new_function_body;
    }
  }
}

// Frees all memory held by the syntax tree
void destroy_syntax_tree(void)
{
  destroy_subtree(root);
  root = NULL;
}

// The rest of this file contains private helper functions used by the above functions

// Prints out the given node and all its children recursively
static void node_print(node_t *node, int nesting)
{
  // Indent the line based on how deep the node is in the syntax tree
  printf("%*s", nesting, "");

  if (node == NULL)
  {
    printf("(NULL)\n");
    return;
  }

  printf("%s", NODE_TYPE_NAMES[node->type]);

  // For nodes with extra data, include it in the printout
  switch (node->type)
  {
  case OPERATOR:
    printf(" (%s)", node->data.operator);
    break;
  case IDENTIFIER:
    printf(" (%s)", node->data.identifier);
    break;
  case NUMBER_LITERAL:
    printf(" (%ld)", node->data.number_literal);
    break;
  case STRING_LITERAL:
    printf(" (%s)", node->data.string_literal);
    break;
  case STRING_LIST_REFERENCE:
    printf(" (%zu)", node->data.string_list_index);
    break;
  default:
    break;
  }

  putchar('\n');

  // Recursively print children, with some more indentation
  for (size_t i = 0; i < node->n_children; i++)
    node_print(node->children[i], nesting + 1);
}

// Constant folds the given OPERATOR node, if all children are NUMBER_LITERAL
static node_t *constant_fold_operator(node_t *node)
{
  assert(node->type == OPERATOR);

  /*
   * TODO: 2.2 Implement constant folding of OPERATOR nodes.
   *
   * This is the most classic form of constant folding.
   * When all operands of an operator are know at compile time, such as 4 + 20,
   * the calculation can be performed at compile time, and the whole expression can be replaced
   * by 24.
   *
   * In our compiler, a value is known at compile time if it is a NUMBER_LITERAL node.
   * If all children of an OPERATOR node have this type, we can perform the operation on the
   * numbers. The OPERATOR node can then be replaced with a NUMBER_LITERAL node holding the result
   * as its data.
   *
   * Remember:
   *  - the values of NUMBER_LITERALs are stored in number_node->data.number_literal.
   *  - the operator of an OPERATOR node is stored in the field node->data.operator, and is a
   * string.
   *
   * Checking if a string is equal to another string can be done using strcmp(str1, str2), which
   * returns 0 if the strings are identical.
   */
  for (size_t i = 0; i < node->n_children; ++i)
  {
    if (node->children[i]->type != NUMBER_LITERAL)
    {
      return node;
    }
  }

  /*
  I investigated the assembly after compiling with gcc/clang -O3 on my x86 linux machine. The result is quite dissapointing,
  the switch statement is not optimized and a bunch of if else statements are used instead.
  This does somewhat make sense since constructing a jump table will consume 0xffff*sizeof(void *)=524280 bytes.
  Which will make the binary much bigger due to a single switch statement.

  Using modulo 217 gives 0 collisions and shrinks the values to a maximum of 217 which the compiler can generate a jump table for
  */

  switch ((*(uint16_t *)node->data.operator) % 217) // Hack for not using strcmp, which is slower. This assumes that all strings are of length 1 or 2
  {
  case (((uint16_t)'\0' << 8) | '+') % 217: // "+"
    node->data.number_literal = node->children[0]->data.number_literal + node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'\0' << 8) | '-') % 217: // "-"
    if (node->n_children == 1)
    {
      node->data.number_literal = -node->children[0]->data.number_literal;
      break;
    }
    node->data.number_literal = node->children[0]->data.number_literal - node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'\0' << 8) | '*') % 217: // "*"
    node->data.number_literal = node->children[0]->data.number_literal * node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'\0' << 8) | '/') % 217: // "/"
    node->data.number_literal = node->children[0]->data.number_literal / node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'=' << 8) | '=') % 217: // "=="
    node->data.number_literal = node->children[0]->data.number_literal == node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'=' << 8) | '!') % 217: // "!="
    node->data.number_literal = node->children[0]->data.number_literal != node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'\0' << 8) | '<') % 217: // "<"
    node->data.number_literal = node->children[0]->data.number_literal < node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'=' << 8) | '<') % 217: // "<="
    node->data.number_literal = node->children[0]->data.number_literal <= node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'\0' << 8) | '>') % 217: // ">"
    node->data.number_literal = node->children[0]->data.number_literal > node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'=' << 8) | '>') % 217: // ">="
    node->data.number_literal = node->children[0]->data.number_literal >= node->children[1]->data.number_literal;
    break;
  case (((uint16_t)'\0' << 8) | '!') % 217: // "!"
    node->data.number_literal = !node->children[0]->data.number_literal;
    break;
  default:
    return node;
  }

  node->type = NUMBER_LITERAL;
  for (size_t i = 0; i < node->n_children; ++i)
  {
    destroy_subtree(node->children[i]);
  }
  free(node->children);
  node->children = NULL;
  node->n_children = 0;

  return node;
}

// If the condition of the given if node is a NUMBER_LITERAL, the if is replaced by the taken
// branch. If the if condition is false, and the if has no else-body, NULL is returned.
static node_t *constant_fold_if(node_t *node)
{
  assert(node->type == IF_STATEMENT);

  /*
   * TODO: 2.3 Implement "constant folding" on if statements
   *
   * The first child of an IF_STATEMENT node is its condition.
   * If the condition node has type NUMBER_LITERAL, then we already know whether the if is going to
   * be taken or not:
   *  - 0 means not taken
   *  - any other value means taken
   *
   * If we know that the if will be taken, we can remove the IF_STATEMENT from the program.
   * Instead we return the node that contains its then-body (the IF_STATEMENT node's second child).
   *
   * Likewise, if the know that the if will not be taken, we can replace the entire IF_STATEMENT
   * with its else-body. Not all IFs have an else body, so check that the IF_STATEMENT node has 3
   * children first. If the IF_STATEMENT is never taken, but it does not have an else-body, you can
   * return NULL.
   *
   * Be very careful about cleaning up any parts of the tree that are no longer used.
   * The easiest way of doing this is to "detach" any part you want to keep,
   * and then sending the rest of the IF_STATEMENT node to destroy_subtree.
   */

  if (node->children[0]->type != NUMBER_LITERAL)
  {
    return node;
  }

  node_t *ret = NULL;
  if (node->children[0]->data.number_literal)
  {
    destroy_subtree(node->children[0]);
    if (node->n_children > 2)
    {
      destroy_subtree(node->children[2]);
    }
    ret = node->children[1];
  }
  else
  {
    destroy_subtree(node->children[0]);
    destroy_subtree(node->children[1]);
    if (node->n_children > 2)
    {
      ret = node->children[2];
    }
  }
  node_finalize(node);

  return ret;
}

// If the condition of the given while node is a NUMBER_LITERAL, and it is false (0),
// we remove the entire while node and return NULL instead.
// Loops that look like while(true) { ... } are kept as is. They may have a break inside
static node_t *constant_fold_while(node_t *node)
{
  assert(node->type == WHILE_STATEMENT);

  /*
   * TODO: 2.4 Implement "constant folding" on while statements
   *
   * Just like with IF_STATEMENTS, the first child of a WHILE_STATEMENT is the condition.
   *
   * If the condition is a NUMBER_LITERAL holding a constant value 0, we know that the while is
   * never taken, and it is safe to replace the whole WHILE_STATEMENT subtree with NULL.
   */

  if (node->children[0]->type != NUMBER_LITERAL || node->children[0]->data.number_literal)
  {
    return node;
  }

  destroy_subtree(node);
  return NULL;
}

// Does constant folding on the subtreee rooted at the given node.
// Returns the root of the new subtree.
// Any node that is detached from the tree by this operation must be freed, to avoid memory leaks.
static node_t *constant_fold_subtree(node_t *node)
{
  if (node == NULL)
  {
    return node;
  }

  /*
   * TODO: 2.1 Implement constant folding of subtrees.
   * This function should recursively perform constant folding on the subtree rooted at node.
   * Constant folding works in a bottom-up manner, so handle subtrees first.
   *
   * Once all subtrees are handled, check if the current node is a possible candidate for contant
   * folding. If it is, hand it over to one of the three constant_fold_X functions above.
   *
   * Note that this function returns the new root of the constant folded subtree.
   * In most cases the root node is just node, but you are free to return a new node if you want to
   * replace a subtree. Just remember that any node that becomes detached from the tree, will not be
   * reached during regular tree cleanup. Therefore it is your responsibility to clean up any such
   * nodes to reclaim their memory.
   */

  for (size_t i = 0; i < node->n_children; ++i)
  {
    node->children[i] = constant_fold_subtree(node->children[i]);
  }
  switch (node->type)
  {
  case OPERATOR:
    node = constant_fold_operator(node);
    break;
  case IF_STATEMENT:
    node = constant_fold_if(node);
    break;
  case WHILE_STATEMENT:
    node = constant_fold_while(node);
    break;
  default:
    break;
  }

  return node;
}

// Operates on the statement given as node, and any sub-statements it may have.
// Returns true if execution of the given statement is guaranteed to interrupt execution
// through either a return statement or a break statement.
// When node is a BLOCK, any statements that come after such an interrupting statement are removed.
static bool remove_unreachable_code(node_t *node)
{
  if (node == NULL)
    return false;

  switch (node->type)
  {
  case RETURN_STATEMENT:
  case BREAK_STATEMENT:
    return true;
  case IF_STATEMENT:
  {
    if (node->n_children == 2)
    {
      // If the if only has a then-statement, it can not terminate execution
      remove_unreachable_code(node->children[1]);
      return false;
    }
    else
    {
      // If both the then-statement and the else-statement are interrupted
      // we know that the if itself is interrupting as well
      bool then_interrupts = remove_unreachable_code(node->children[1]);
      bool else_interrupts = remove_unreachable_code(node->children[2]);
      return then_interrupts && else_interrupts;
    }
  }
  case WHILE_STATEMENT:
  {
    // Even if the body of the while contains interrupting statements,
    // that is not a guarantee that the code after the while is unreachable.
    // The while may never be entered, for example, or the interrupting statement may be BREAK.
    remove_unreachable_code(node->children[1]);
    return false;
  }
  case BLOCK:
  {
    // The list of statements in a BLOCK is always the last child node
    node_t *statement_list = node->children[node->n_children - 1];

    /*
     * TODO 2.5: Implement unreachable code elimination
     *
     * A BLOCK node consists of a list of statements.
     * For each statement S, do a recursive call to remove_unreachable_code( S ).
     * If the call returns true, that means the statement S is guaranteed to abort the rest of the
     * block. This can for example be due to a RETURN_STATEMENT or a BREAK_STATEMENT. If that
     * happens, we know that the rest of the BLOCK is unreachable code. Remove all children of the
     * statement LIST that come after S by changing n_children. Make sure you clean up all nodes
     * that become detached from the LIST. Finally, return true, as the BLOCK contains a statement
     * that interrupts regular execution.
     */

    for (size_t i = 0; i < statement_list->n_children; ++i)
    {
      if (remove_unreachable_code(statement_list->children[i]))
      {
        while (--statement_list->n_children > i)
        {
          destroy_subtree(statement_list->children[statement_list->n_children]);
          statement_list->children[statement_list->n_children] = NULL;
        }
        ++statement_list->n_children;
        return true;
      }
    }

    // If we get here, none of the statements in the block are interrupting.
    return false;
  }
  default:
    return false;
  }
}

// Frees the memory owned by the given node, but does not touch its children
static void node_finalize(node_t *discard)
{
  if (discard == NULL)
  {
    return;
  }

  // Only free data if the data field is owned by the node
  switch (discard->type)
  {
  case IDENTIFIER:
    free(discard->data.identifier);
    break;
  case STRING_LITERAL:
    free(discard->data.string_literal);
    break;
  default:
    break;
  }
  free(discard->children);
  free(discard);
}

// Recursively frees the memory owned by the given node, and all its children
static void destroy_subtree(node_t *discard)
{
  if (discard == NULL)
  {
    return;
  }

  for (size_t i = 0; i < discard->n_children; i++)
  {
    destroy_subtree(discard->children[i]);
  }
  node_finalize(discard);
}

// Definition of the global string array NODE_TYPE_NAMES
const char *NODE_TYPE_NAMES[NODE_TYPE_COUNT] = {
#define NODE_TYPE(node_type) #node_type
#include "nodetypes.h"
};
