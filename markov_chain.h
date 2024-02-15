#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MASSAGE "Allocation failure: Failed to allocate memory\n"


/***************************/
/*   insert typedefs here  */
/***************************/
typedef void (*Print)(void*);
typedef int (*Compare)(void*,void*);
typedef void (*Free_Func)(void *);
typedef void* (*Copy)(void *);
typedef bool (*Last)(void *);
/***************************/



/***************************/
/*        STRUCTS          */
/***************************/
struct NextNodeCounter;
typedef struct MarkovNode {
    void* data;
    struct NextNodeCounter * next_node_counter;
    int counter_list_length;
} MarkovNode;

typedef struct NextNodeCounter {
    MarkovNode *markov_node;
    int count;
} NextNodeCounter;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain {
    LinkedList *database;

    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    Print print_func;

    // pointer to a func that gets 2 pointers of generic data type(same one)
    // and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    Compare comp_func;

    // a pointer to a function that gets a pointer of generic data type
    // and frees it.
    // returns void.
    Free_Func free_data;

    // a pointer to a function that  gets a pointer of generic data type and
    // returns a newly allocated copy of it
    // returns a generic pointer.
    Copy copy_func;

    //  a pointer to function that gets a pointer of generic data type
    //  and returns:
    //      - true if it's the last state.
    //      - false otherwise.
    Last is_last;
} MarkovChain;


/**
 * allocates memory to build a new markov node and initialize it's fields.
 * @param data_ptr- a pointer to the data.
 * @param markov_chain
 * @return- a markov node, or NULL if memory allocation failed.
 */
MarkovNode *build_markov_node(void *data_ptr, MarkovChain*markov_chain);



/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain);


/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a random
 * markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_markov_chain(MarkovChain **markov_chain);


/**
* Check if second node is in counter list of the first node
 * @param markov_chain the chain to look in its database
 * @param first_node
 * @param second_node
 * @return Pointer to the NextNodeCounter of the second node if it is in the
 * counter list, NULL otherwise
 */
NextNodeCounter *find_next_node_counter(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);


/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);

/**
* Check if data_ptr is in database. If so, return the markov_node
 * wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr);

#endif /* MARKOV_CHAIN_H */
