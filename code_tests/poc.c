#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "421proj3structs.h"

#define __NR_init 600
#define __NR_free 601
#define __NR_get_priority 602
#define __NR_add_priority 603

long init_syscall(void){
        return syscall(__NR_init);
}

long free_syscall(void) {
        return syscall(__NR_free);
}

long get_priority_syscall(queue_node_421_t* node) {
        return syscall(__NR_get_priority, node);
}

long add_priority_syscall(queue_node_421_t* node) {
        return syscall(__NR_add_priority, node);
}

void* thread_add_prior_h(void* id_ptr) {
	queue_node_421_t* node = malloc(sizeof(queue_node_421_t));
	int id = *(int *)id_ptr;
	node->id = id;
	printf("Queue Node ID is %d\n", node->id);
	node->priority = 2;
	printf("Queue Node Priority is high\n");
	snprintf(node->data, DATA_LENGTH, "Priority_HIGH_ID_%d", node->id);
	printf("Data is: ");
	printf(node->data);
	printf("Enqueued (0 on success)?: %ld\n", add_priority_syscall(node));
	pthread_exit(NULL);
}
void* thread_add_prior_m(void* id_ptr) {
        queue_node_421_t* node = malloc(sizeof(queue_node_421_t));
        int id = *(int *)id_ptr;
	node->id = id;
        printf("Queue Node ID is %d\n", node->id);
        node->priority = 1;
        printf("Queue Node Priority is medium\n");
	snprintf(node->data, DATA_LENGTH, "Priority_MEDIUM_ID_%d", node->id);
        printf("Data is: ");
        printf(node->data);
        printf("Enqueued (0 on success)?: %ld\n", add_priority_syscall(node));
        pthread_exit(NULL);
}
void* thread_add_prior_l(void* id_ptr) {
        queue_node_421_t* node = malloc(sizeof(queue_node_421_t));
        int id = *(int *)id_ptr;
	node->id = id;
        printf("Queue Node ID is %d\n", node->id);
        node->priority = 0;
        printf("Queue Node Priority is Low\n");
	snprintf(node->data, DATA_LENGTH, "Priority_LOW_ID_%d", node->id);
        printf("Data is: ");
        printf(node->data);
        printf("Enqueued (0 on success)?: %ld\n", add_priority_syscall(node));
        pthread_exit(NULL);
}


int main(){
        // Initialize queue
        printf("Initializing application\n");
        printf("Application initilization: %ld\n\n", init_syscall());

        // Add 15 threads that make nodes w ascending ids
	// high priority (ids 1-5)
	// medium priority (ids 5-10)
	// low priority (ids 10-15)
	for (int i = 0; i < 5; i++) {
		int id = i;
		pthread_t thread;
		pthread_create(&thread, NULL, thread_add_prior_h, &id);
		pthread_join(thread, NULL);
	}
	for (int i = 5; i < 10; i++) {
                int id = i;
                pthread_t thread;
                pthread_create(&thread, NULL, thread_add_prior_m, &id);
                pthread_join(thread, NULL);
        }
	for (int i = 10; i < 15; i++) {
                int id = i;
                pthread_t thread;
                pthread_create(&thread, NULL, thread_add_prior_l, &id);
                pthread_join(thread, NULL);
        }


	// Remove nodes
	printf("Beggining to remove nodes:\n");
	for (int i = 0; i < 15; i++) {
		queue_node_421_t* temp = malloc(sizeof(queue_node_421_t));
		printf("Get Priority (0 on success)?: %ld \n", get_priority_syscall(temp));
		printf("Id should be %d, ", i);
		printf("Id returned is: %d\n", temp->id);
		printf("Data for id is: %s\n", temp->data);
		free(temp);
	}

        // free
        printf("Freeing the queue \n");
        printf("Freed (0 on success)?: %ld\n", free_syscall());
        return 0;
}
