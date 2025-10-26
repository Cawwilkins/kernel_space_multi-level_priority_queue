#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "421proj3structs.h"
#include <time.h>

#define __NR_init 600
#define __NR_free 601
#define __NR_add_priority 602
#define __NR_get_priority 603

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

// pthread helpers
void* thread_enqueue(void* id_ptr) {
        queue_node_421_t* node = malloc(sizeof(queue_node_421_t));
        int id = *(int *)id_ptr;
        node->id = id;
        node->priority = id % 3;
        snprintf(node->data, DATA_LENGTH, "Priority_is_%d_ID_%d", node->priority, node->id);
        add_priority_syscall(node);
        pthread_exit(NULL);
}

void* thread_dequeue(void* id_ptr) {
        queue_node_421_t* temp = malloc(sizeof(queue_node_421_t));
        get_priority_syscall(temp);
        free(temp);
        pthread_exit(NULL);
}

void* thread_init(void* none) {
	init_syscall();
	pthread_exit(NULL);
}

void* thread_free(void* none) {
	free_syscall();
	pthread_exit(NULL);
}

// Test functions
// Kern Init (Normal)
void test_kern_init_norm(void) {
        int enqueueWorked = 1;
        int correctReturn = init_syscall();

        // Create and add a node
        queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
        nodeOne->id = 1;
	nodeOne->priority = HIGH;
	snprintf(nodeOne->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeOne->id);
        enqueueWorked = add_priority_syscall(nodeOne);

        // If enqueue happened and init worked, pass
        if (enqueueWorked == 0 && correctReturn == 0){
                printf("[PASS]: test_kern_init_norm\n");
                free_syscall();
                free(nodeOne);
                return;
        } else {
                printf("[FAIL]: test_kern_init_norm\n");
                free_syscall();
                free(nodeOne);
                return;
        }
        return;
}


// Kern init (Edge)
void test_kern_init_edge(void) {
        int initWorked = 1;

        queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
        nodeOne->id = 1;
	nodeOne->priority = HIGH;
        snprintf(nodeOne->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeOne->id);
        add_priority_syscall(nodeOne);

        queue_node_421_t* nodeTwo = malloc(sizeof(queue_node_421_t));
        nodeTwo->id = 2;
	nodeTwo->priority = HIGH;
        snprintf(nodeTwo->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeTwo->id);
        add_priority_syscall(nodeTwo);

        queue_node_421_t* nodeThree = malloc(sizeof(queue_node_421_t));
        nodeThree->id = 3;
	nodeThree->priority = HIGH;
        snprintf(nodeThree->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeThree->id);
        add_priority_syscall(nodeThree);

        queue_node_421_t* nodeFour = malloc(sizeof(queue_node_421_t));
        nodeFour->id = 4;
	nodeFour->priority = HIGH;
        snprintf(nodeFour->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeFour->id);
        add_priority_syscall(nodeFour);

        queue_node_421_t* nodeFive = malloc(sizeof(queue_node_421_t));
        nodeFive->id = 5;
	nodeFive->priority = HIGH;
        snprintf(nodeFive->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeFive->id);
        add_priority_syscall(nodeFive);

        queue_node_421_t* nodeSix = malloc(sizeof(queue_node_421_t));
        nodeSix->id = 6;
	nodeSix->priority = HIGH;
        snprintf(nodeSix->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeSix->id);
        add_priority_syscall(nodeSix);

        queue_node_421_t* nodeSeven = malloc(sizeof(queue_node_421_t));
        nodeSeven->id = 7;
	nodeSix->priority = HIGH;
        snprintf(nodeSeven->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeSeven->id);
        add_priority_syscall(nodeSeven);

        queue_node_421_t* nodeEight = malloc(sizeof(queue_node_421_t));
        nodeEight->id = 8;
	nodeEight->priority = HIGH;
        snprintf(nodeEight->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeEight->id);
        add_priority_syscall(nodeEight);

        queue_node_421_t* nodeNine = malloc(sizeof(queue_node_421_t));
        nodeNine->id = 9;
	nodeNine->priority = HIGH;
        snprintf(nodeNine->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeNine->id);
        add_priority_syscall(nodeNine);

        queue_node_421_t* nodeTen = malloc(sizeof(queue_node_421_t));
        nodeTen->id = 10;
	nodeTen->priority = HIGH;
        snprintf(nodeTen->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeTen->id);
        add_priority_syscall(nodeTen);

        // If free then re-initialize
        free_syscall();
        initWorked = init_syscall();

        // If free worked and nothing to dequeue, pass
        if (initWorked == 0){
                printf("[PASS]: test_kern_init_edge\n");
                free(nodeOne);
                free(nodeTwo);
                free(nodeThree);
                free(nodeFour);
                free(nodeFive);
                free(nodeSix);
                free(nodeSeven);
                free(nodeEight);
                free(nodeNine);
                free(nodeTen);
                free_syscall();
                return;
        } else {
                printf("[FAIL]: test_kern_init_edge\n");
                free(nodeOne);
                free(nodeTwo);
                free(nodeThree);
                free(nodeFour);
                free(nodeFive);
                free(nodeSix);
                free(nodeSeven);
               	free(nodeSix);
                free(nodeSeven);
                free(nodeEight);
                free(nodeNine);
                free(nodeTen);
                free_syscall();
                return;
        }
        return;
}


