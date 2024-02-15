#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY (-1)
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define ARG_NUM 3
#define ARG_ERROR "USAGE: the number of arguments is not valid."
#define BASE 10
#define LAST_CELL_NUM 100
/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the
    // ladder in case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case
    // there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database)
{
    printf("%s", error_msg);
    if (database != NULL)
    {
        free_markov_chain(database);
    }
    return EXIT_FAILURE;
}


static int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                free(cells[j]);
            }
            handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
    Cell* cells[BOARD_SIZE];
    if(create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain,cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to,cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain, cells[index_to])
                    ->data;
            add_node_to_counter_list(from_node, to_node, markov_chain);
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain, cells[index_to])
                        ->data;
                add_node_to_counter_list(from_node, to_node, markov_chain);
            }
        }
    }
    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

static bool is_last_cell(void *data){
  Cell *cell_data= (Cell*)data;
  if(cell_data->number == LAST_CELL_NUM){
    return true;
  }
  return false;
}

static void print_cell(void* data){
  Cell * cell= (Cell*)data;
  if (cell->number== LAST_CELL_NUM){
    printf ("[%d]",cell->number);
    return;
  }
  if(cell->ladder_to==EMPTY && cell->snake_to== EMPTY){
    printf ("[%d] -> ",cell->number);
  }
  else if (cell->snake_to!= EMPTY){
    printf ("[%d]-snake to %d -> ",cell->number,cell->snake_to);
  }
  else if (cell->ladder_to!= EMPTY){
    printf ("[%d]-ladder to %d -> ",cell->number,cell->ladder_to);
  }
}

static int comp_cells(void * data1, void * data2){
  Cell *first_cell= (Cell*)data1;
  Cell* second_cell= (Cell*)data2;
  if(first_cell->number > second_cell->number){
    return 1;
  }
  else if(first_cell->number == second_cell->number){
    return 0;
  }
  else if(first_cell->number < second_cell->number){
    return -1;
  }
  return 0;
}

static void free_cell(void * data){
  Cell *cell= (Cell *)data;
  free (cell);
}

static void * copy_cell(void * data){
  Cell *srs= (Cell*)data;
  Cell* dest= malloc (sizeof (Cell));
  if(dest== NULL){
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  dest->number= srs->number;
  dest->snake_to=srs->snake_to;
  dest->ladder_to=srs->ladder_to;
  return dest;
}

MarkovChain* build_database(){
  MarkovChain *markov_chain= malloc (sizeof(MarkovChain));
  if (markov_chain==NULL){
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  LinkedList *database= malloc (sizeof (LinkedList));
  if (database==NULL){
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  database->first=NULL;
  database->last=NULL;
  database->size=0;

  markov_chain->database=database;
  markov_chain->print_func = print_cell;
  markov_chain->comp_func = comp_cells;
  markov_chain->free_data= free_cell;
  markov_chain->copy_func= copy_cell;
  markov_chain->is_last= is_last_cell;

  int success = fill_database (markov_chain);
  if(success== EXIT_FAILURE){
    free_markov_chain (&markov_chain);
    markov_chain=NULL;
    return NULL;
  }
  return markov_chain;
}

void print_paths(MarkovChain* markov_chain, int paths_num){
MarkovNode * first_cell= markov_chain->database->first->data;
for(int i=0; i<paths_num;i++){
  printf ("Random Walk %d: ", i+1);
  generate_random_sequence(markov_chain, first_cell,
                           MAX_GENERATION_LENGTH);
  printf ("\n");
  }
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[]){
  if(argc!= ARG_NUM){
    printf (ARG_ERROR);
    return EXIT_FAILURE;
  }

  unsigned int seed= strtol (argv[1], NULL, BASE);
  int paths_num = (int) strtol (argv[2],NULL, BASE);
  srand (seed);

  MarkovChain *markov_chain= build_database ();
  if(markov_chain==NULL){
    return EXIT_FAILURE;
  }
  print_paths (markov_chain,paths_num);
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}
