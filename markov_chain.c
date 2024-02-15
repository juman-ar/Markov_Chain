#include "markov_chain.h"

Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr){
  //checks if the markov chain is not null
  if (markov_chain==NULL || markov_chain->database==NULL){
    return NULL;
  }
  //iterates on the database and searches for the data given to the function
  Node *ind_ptr= markov_chain->database->first;
  while(NULL != ind_ptr){
    if (markov_chain->comp_func(ind_ptr->data->data,data_ptr)==0){
      return ind_ptr;
    }
    //move to the next node in the database
    ind_ptr= ind_ptr->next;
  }
  return NULL;
}


MarkovNode *build_markov_node(void *data_ptr, MarkovChain*markov_chain){
  //allocating memory for the new markov node.
  MarkovNode *markov_node= calloc (1,sizeof (MarkovNode));
  if( markov_node==NULL){
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  //copy the data into a new variable
  void *data=markov_chain->copy_func(data_ptr);
  if(data== NULL){
    free (markov_node);
    return NULL;
  }
  //initialize the fields of the markov node
  markov_node->data=data;
  markov_node->next_node_counter=NULL;
  markov_node->counter_list_length=0;
  return markov_node;
}


Node* add_to_database(MarkovChain *markov_chain, void *data_ptr){
  //searches for the data in the database.
  Node* data_node= get_node_from_database (markov_chain,data_ptr);

  //if the data isn't in the database.
  if (data_node==NULL){
    //make a new markov node and add it to the database
    MarkovNode *markov_node= build_markov_node (data_ptr,markov_chain);
    if(markov_node !=NULL){
      if(!add (markov_chain->database,markov_node)){
        return markov_chain->database->last;
      }
      else{
        free (markov_node->data);
        free (markov_node);
      }
    }
  }
  //if the data is already in the database, return it's markov node.
  return data_node;
}


NextNodeCounter *find_next_node_counter(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain){
  NextNodeCounter *counter= first_node->next_node_counter;
  for(int ind=0; ind<first_node->counter_list_length;ind++){
    if (markov_chain->comp_func(counter[ind].markov_node->data,
                                second_node->data)==0){
      return &counter[ind];
    }
  }
  return NULL;
}


bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain){
  if(first_node->data==NULL|| second_node->data==NULL){
    return false;
  }

  if (first_node->next_node_counter == NULL){
    NextNodeCounter *new_counter = calloc (1, sizeof (NextNodeCounter));
    if (new_counter == NULL)
    {
      printf (ALLOCATION_ERROR_MASSAGE);
      return false;
    }
    new_counter->markov_node = second_node;
    new_counter->count = 1;
    first_node->next_node_counter = new_counter;
    first_node->counter_list_length++;
    return true;
  }

  NextNodeCounter *next = find_next_node_counter (first_node,second_node,
                                                  markov_chain);
  if (next==NULL){
    int len= first_node->counter_list_length; //counter_length (first_node);
    NextNodeCounter *new_count_list= realloc (first_node->next_node_counter,
      sizeof(NextNodeCounter)*(len+1));
    if (new_count_list==NULL){
      printf (ALLOCATION_ERROR_MASSAGE);
      return false;
    }
    first_node->next_node_counter= new_count_list;
    first_node->next_node_counter[len].markov_node=second_node;
    first_node->next_node_counter[len].count=1;
    first_node->counter_list_length++;
    return true;
  }

  next->count+=1;
  return true;
}

void free_markov_chain(MarkovChain **markov_chain){
  if(markov_chain == NULL || *markov_chain == NULL){
    return;
  }
  MarkovChain *markov_chain_ptr = *markov_chain;
  LinkedList *database = markov_chain_ptr->database;
  if(database==NULL){
    free (markov_chain_ptr);
    return;
  }
  Node *current = database->first;
  Node *temp=NULL;

  while(current != NULL) {
    MarkovNode *markov_node = (MarkovNode *)current->data;
    if(markov_node->data != NULL){
      markov_chain_ptr->free_data(markov_node->data);
    }
   if(markov_node->next_node_counter != NULL){
     free (markov_node->next_node_counter);
   }

    free(markov_node);
    temp = current;
    current = current->next;
    free(temp);
    temp =NULL;
  }
  free(database);
  free(*markov_chain);
  *markov_chain = NULL;
}


int get_random_number(int max_num){
  return rand()%max_num;
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
  Node *cur_node = markov_chain->database->first;
  while (NULL != cur_node){
    int random_num = get_random_number (markov_chain->database->size);
    for (int i = 0; i < random_num; i++){
      cur_node = cur_node->next;
    }
    if (!markov_chain->is_last (cur_node->data->data)){
      return cur_node->data;
    }
    else{
      cur_node = markov_chain->database->first;
    }
  }
  return cur_node->data;
}


int count_sum(MarkovNode* markov_node){
  int sum=0;
  NextNodeCounter *counter= markov_node->next_node_counter;

  if(counter==NULL){
    return 0;
  }
 // while (curr_count!=NULL){
 for(int i=0; i< markov_node->counter_list_length;i++){
    sum+=counter[i].count;
  }
  return sum;
}

//MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr){
//  int total_count = count_sum (state_struct_ptr);
//  if(total_count==0){
//    return NULL;
//  }
//
//  int random_num= get_random_number (total_count);
//  int curr_ind=-1;
//  NextNodeCounter *curr_node_count=state_struct_ptr->next_node_counter;
//  while(curr_ind < total_count){
//    curr_ind+=curr_node_count->count;
//
//    if(curr_ind >= random_num){
//      break;
//    }
//    curr_node_count++;
//    if(curr_node_count==NULL){
//      return NULL;
//    }
//  }
//  return curr_node_count->markov_node;
//}

MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr) {
  int total_count = count_sum(state_struct_ptr);
  if(total_count == 0) {
    return NULL;
  }

  int random_num = get_random_number(total_count);
  int sum = 0;
  for(int i = 0; i < state_struct_ptr->counter_list_length; i++) {
    sum += state_struct_ptr->next_node_counter[i].count;
    if(sum > random_num) {
      return state_struct_ptr->next_node_counter[i].markov_node;
    }
  }
  return NULL;
}


void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length){

  MarkovNode *curr_node= first_node;
  if(curr_node==NULL){
    curr_node= get_first_random_node (markov_chain);
  }

  for(int data_read=0; data_read < max_length; data_read++){

    markov_chain->print_func(curr_node->data);
    if(markov_chain->is_last(curr_node->data)|| data_read==max_length-1){
      return;
    }
    curr_node= get_next_random_node (curr_node);
    if(curr_node==NULL){
      return;
    }
  }
}

