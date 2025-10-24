#include "proj3KernelTemplate.h"
#include <linux/kernel.h>
#include <linux/syscalls.h>
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

qnode_421_t* dequeue(priority_queue_421_t* queue) {
	queue_node_421_t* temp = queue->head;
	if (queue->num_nodes == 1) {
		queue->head = NULL;
		queue->tail = NULL;
		queue->num_nodes--;
		return temp;
	} else {
		queue->head = queue->head->next;
			if (queue->num_nodes == 2) {
				queue->tail = queue->head;
			}
		queue->num_nodes--;
		return temp;
	}
	return NULL;
}

SYSCALL_DEFINE0(init_kern_application) {
	mutex_lock(&application_init_lock);

	// Check if application already exists
	if (application != NULL) {
		mutex_unlock(&application_init_lock);
		return EPERM;

	// Allocate application
	application = kmalloc(sizeof(proj_app_ctx), GFP_KERNEL);
	if (!application) {
		mutex_unlock(&application_init_lock);
		return ENOMEM;
	}

	// Create and allocate High Queue
	application->highQueue = kmalloc(sizeof(priority_queue_421), GFP_KERNEL);
	if (!application->highQueue) {
		mutex_unlock(&application_init_lock);
		return EIO;
	}

	application->highQueue->head = NULL;
	application->highQueue->tail = NULL;
	application->highQueue->num_nodes = 0;

	// Create and allocate Medium Queue
	application->mediumQueue = kmalloc(sizeof(priority_queue_421), GFP_KERNEL);
	if (!application->mediumQueue) {
		mutex_unlock(&application_init_lock);
		return EIO;
	}

	application->mediumQueue->head = NULL;
        application->mediumQueue->tail = NULL;
        application->mediumQueue->num_nodes = 0;

	// Create and allocate Low Queue
	application->lowQueue = kmalloc(sizeof(priority_queue_421), GFP_KERNEL);
	if (!application->lowQueue) {
		mutex_unlock(&application_init_lock);
		return  EIO;
	}

	application->lowQueue->head = NULL;
        application->lowQueue->tail = NULL;
        application->lowQueue->num_nodes = 0;

	// If everything has succeeded, return 0
	mutex_unlock(&application_init_lock);
	return 0;
}

SYSCALL_DEFINE0(free_kern_application) {
	mutex_lock(&application_free_lock);

	// Check if application is already freed
	if (application == NULL) {
		mutex_unlock(&application_free_lock);
		return EPERM;
	}

	// Free highQueue
	while (application->highQueue->head != NULL) {
		kfree(dequeue(highQueue));
	}
	application->highQueue->head == NULL;
	application->highQueue->tail == NULL;
	application->highQueue->num_nodes = 0;
	kfree(application->highQueue);
	application->highQueue = NULL;

	// Free mediumQueue
	while (application->mediumQueue->head != NULL) {
		kfree(dequeue(mediumQueue));
	}
	application->mediumQueue->head = NULL;
        application->mediumQueue->tail = NULL;
        application->mediumQueue->num_nodes = 0;
	kfree(application->mediumQueue);
	application->mediumQueue = NULL;

	// Free lowQueue
	while (application->lowQueue->head != NULL) {
		kfree(dequeue(lowQueue));
	}
	application->lowQueue->head = NULL;
        application->lowQueue->tail = NULL;
        application->lowQueue->num_nodes = 0;
	kfree(application->lowQueue);
	application->lowQueue = NULL;

	// Free application
	kfree(application);
	application = NULL;
	mutex_unlock(&application_free_lock);
	return 0;
}

SYSCALL_DEFINE1(kern_add_priority, void __user*, node) {
	mutex_lock(&application_init_lock);
	priority_queue_421* relevantQueue = NULL;

	if (application == NULL) {
		mutex_unlock(&application_init_lock);
		return EPERM;
	}

	queue_node_421_t* newNode = kmalloc(sizeof(queue_node_421_t), GFP_KERNEL);
	if (!newNode) {
		kfree(newNode);
		mutex_unlock(&application_init_lock);
		return ENOMEM;
	}

	if (newNode->priority == HIGH && application->highQueue == NULL) {
		kfree(newNode);
		mutex_unlock(&application_init_lock);
		return ENOENT;
	}
	if (newNode->priority == MEDIUM && application->mediumQueue == NULL) {
		kfree(newNode);
		mutex_unlock(&application_init_lock);
		return ENOENT;
	}
	if (newNode->priority == LOW && application->lowQueue == NULL) {
		kfree(newNode);
		mutex_unlock(&application_init_lock);
		return ENOENT;
	}

	if (copy_from_user(newNode, node, sizeof(queue_node_421_t))) {
		kfree(newNode);
		mutex_unlock(&application_init_lock);
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
		mutex_unlock(&application_init_lock);
		return 0;
	} else if (relevantQueue->num_nodes == 1) {
		relevantQueue->head->next = newNode;
		relevantQueue->tail = newNode;
		relevantQueue->num_nodes++;
		mutex_unlock(&application_init_lock);
		return 0;
	} else {
		relevantQueue->tail->next = node;
		relevantQueue->tail = node;
		relevantQueue->num_nodes++;
		mutex_unlock(&application_init_lock);
		return 0;

	// If here, some uknown error has occured
	mutex_unlock(&application_init_lock);
	return -1;
}

SYSCALL_DEFINE1(kern_get_priority, void __user*, dest) {
 	mutex_lock(&application_free_lock);
	if (application == NULL) {
		mutex_unlock(&application_free_lock);
		return EPERM;
	} else if (application->highQueue == NULL && application->mediumQueue == NULL && application->lowQueue == NULL) {
		mutex_unlock(&application_free_lock);
		return ENOENT;
	}

	// Check where the top/highest priority is then copy value of head node and dequeue
	qnode_421_t* topOfQueue = NULL;
	if (application->highQueue != NULL) {
		topOfQueue = application->highQueue->head;
		if (copy_to_user(dest, topOfQueue, sizeof(queue_node_421_t))) {
			kfree(topOfQueue);
			mutex_unlock(&application_free_lock);
			return EIO;
		}
		dequeue(highQueue);
		kfree(topOfQueue);
		mutex_unlock(&application_free_lock);
		return 0;
	} else if (application->mediumQueue != NULL) {
                topOfQueue = application->mediumQueue->head;
                if (copy_to_user(dest, topOfQueue, sizeof(queue_node_421_t))) {
                        kfree(topOfQueue);
			mutex_unlock(&application_free_lock);
                        return EIO;
                }
                dequeue(mediumQueue);
                kfree(topOfQueue);
                mutex_unlock(&application_free_lock);
                return 0;
	} else {
                topOfQueue = application->lowQueue->head;
                if (copy_to_user(dest, topOfQueue, sizeof(queue_node_421_t))) {
                        kfree(topOfQueue);
			mutex_unlock(&application_free_lock);
                        return EIO;
                }
                dequeue(lowQueue);
                kfree(topOfQueue);
                mutex_unlock(&application_free_lock);
                return 0;
	mutex_unlock(&application_free_lock);
	return -1;
}
