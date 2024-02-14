tweets: tweets_generator.c markov_chain.c linked_list.c
	gcc tweets_generator.c markov_chain.c linked_list.c -o tweets_generator

snake: snakes_and_ladders.c markov_chain.c linked_list.c
	gcc snakes_and_ladders.c markov_chain.c linked_list.c -o snakes_and_ladders