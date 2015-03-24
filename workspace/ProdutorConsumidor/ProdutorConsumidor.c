/*
 * ProdutorConsumidor.c
 *
 *  Created on: 24/03/2015
 *      Author: ricardo
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/semaphore.h>

#define BUFF_SIZE   2		/* total number of slots */
#define NP          3		/* total number of producers */
#define NC          3		/* total number of consumers */
#define NITERS      6		/* number of items produced/consumed */

typedef struct {
	int buf[BUFF_SIZE]; /* shared var */
	int in; /* buf[in%BUFF_SIZE] is the first empty slot */
	int out; /* buf[out%BUFF_SIZE] is the first full slot */
	sem_t full; /* keep track of the number of full spots */
	sem_t empty; /* keep track of the number of empty spots */
	pthread_mutex_t mutex; /* enforce mutual exclusion to shared data */
} sbuf_t;

sbuf_t shared; //struct compartilhada

//produtor
void *Producer(void *arg) {
	int i, item, index;

	index = (int) arg + 1; //numero da thread

	for (i = 0; i < NITERS; i++) {

		/* Produce item */
		item = i + 65; //letra codigo asc

		/* Prepare to write item to buf */

		/* If there are no empty slots, wait */
		sem_wait(&shared.empty);
		/* If another thread uses the buffer, wait */
		pthread_mutex_lock(&shared.mutex);
		shared.buf[shared.in] = item; //primeiro item disponivel para ser consumido
		shared.in = (shared.in + 1) % BUFF_SIZE; //incrementa o buffer de entrada
		printf("    [P%d] Producing %c ...\n", index, item); //imprimi na tela o numero do produtor e a letra do item
		fflush(stdout); //limpa o buffer
		/* Release the buffer */
		pthread_mutex_unlock(&shared.mutex);
		/* Increment the number of full slots */
		sem_post(&shared.full);

		/* Interleave  producer and consumer execution */
		if (i % 2 == 1)
			sleep(1);
	}
	return NULL;
}

void *Consumer(void *arg) {
	/* Fill in the code here */
	int i, item, index;

	index = (int) arg + 1; //numero da thread

	for (i = NITERS; i > 0; i--) {

		/* Prepare to write item to buf */

		/* If there are no full slots, wait */
		sem_wait(&shared.full);
		/* If another thread uses the buffer, wait */
		pthread_mutex_lock(&shared.mutex);
		/* Consume item */
		item = shared.buf[shared.out]; //primeiro item disponivel para ser consumido
		shared.out = (shared.out + 1) % BUFF_SIZE; //incrementa o buffer de saida
		printf("------>[C%d] Consuming %c ...\n", index, item); //imprimi na tela o numero do produtor e a letra do item
		fflush(stdout); //limpa o buffer
		/* Release the buffer */
		pthread_mutex_unlock(&shared.mutex);
		/* Increment the number of empty slots */
		sem_post(&shared.empty);

		/* Interleave  producer and consumer execution */
		if (i % 2 == 1)
			sleep(1);
	}
	return NULL;
}

int main() {
	pthread_t idP, idC;
	int index;

	sem_init(&shared.full, 0, 0); //inicia o semaforo com buffer vazio
	sem_init(&shared.empty, 0, BUFF_SIZE); // inicia semaforo com 2 espaços disponiveis

	/* Insert code here to initialize mutex*/
	pthread_mutex_init(&shared.mutex, NULL);

	for (index = 0; index < NP; index++) {
		/* Create a new producer */
		pthread_create(&idP, NULL, Producer, (void*) index);
	}

	/* Insert code here to create NC consumers */
	for (index = 0; index < NC; index++) {
		/* Cria um novo consumidor */
		pthread_create(&idC, NULL, Consumer, (void*) index);
	}

	pthread_exit(NULL);
}