// Kern Init - EPERM
void test_kern_init_eperm(void) {
        // Attempting to do multiple init syscalls
        int firstReturn = init_syscall();
        int errorReturn = init_syscall();

        // If first worked and second failed, pass
        if (firstReturn == 0 && errorReturn == 1){
                printf("[PASS]: test_kern_init_eperm\n");
                free_syscall();
                return;
        } else {
                printf("[FAIL]: test_kern_init_eperm\n");
                free_syscall();
                return;
        }
        return;
}


// Kern Free (Normal)
void test_kern_free_norm(void) {
        int correctReturn = init_syscall();
        int freeWorked = free_syscall();

        // If both calls had properly gone through then queue was initialized and freed correctly
        if (freeWorked == 0 && correctReturn == 0){
                printf("[PASS]: test_kern_free_norm\n");
                return;
        } else {
                printf("[FAIL]: test_kern_free_norm\n");
                return;
        }
        return;
}


// Kern Free (Edge)
void test_kern_free_edge(void) {
        int dequeueFailed = 0;

        // Enqueue 100 nodes
        for (int i = 0; i < 100; i++){
                queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
                nodeOne->id = i;
		nodeOne->priority = i % 3;
        	snprintf(nodeOne->data, DATA_LENGTH, "ID is %d", nodeOne->id);
		add_priority_syscall(nodeOne);
        }

        // If free is successful, will get an error when trying to dequeue
        free_syscall();
        queue_node_421_t* nodeTop = malloc(sizeof(queue_node_421_t));
	dequeueFailed = get_priority_syscall(nodeTop);

        // If free worked and nothing to dequeue, pass
        if (dequeueFailed == 1){
                printf("[PASS]: test_kern_free_edge\n");
                free_syscall();
                free(nodeTop);
                return;
        } else {
                printf("[FAIL]: test_kern_free_edge\n");
                free_syscall();
                free(nodeTop);
                return;
        }
        return;
}


// Kern Free EPERM
void test_kern_free_eperm(void) {
        int freeFailed = 0;

        // Will attemped to free when we have not created a queue yet
        freeFailed = free_syscall();

        // If free failed, then test passes
        if (freeFailed == 1){
                printf("[PASS]: test_kern_free_eperm\n");
                return;
        } else {
                printf("[FAIL]: test_kern_free_eperm\n");
                return;
        }
        return;
}


// Kern Get (Normal)
void test_kern_get_norm(void) {
        // Attempting to enqueue several nodes and dequeue one
        int dequeueWorked = 1;
        int enqueueWorked = 1;
        int dequeueValue = 0;
        int correctReturn = init_syscall();

        // Enqueue three nodes
        queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
        nodeOne->id = 1;
	nodeOne->priority = HIGH;
        snprintf(nodeOne->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeOne->id);
        enqueueWorked = add_priority_syscall(nodeOne);

        queue_node_421_t* nodeTwo = malloc(sizeof(queue_node_421_t));
        nodeTwo->id = 2;
	nodeTwo->priority = MEDIUM;
        snprintf(nodeTwo->data, DATA_LENGTH, "Priority_MEDIUM_ID_%d", nodeTwo->id);
        add_priority_syscall(nodeTwo);

        queue_node_421_t* nodeThree = malloc(sizeof(queue_node_421_t));
        nodeThree->id = 3;
	nodeThree->priority = LOW;
        snprintf(nodeThree->data, DATA_LENGTH, "Priority_LOW_ID_%d", nodeThree->id);
        add_priority_syscall(nodeThree);

        // Dequeue one
        queue_node_421_t* nodeFour = malloc(sizeof(queue_node_421_t));
        dequeueWorked = get_priority_syscall(nodeFour);
        dequeueValue = nodeFour->id;

        // If init, enqueued, and dequeued then passes
        if (enqueueWorked == 0 && correctReturn == 0 && dequeueWorked == 0 && dequeueValue == 1){
                printf("[PASS]: test_kern_get_norm\n");
                free(nodeOne);
		free(nodeTwo);
		free(nodeThree);
		free(nodeFour);
                free_syscall();
                return;
        } else {
                printf("[FAIL]: test_kern_get_norm\n");
                free(nodeOne);
		free(nodeTwo);
		free(nodeThree);
		free(nodeFour);
                free_syscall();
                return;
        }
        return;
}


