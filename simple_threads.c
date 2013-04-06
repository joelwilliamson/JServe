#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define array_size 1000000

typedef void* (*thread_f)(void *);
struct summer_data {
	long long int *data;
	long long int n;
};


void* thread_sum(struct summer_data* arg) {
	if (arg->n==1) return (void*)arg->data[0];
	pthread_t t1,t2;
	struct summer_data s1 = {arg->data,arg->n/2};
	struct summer_data s2 = {arg->data+arg->n/2,arg->n-arg->n/2};	
	pthread_create(&t1,NULL,(thread_f)thread_sum,&s1);
	pthread_create(&t2,NULL,(thread_f)thread_sum,&s2);
	void *r1, *r2;
	pthread_join(t1,&r1);
	pthread_join(t2,&r2);
	return (void*)((long long int)r1+(long long int)r2);
}

void* thread_start(void *arg) {
	int my_id = pthread_self();
	printf("Thread #%d is starting execution...\n",my_id);
	sleep(1);
	printf("Thread #%d is executing...\n",my_id);
	sleep(1);
	printf("Thread #%d is finishing...\n",my_id);
	return NULL;
}


int main() {
	int i,err;
	pthread_t *thread_buffer;
	long long int sumarray[array_size];
	for (i=0;i<array_size;++i) {
		sumarray[i]=i;
	}
	struct summer_data s = {sumarray,array_size};
	pthread_t t;
//	pthread_create(&t,NULL,(thread_f)thread_sum,&s);
	long long int sum;
//	pthread_join(t,(void**)&sum);
//	printf("The sum of the first %d naturals is %lld.\n",array_size,sum);
 	for (int j=0;j<10000;++j) {
	sum = 0;
	for (i=0;i<array_size;++i) {
		sum+=sumarray[i];
	}}
	printf("Sum: %lld with single threads.\n",sum);
	return 0;
}
