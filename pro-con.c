#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_FILES 3
#define BUFFER_SIZE 6
#define CLAUSE_SIZE 50
#define CONS_LOOP   4

//procedures and functions
void producer(int *id);
void consumer(int *id);

//global variables
FILE *fp;
int in, out, INPUT_FILE;
char buffer[BUFFER_SIZE][CLAUSE_SIZE];
char input[TOTAL_FILES][CLAUSE_SIZE] = {"input/1.pl", "input/2.pl", "input/3.pl"};

sem_t empty;
sem_t full;
sem_t cons_mutex;
sem_t prod_mutex;

int main(int argc, char *argv[]) {
    
    //Initialize
    pthread_t p1, p2, p3, c1, c2, c3;
    int n_p1 = 1, n_p2 = 2, n_p3 = 3, n_c1 = 4, n_c2 = 5, n_c3 = 6;
    in = out = INPUT_FILE = 0;
    fp = fopen("output","a+");  
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	sem_init(&cons_mutex, 0, 1);
	sem_init(&prod_mutex, 0, 1);

	pthread_create(&p1, NULL, (void*) producer, &n_p1);
	pthread_create(&p2, NULL, (void*) producer, &n_p2);
	pthread_create(&p3, NULL, (void*) producer, &n_p3);
	pthread_create(&c1, NULL, (void*) consumer, &n_c1);
	pthread_create(&c2, NULL, (void*) consumer, &n_c2);
	pthread_create(&c3, NULL, (void*) consumer, &n_c3);

	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(p3, NULL);
	pthread_join(c1, NULL);
	pthread_join(c2, NULL);
	pthread_join(c3, NULL);
    
    sem_close(&empty);
    sem_close(&full);
    sem_close(&cons_mutex);
    sem_close(&prod_mutex);
    
    fclose(fp);    
	return 1;
}

void producer(int *id) {
	int i = 0;
	
	char data[CLAUSE_SIZE];
	sem_wait(&prod_mutex);
	printf("input :: %s\n", input[INPUT_FILE]);
	char *file_name = input[INPUT_FILE++];
	sem_post(&prod_mutex);
	
	FILE *ifile = fopen(file_name, "r");	
	while(1) {
	    if(feof(ifile)) break;	
		fprintf(fp, "Producer (Thr#%d) :: Interation#%d >> %s\n", *id, i++, file_name);    
		sem_wait(&empty);
		sem_wait(&prod_mutex);
		if(fscanf(ifile, "%s", buffer[in]) != EOF) {
		    strcat(buffer[in], "\n");
		    printf("I ++ %s", buffer[in]);
		    in = (in + 1) % BUFFER_SIZE;
	    }
	    sem_post(&prod_mutex);
        sem_post(&full);
	}
}

void consumer(int *id) {
    for (int i = 0; i < CONS_LOOP; i++) {
	    sem_wait(&full);
	    sem_wait(&cons_mutex);
	    char *clause = buffer[out];
	    out = (out + 1) % BUFFER_SIZE;
	    sem_post(&cons_mutex);
	    sem_post(&empty);
        printf("O -- %s", clause);
	    fprintf(fp, "Consumer (Thr#%d) :: Interation#%d >> %s", *id, i, clause);
    }
}