// Kern Get (Edge)
void test_kern_get_edge(void) {
        // Attempting to enqueue and dequeue a node, making the queue empty
        int dequeueWorked = 1;
        int enqueueWorked = 1;
        int dequeueValue = 0;
        int correctReturn = init_syscall();

        // Enqueue a node
        queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
        nodeOne->id = 1;
	nodeOne->priority = HIGH;
        snprintf(nodeOne->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeOne->id);
        enqueueWorked = add_priority_syscall(nodeOne);

        // Dequeue a node
        queue_node_421_t* nodeTwo = malloc(sizeof(queue_node_421_t));
        dequeueWorked = get_priority_syscall(nodeTwo);
        dequeueValue = nodeTwo->id;

        // If init, enqueued, and dequeued then passes
        if (enqueueWorked == 0 && correctReturn == 0 && dequeueWorked == 0 && dequeueValue == 1){
                printf("[PASS]: test_kern_get_edge\n");
                free(nodeOne);
		free(nodeTwo);
                free_syscall();
                return;
        } else {
                printf("[FAIL]: test_kern_get_edge\n");
                free(nodeOne);
		free(nodeTwo);
                free_syscall();
                return;
        }
        return;
}


// Kern Get - EPERM
void test_kern_get_eperm(void) {
        int dequeueFailed = 0;

        // Enqueue 100 nodes
        for (int i = 0; i < 100; i++){
                queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
                nodeOne->id = i;
                nodeOne->priority = i % 3;
                snprintf(nodeOne->data, DATA_LENGTH, "ID is %d", nodeOne->id);
                add_priority_syscall(nodeOne);
        }

        // If free is successful, will get an error when trying to dequeue
        free_syscall();
        queue_node_421_t* nodeTop = malloc(sizeof(queue_node_421_t));
        dequeueFailed = get_priority_syscall(nodeTop);

        // If free worked and nothing to dequeue, pass
        if (dequeueFailed == 1){
                printf("[PASS]: test_kern_get_eperm\n");
                free_syscall();
                free(nodeTop);
                return;
        } else {
                printf("[FAIL]: test_kern_get_eperm\n");
                free_syscall();
                free(nodeTop);
                return;
        }
        return;
}


// Kern Get - ENOENT
void test_kern_get_enoent(void) {
        int returnVal = 0;

	// Attempting to dequeue an empty queue
        init_syscall();

        queue_node_421_t* node = malloc(sizeof(queue_node_421_t));
        returnVal = syscall(603, node);

        // If queue was allocated and dequeue correctly returned ENOENT, test passes
        if (returnVal == 2){
                printf("[PASS]: test_kern_get_enoent\n");
                free(node);
                free_syscall();
                return;
        } else {
                printf("[FAIL]: test_kern_get_enoent\n");
                free(node);
                free_syscall();
                return;
        }
        return;
}


// Kern Get - EIO
void test_kern_get_eio(void) {
        int dequeueValue = 0;
	int dequeueError = 0;

        // Attempting to dequeue using a null pointer to get EIO
        init_syscall();

        // Enqueue a node and use a null ptr to try to copy to
        queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
        nodeOne->id = 10;
	nodeOne->priority = HIGH;
        snprintf(nodeOne->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeOne->id);
        add_priority_syscall(nodeOne);

	queue_node_421_t* badNode = NULL;
        dequeueError = syscall(602, badNode);

        // Ensure that head still there
        queue_node_421_t* topNode = malloc(sizeof(queue_node_421_t));
        get_priority_syscall(topNode);
        dequeueValue = topNode->id;

        // If queue was allocated but data could not be copied and the head node was still there, pass
        if (dequeueError == 5 && dequeueValue == 10){
                printf("[PASS]: test_kern_get_eio\n");
                free(topNode);
                free_syscall();
                return;
        } else {
                printf("[FAIL]: test_kern_get_eio\n");
                free(topNode);
                free_syscall();
                return;
        }
        return;
}


// Kern Add (Normal)
void test_kern_add_norm(void) {
        int enqueueWorked = 1;
        int correctReturn = init_syscall();

        // Enqueue
        queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
        nodeOne->id = 1;
	nodeOne->priority = HIGH;
        snprintf(nodeOne->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeOne->id);
        enqueueWorked = add_priority_syscall(nodeOne);

        // If both have successful calls then the enqueue had worked
        if (enqueueWorked == 0 && correctReturn == 0){
                printf("[PASS]: test_kern_add_norm\n");
                free(nodeOne);
                free_syscall();
                return;
        } else {
                printf("[FAIL]: test_kern_add_norm\n");
                free(nodeOne);
                free_syscall();
                return;
        }
        return;
}


