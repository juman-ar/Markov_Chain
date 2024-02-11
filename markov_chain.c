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

MarkovNode *build_markov_node(void *data_ptr){
  MarkovNode *markov_node= calloc (1,sizeof (MarkovNode));
  if( markov_node==NULL){
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  //TODO: check the data allocation and copying the data
  char* data= (char *)malloc (sizeof(data_ptr));
  if(data== NULL){
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  strcpy(data, data_ptr);
  markov_node->data=data;
  markov_node->next_node_counter=NULL;
  return markov_node;
}

Node* add_to_database(MarkovChain *markov_chain, void *data_ptr){
  Node* data_node= get_node_from_database (markov_chain,data_ptr);

  if (data_node==NULL){
    MarkovNode *markov_node= build_markov_node (data_ptr);
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

int find_next_node_counter(MarkovNode *first_node, MarkovNode
*second_node){

}

bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain){
  if(first_node->next_node_counter==NULL){
    NextNodeCounter *new_counter= calloc (1, sizeof (NextNodeCounter));
    if(new_counter==NULL){
      printf (ALLOCATION_ERROR_MASSAGE);
      return false;
    }
    new_counter->markov_node= second_node;
    new_counter->count=1;
    first_node->next_node_counter=new_counter;
    return true;
  }

}