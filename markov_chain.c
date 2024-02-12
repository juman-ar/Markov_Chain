#include "markov_chain.h"

Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr){
  if (markov_chain==NULL){
    return NULL;
  }
  Node *ind_ptr= markov_chain->database->first;
  while(NULL != ind_ptr){
    if (markov_chain->comp_func(ind_ptr->data->data,data_ptr)==0){
      return ind_ptr;
    }
    ind_ptr= ind_ptr->next;
  }
  return NULL;
}

MarkovNode *build_markov_node(void *data_ptr, MarkovChain*markov_chain){
  MarkovNode *markov_node= calloc (1,sizeof (MarkovNode));
  if( markov_node==NULL){
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  //TODO: check the data allocation and copying the data
//  char* data= (char *)malloc (sizeof(data_ptr));
//  if(data== NULL){
//    printf (ALLOCATION_ERROR_MASSAGE);
//    return NULL;
//  }
  void *data=markov_chain->copy_func(data_ptr);
  markov_node->data=data;
  markov_node->next_node_counter=NULL;
  return markov_node;
}

Node* add_to_database(MarkovChain *markov_chain, void *data_ptr){
  Node* data_node= get_node_from_database (markov_chain,data_ptr);

  if (data_node==NULL){
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
  return data_node;
}

NextNodeCounter *find_next_node_counter(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain){
  NextNodeCounter *ind_counter= first_node->next_node_counter;
  while(NULL != ind_counter){
    if (markov_chain->comp_func(ind_counter->markov_node->data,
                                second_node->data)){
      return ind_counter;
    }
    ind_counter+=1;
  }
  return NULL;
}

int counter_length(MarkovNode* markov_node){
  int count=0;
  NextNodeCounter *ind= markov_node->next_node_counter;
  while(ind!=NULL){
    count+=1;
    ind+=1;
  }
  return count;
}

bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain){
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
    return true;
  }

  NextNodeCounter *next = find_next_node_counter (first_node,second_node,
                                                  markov_chain);
  if (next==NULL){
    int len= counter_length (first_node);
    NextNodeCounter *new_count= realloc (first_node->next_node_counter,
      sizeof(NextNodeCounter)*(len+1));
    if (new_count==NULL){
      printf (ALLOCATION_ERROR_MASSAGE);
      return false;
    }
    first_node->next_node_counter= new_count;
    first_node->next_node_counter[len].markov_node=second_node;
    first_node->next_node_counter[len].count=1;
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
  Node *current = database->first, *temp;

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
  }

  free(database);
  free(markov_chain);
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
  NextNodeCounter *curr_count= markov_node->next_node_counter;
  while (curr_count!=NULL){
    sum+=curr_count->count;
    curr_count+=1;
  }
  return sum;
}
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr){
  int total_count = count_sum (state_struct_ptr);
  if(total_count==0){
    return NULL;
  }
  int random_num= get_random_number (total_count);
  int curr_ind=-1;
  NextNodeCounter *curr_node_count=state_struct_ptr->next_node_counter;
  while(curr_ind < total_count){
    curr_ind+=curr_node_count->count;

    if(curr_ind >= random_num){
      break;
    }
    curr_node_count++;
    if(curr_node_count==NULL){
      return NULL;
    }
  }
  return curr_node_count->markov_node;
}

void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length){
  MarkovNode *curr_node= first_node;
  if(curr_node==NULL){
    curr_node= get_first_random_node (markov_chain);
  }
  for(int words_read=0; words_read<max_length;words_read++){
    markov_chain->print_func(curr_node->data);
    if(markov_chain->is_last(curr_node->data)|| words_read==max_length-1){
      return;
    }
    curr_node= get_next_random_node (curr_node);
    if(curr_node==NULL){
      return;
    }
  }
}