// Kern Add (Edge)
void test_kern_add_edge(void) {
        int enqueueWorked = 1;
        int correctReturn = init_syscall();

        // Enqueue 100 nodes
        for (int i = 0; i < 100; i++){
                queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
                nodeOne->id = i;
		nodeOne->priority = i % 3;
	        snprintf(nodeOne->data, DATA_LENGTH, "ID is %d", nodeOne->id);
                add_priority_syscall(nodeOne);
        }

        queue_node_421_t* nodeTop = malloc(sizeof(queue_node_421_t));
        get_priority_syscall(nodeTop);
	// 0 is in low, 1 in medium, 2 in high bc of %3
        // so, enqueueWorked should = 2
	enqueueWorked = nodeTop->id;

        // If enqueues worked and top node is still correct, test passes
        if (enqueueWorked == 2 && correctReturn == 0){
                printf("[PASS]: test_kern_add_edge\n");
                free_syscall();
                free(nodeTop);
                return;
        } else {
                printf("[FAIL]: test_kern_add_edge\n");
                free_syscall();
                free(nodeTop);
                return;
        }
        return;
}


// Kern Add - EIO
void test_kern_add_eio(void) {
        int getReturn = 0;
	init_syscall();

	// Attempting to enqueue to a null queue
        queue_node_421_t* impossPointer = NULL;
        getReturn = syscall(602, impossPointer);

        // If Enqueue fails and error is EIO, passes
        if (getReturn == 5){
                printf("[PASS]: test_kern_add_eio\n");
                free(impossPointer);
		free_syscall();
                return;
        } else {
                printf("[FAIL]: test_kern_add_eio\n");
                free(impossPointer);
		free_syscall();
                return;
        }
        return;
}


// Kern Add - EPERM
void test_kern_add_eperm(void) {
        int enqueueFailed = 0;

        // Attempting to enqueue to a null queue
        queue_node_421_t* nodeOne = malloc(sizeof(queue_node_421_t));
        nodeOne->id = 1;
	nodeOne->priority = HIGH;
        snprintf(nodeOne->data, DATA_LENGTH, "Priority_HIGH_ID_%d", nodeOne->id);
        enqueueFailed = add_priority_syscall(nodeOne);

        // If the enqueue returned eperm then pass
        if (enqueueFailed == 1){
                printf("[PASS]: test_kern_add_eperm\n");
                free(nodeOne);
                return;
        } else {
                printf("[FAIL]: test_kern_add_eperm\n");
                free(nodeOne);
                return;
        }
        return;
}

// Test Threading
void test_threading() {
	int count = 0;
	init_syscall();
	// Enqueue 100 nodes
        for (int i = 0; i < 100; i++){
		int id = i;
		pthread_t thread;
		pthread_create(&thread, NULL, thread_enqueue, &id);
		pthread_join(thread, NULL);
		count += 1;
        }

	// Dequeue 100 nodes
	for (int i = 0; i < 100; i++){
                pthread_t thread;
                pthread_create(&thread, NULL, thread_dequeue, NULL);
                pthread_join(thread, NULL);
		count -= 1;
        }

	free_syscall();
	init_syscall();

	// Randomize
	srand(time(NULL));

	// 30 threads to randomly enqueue/dequeue
	for (int i = 0; i < 30; i++){
                int num = rand() % 2;
		if (num == 1){
			int id = i;
                	pthread_t thread;
                	pthread_create(&thread, NULL, thread_enqueue, &id);
                	pthread_join(thread, NULL);
		} else {
                	pthread_t thread;
                	pthread_create(&thread, NULL, thread_dequeue, NULL);
                	pthread_join(thread, NULL);
		}
        }
	// 20 Threads to randomly free or init
	for (int i = 0; i < 20; i++){
                int num = rand() % 2;
                if (num == 1){
			pthread_t thread;
			pthread_create(&thread, NULL, thread_init, NULL);
			pthread_join(thread, NULL);
                } else {
                        pthread_t thread;
			pthread_create(&thread, NULL, thread_free, NULL);
			pthread_join(thread, NULL);
                }
        }
	free_syscall();
	// if still here, no deadlock, and if count == 0, test passes
	if (count == 0) {
		printf("[PASS]: test_threading\n");
	} else {
		printf("[FAIL]: test_threading\n");
	}
}


int main(){
        // Init tests
        test_kern_init_norm();
        test_kern_init_edge();
	test_kern_init_eperm();

        // Free Tests
        test_kern_free_norm();
        test_kern_free_edge();
        test_kern_free_eperm();

        // Dequeue Tests
        test_kern_get_norm();
        test_kern_get_edge();
        test_kern_get_eperm();
        test_kern_get_enoent();
	test_kern_get_eio();

        // Enqueue Tests
        test_kern_add_norm();
        test_kern_add_edge();
	test_kern_add_eperm();
        test_kern_add_eio();

	// Thread Test
	test_threading();
        return 0;
}
