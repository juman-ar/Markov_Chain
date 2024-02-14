generate_tweets: tweets_generator.c markov_chain.c linked_list.c
	gcc tweets_generator.c markov_chain.c linked_list.c -o tweets_generator

snakes_and_ladders: snakes_and_ladders.c markov_chain.c linked_list.c
	gcc snakes_and_ladders.c markov_chain.c linked_list.c -o snakes_and_ladders