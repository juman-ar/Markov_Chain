#include "markov_chain.h"
#include <string.h>

#define MAX_SENTENCE 1000
#define TWEET_MAX 20
#define READ_ALL_FILE (-1)
#define ARG1 4
#define ARG2 5
#define BASE 10

#define ARG_ERROR "USAGE: the number of arguments is not valid.\n"
#define FILE_ERROR "ERROR: failed to open the file.\n"

static bool is_closing_word(void *data){
  char* data_str= (char *)data;
  int len= (int) strlen ((char*)data_str);
  if(data_str[len-1]=="."){
    return true;
  }
  return false;
}

static void print_str(void* data){
  if(is_closing_word (data)){
    printf ("%s", (char*)data);
  }
  else{
    printf ("%s ",(char*)data);
  }
}

static int comp_str(void * data1, void * data2){
  return strcmp ((char*)data1, (char*)data2);
}

static void free_str(void * data){
  free ((char*)data);

}

static void * copy_str(void * data){
  char* dest= malloc (strlen ((char*)data));
  if(dest== NULL){
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  strcpy (dest, (char*)data);
  return (char*)dest;
}

static FILE *open_file(char* path){
  FILE *tweets_file= fopen (path,"r");
  if(tweets_file==NULL){
    printf (FILE_ERROR);
    return NULL;
  }
  return tweets_file;
}

static int fill_database(FILE*fp, int words_to_read,
                         MarkovChain* markov_chain){
  char line[MAX_SENTENCE];
  int words_read=0;
  char* word_ptr= NULL;

  while(fgets (line,MAX_SENTENCE,fp)&&
      ( words_read< words_to_read || words_to_read == READ_ALL_FILE)&&
      (word_ptr= strtok (line,"  \n\r"))){

    Node *current_node= add_to_database (markov_chain,word_ptr);
    if(current_node==NULL){
      return EXIT_FAILURE;
    }
    words_read++;
    while((words_to_read==READ_ALL_FILE || words_read<words_to_read )&&
        (word_ptr= strtok (NULL,"  \n\r"))){
      Node *node= add_to_database (markov_chain,word_ptr);
      if(node==NULL){
        return EXIT_FAILURE;
      }
      if(current_node!= NULL &&
      !markov_chain->is_last(current_node->data->data)){

        if(!add_node_to_counter_list (current_node->data,
                                      node->data,markov_chain)){
          return EXIT_FAILURE;
        }
      }
      current_node=node;
      words_read++;
    }
  }
  return EXIT_SUCCESS;
}


MarkovChain *make_database(FILE* tweets_file,int words_to_read){
  MarkovChain *markov_chain= calloc (1,sizeof(MarkovChain));
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
  markov_chain->print_func = print_str;
  markov_chain->comp_func = comp_str;
  markov_chain->free_data= free_str;
  markov_chain->copy_func= copy_str;
  markov_chain->is_last= is_closing_word;

  int success= fill_database (tweets_file, words_to_read, markov_chain);
  if(success== EXIT_FAILURE){
    free_markov_chain (&markov_chain);
    markov_chain=NULL;
    return NULL;
  }
  return markov_chain;
}

void print_tweets(MarkovChain* markov_chain, int tweets_num){
  for(int i=0;i<tweets_num;i++){
    printf ("Tweet %d: ", i+1);
    generate_random_sequence (markov_chain,NULL,TWEET_MAX);
    printf("\n");
  }
}

int main(int argc, char * argv[]){
  if(argc!=ARG1 && argc!= ARG2){
    printf (ARG_ERROR);
    return EXIT_FAILURE;
  }
  unsigned int seed= strtol (argv[1], NULL, BASE);
  int tweet_num = (int) strtol (argv[2],NULL, BASE);
  int word_count=READ_ALL_FILE;
  if(argc==ARG2){
    word_count= (int) strtol (argv[4], NULL, BASE);
  }
  srand (seed);

  FILE *tweets_file= open_file (argv[3]);
  if(tweets_file==NULL){
    return EXIT_FAILURE;
  }

  MarkovChain *markov_chain= make_database (tweets_file,word_count);
  if(markov_chain==NULL){
    return EXIT_FAILURE;
  }
  print_tweets (markov_chain,tweet_num);
  fclose (tweets_file);
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}
