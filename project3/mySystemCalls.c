#include "proj3KernelTemplate.h"

/**
 *  Globally available mutexes (or binary semaphores) for your application to use.
 *  if you're reading this ... what is the best energy drink/type of coffee/drink of choice? -AJ
 *  					**Black coffee fsfs - Christian
 *  Respectively, the each lock serves the following purpose.
 *
 *  application_init_lock:  locks concurrent attempts to initialize (or re-initialize) the application context.
 *                          it is important to note that this mutex may be used for more than the init system call!
 *                          think about what critical data this lock can access ...
 *
 *  application_free_lock:  locks concurrent attempts to free the application context.
 *                          similar to the init lock, this mutex should be used for more than the free system call.
 *
 *  hopefully that gives some insight into what you should be checking.
 *  happy coding!
 *  -AJ
 *   */
static DEFINE_MUTEX(application_init_lock);
static DEFINE_MUTEX(application_free_lock);

proj_app_ctx_t* application = NULL;

SYSCALL_DEFINE0(init_kern_application) {

	// Check if application already exists
	if (application != NULL) return EPERM;

	// Allocate application
	application = kmalloc(sizeof(proj_app_ctx), GFP_KERNEL);
	if (!application) return ENOMEM;

	// Create and allocate High Queue
	application->highQueue = kmalloc(sizeof(priority_queue_421), GFP_KERNEL);
	if (!application->highQueue) return EIO;

	application->highQueue->head = NULL;
	application->highQueue->tail = NULL;
	application->highQueue->num_nodes = 0;

	// Create and allocate Medium Queue
	application->mediumQueue = kmalloc(sizeof(priority_queue_421), GFP_KERNEL);
	if (!application->mediumQueue) return EIO;

	application->mediumQueue->head = NULL;
        application->mediumQueue->tail = NULL;
        application->mediumQueue->num_nodes = 0;

	// Create and allocate Low Queue
	application->lowQueue = kmalloc(sizeof(priority_queue_421), GFP_KERNEL);
	if (!application->lowQueue) return  EIO;

	application->lowQueue->head = NULL;
        application->lowQueue->tail = NULL;
        application->lowQueue->num_nodes = 0;

	// If everything has succeeded, return 0
	return 0;
}

SYSCALL_DEFINE0(free_kern_application) {

	// Check if application is already freed
	if (application == NULL) return EPERM;

	// Free highQueue
	while (application->highQueue->head != NULL) kfree(dequeue());
	application->highQueue->head == NULL;
	application->highQueue->tail == NULL;
	application->highQueue->num_nodes = 0;
	kfree(application->highQueue);
	application->highQueue = NULL;

	// Free mediumQueue
	while (application->mediumQueue->head != NULL) kfree(dequeue());
	application->mediumQueue->head = NULL;
        application->mediumQueue->tail = NULL;
        application->mediumQueue->num_nodes = 0;
	kfree(application->mediumQueue);
	application->mediumQueue = NULL;

	// Free lowQueue
	while (application->lowQueue->head != NULL) kfree(dequeue());
	application->lowQueue->head = NULL;
        application->lowQueue->tail = NULL;
        application->lowQueue->num_nodes = 0;
	kfree(application->lowQueue);
	application->lowQueue = NULL;

	// Free application
	kfree(application);
	application = NULL;

	return 0;
}

SYSCALL_DEFINE1(kern_add_priority, void __user*, node) {
	priority_queue_421* relevantQueue = NULL;

	if (application == NULL) return EPERM;

	qnode_421_t* newNode = kmalloc(sizeof(qnode_421_t), GFP_KERNEL);
	if (!newNode) return ENOMEM;

	if (newNode->priority == HIGH && application->highQueue == NULL) return ENOENT;
	if (newNode->priority == MEDIUM && application->mediumQueue == NULL) return ENOENT;
	if (newNode->priority == LOW && application->lowQueue == NULL) return ENOENT;

	if (copy_from_user(newNode, node, sizeof(qnode_421_t))) {
		kfree(newNode);
		return EIO;
	}
	newNode->next = NULL;
	// Enqueuing
	if (newNode->priority == HIGH) relevantQueue = application->highQueue;
        if (newNode->priority == MEDIUM) relevantQueue = application->mediumQueue;
        if (newNode->priority == LOW) relevantQueue = application->lowQueue;

	if (relevantQueue->num_nodes < 1) {
		relevantQueue->head = newNode;
		relevantQueue->tail = newNode;
		relevantQueue->num_nodes++;
	} else if (relevantQueue->num_nodes == 1) {
		relevantQueue->head->next = newNode;
		relevantQueue->tail = newNode;
		relevantQueue->num_nodes++;
	return 0;
}

SYSCALL_DEFINE1(kern_get_priority, void __user*, dest) {
  return -1;
}
