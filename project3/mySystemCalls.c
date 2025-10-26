#include "proj3KernelTemplate.h"
#include "421proj3structs.h"
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

queue_node_421_t* dequeue(priority_queue_421_t* queue) {
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
	}
	// Allocate application
	application = kmalloc(sizeof(proj_app_ctx_t), GFP_KERNEL);
	if (!application) {
		mutex_unlock(&application_init_lock);
		return ENOMEM;
	}

	// Create and allocate High Queue
	application->highQueue = kmalloc(sizeof(priority_queue_421_t), GFP_KERNEL);
	if (!application->highQueue) {
		mutex_unlock(&application_init_lock);
		return EIO;
	}

	application->highQueue->head = NULL;
	application->highQueue->tail = NULL;
	application->highQueue->lock = kmalloc(sizeof(struct mutex), GFP_KERNEL);
	mutex_init(application->highQueue->lock);
	application->highQueue->num_nodes = 0;

	// Create and allocate Medium Queue
	application->mediumQueue = kmalloc(sizeof(priority_queue_421_t), GFP_KERNEL);
	if (!application->mediumQueue) {
		mutex_unlock(&application_init_lock);
		return EIO;
	}

	application->mediumQueue->head = NULL;
        application->mediumQueue->tail = NULL;
	application->mediumQueue->lock = kmalloc(sizeof(struct mutex), GFP_KERNEL);
        mutex_init(application->mediumQueue->lock);
        application->mediumQueue->num_nodes = 0;

	// Create and allocate Low Queue
	application->lowQueue = kmalloc(sizeof(priority_queue_421_t), GFP_KERNEL);
	if (!application->lowQueue) {
		mutex_unlock(&application_init_lock);
		return  EIO;
	}

	application->lowQueue->head = NULL;
        application->lowQueue->tail = NULL;
	application->lowQueue->lock = kmalloc(sizeof(struct mutex), GFP_KERNEL);
        mutex_init(application->lowQueue->lock);
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
		kfree(dequeue(application->highQueue));
	}
	application->highQueue->head == NULL;
	application->highQueue->tail == NULL;
	kfree(application->highQueue->lock);
        application->highQueue->lock = NULL;
	application->highQueue->num_nodes = 0;
	kfree(application->highQueue);
	application->highQueue = NULL;

	// Free mediumQueue
	while (application->mediumQueue->head != NULL) {
		kfree(dequeue(application->mediumQueue));
	}
	application->mediumQueue->head = NULL;
        application->mediumQueue->tail = NULL;
	kfree(application->mediumQueue->lock);
	application->mediumQueue->lock = NULL;
        application->mediumQueue->num_nodes = 0;
	kfree(application->mediumQueue);
	application->mediumQueue = NULL;

	// Free lowQueue
	while (application->lowQueue->head != NULL) {
		kfree(dequeue(application->lowQueue));
	}
	application->lowQueue->head = NULL;
        application->lowQueue->tail = NULL;
	kfree(application->lowQueue->lock);
        application->lowQueue->lock = NULL;
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
	priority_queue_421_t* relevantQueue = NULL;

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

	if (copy_from_user(newNode, node, sizeof(queue_node_421_t))) {
                kfree(newNode);
                mutex_unlock(&application_init_lock);
                return EIO;
        }
	newNode->next = NULL;

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

	// Enqueuing
	if (newNode->priority == HIGH) {
		relevantQueue = application->highQueue;
        } else if (newNode->priority == MEDIUM) {
		relevantQueue = application->mediumQueue;
        } else {
		relevantQueue = application->lowQueue;
	}

	mutex_lock(relevantQueue->lock);
	if (relevantQueue->num_nodes < 1) {
		relevantQueue->head = newNode;
		relevantQueue->tail = newNode;
		relevantQueue->num_nodes++;
		mutex_unlock(relevantQueue->lock);
		mutex_unlock(&application_init_lock);
		return 0;
	} else if (relevantQueue->num_nodes == 1) {
		relevantQueue->head->next = newNode;
		relevantQueue->tail = newNode;
		relevantQueue->num_nodes++;
		mutex_unlock(relevantQueue->lock);
		mutex_unlock(&application_init_lock);
		return 0;
	} else {
		relevantQueue->tail->next = newNode;
		relevantQueue->tail = newNode;
		relevantQueue->num_nodes++;
		mutex_unlock(relevantQueue->lock);
		mutex_unlock(&application_init_lock);
		return 0;
	}
	// If here, some uknown error has occured
	mutex_unlock(&application_init_lock);
	return -1;
}

SYSCALL_DEFINE1(kern_get_priority, void __user*, dest) {
 	mutex_lock(&application_free_lock);
	queue_node_421_t* topOfQueue = NULL;

	if (application == NULL) {
		mutex_unlock(&application_free_lock);
		return EPERM;
	} else if (application->highQueue == NULL && application->mediumQueue == NULL && application->lowQueue == NULL) {
		mutex_unlock(&application_free_lock);
		return ENOENT;
	} else if (application->highQueue->head == NULL && application->mediumQueue->head == NULL && application->lowQueue->head == NULL) {
                mutex_unlock(&application_free_lock);
                return ENOENT;
	}

	// Check where the top/highest priority is then copy value of head node and dequeue
	if (application->highQueue != NULL) {
		mutex_lock(application->highQueue->lock);
		topOfQueue = application->highQueue->head;
		if (copy_to_user(dest, topOfQueue, sizeof(queue_node_421_t))) {
			kfree(topOfQueue);
			mutex_unlock(application->highQueue->lock);
			mutex_unlock(&application_free_lock);
			return EIO;
		}
		dequeue(application->highQueue);
		kfree(topOfQueue);
		mutex_unlock(application->highQueue->lock);
		mutex_unlock(&application_free_lock);
		return 0;
	} else if (application->mediumQueue != NULL) {
		mutex_lock(application->mediumQueue->lock);
                topOfQueue = application->mediumQueue->head;
                if (copy_to_user(dest, topOfQueue, sizeof(queue_node_421_t))) {
                        kfree(topOfQueue);
			mutex_unlock(application->mediumQueue->lock);
			mutex_unlock(&application_free_lock);
                        return EIO;
                }
                dequeue(application->mediumQueue);
                kfree(topOfQueue);
		mutex_unlock(application->mediumQueue->lock);
                mutex_unlock(&application_free_lock);
                return 0;
	} else {
		mutex_lock(application->lowQueue->lock);
                topOfQueue = application->lowQueue->head;
                if (copy_to_user(dest, topOfQueue, sizeof(queue_node_421_t))) {
                        kfree(topOfQueue);
			mutex_unlock(application->lowQueue->lock);
			mutex_unlock(&application_free_lock);
                        return EIO;
                }
                dequeue(application->lowQueue);
                kfree(topOfQueue);
		mutex_unlock(application->lowQueue->lock);
                mutex_unlock(&application_free_lock);
                return 0;
	}
	mutex_unlock(&application_free_lock);
	return -1;
}
