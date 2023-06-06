#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 5

int shared_var; //shared variable
int n_writers = 0; //number writers
int n_readers = 0; //number readers

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;  	/* mutex lock for buffer */
pthread_cond_t c_writer = PTHREAD_COND_INITIALIZER; /* writer waits on this cond var */
pthread_cond_t c_reader = PTHREAD_COND_INITIALIZER; /* reader waits on this cond var */

void *writer(void *param);
void *reader(void *param);

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    int i;
    pthread_t t_write_id[NUM_THREADS], t_read_id[NUM_THREADS];  /* thread identifiers */

    for (i = 0; i < NUM_THREADS; i++) { /* create/fork threads */
		pthread_create(&t_write_id[i], NULL, writer, NULL);
        pthread_create(&t_read_id[i], NULL, reader, NULL);
	}

    for (i = 0; i < NUM_THREADS; i++) { /* wait/join threads */
		pthread_join(t_write_id[i], NULL);
        pthread_join(t_read_id[i], NULL);
	}

    return 0;
}

/* Writer value(s) */
void *writer(void *param) {
    int i;
    for(i=0;i<50;i++)
    {
        pthread_mutex_lock(&m);
            while (n_readers > 0)
            {
                pthread_cond_wait(&c_writer,&m);
            }
            shared_var = rand();
            n_writers++;
        pthread_mutex_unlock(&m);
        pthread_cond_broadcast(&c_reader);
        printf("Shared variable value is: %d\n",shared_var);
        fflush(stdout);
        sleep((rand()%10)+1);
    }

}

/* Reader value(s) */
void *reader(void *param) {
    int i,read;
    for(i=0;i<50;i++){
        pthread_mutex_lock(&m);
            if(n_writers < 0){
                exit(1);
            }//underflow
            while(n_writers == 0){
                pthread_cond_wait(&c_reader,&m);
            }
            read = shared_var;
            n_writers--;
        pthread_mutex_unlock(&m);
        pthread_cond_signal(&c_writer);
        printf("Shared variable value read is: %d\n",read);
        sleep((rand()%10)+1);

    }

}