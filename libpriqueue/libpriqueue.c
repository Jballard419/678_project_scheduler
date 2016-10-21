/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->size=0;
	q->comparer=comparer;
	q->frontnode = NULL;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{

	struct node_t *newnode = (struct node_t*)malloc(sizeof(struct node_t));
	newnode->lower_node = NULL;
	newnode->value=ptr;
	int i= 0;
	struct node_t *tempnode = q->frontnode;
	struct node_t *oldnode = NULL;
	if(q->frontnode==NULL ){
		q->size=1;
		q->frontnode=newnode;
		return 0;
	}
	while (q-> comparer(ptr, tempnode->value)> 0) {
		oldnode = tempnode;
		tempnode = oldnode->lower_node;
		i++;
		if (i == q->size)
			break;
	}
	if (oldnode==NULL) {
		q-> frontnode = newnode;
	} else {
		oldnode ->lower_node = newnode;
	}
	newnode->lower_node = tempnode;
		q->size++;
	return i;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(q->size==0)
		return NULL;
	return q->frontnode->value;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if(q->size==0)
		return NULL;
	struct node_t *tempnode = q->frontnode;
	q->frontnode = tempnode -> lower_node;
	q->size--;
	void *v = tempnode -> value;
	free(tempnode); //will this Destroy v as well
	return v;
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	if(q->size<=index)
		return NULL;
	struct node_t *tempnode = q->frontnode;
	struct node_t *oldnode=NULL;
	while (index>0 && tempnode != NULL) {
		index--;
		oldnode = tempnode;
		tempnode = oldnode->lower_node;

	}
	return tempnode-> value;
}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	struct node_t *tempnode = q->frontnode;
	struct node_t *oldnode=NULL;
	int removed=0;
	while (tempnode!=NULL) {
		if(q->comparer(tempnode->value, ptr)==0)
		{
			q->frontnode=tempnode->lower_node;
//			free(tempnode->value);
			free(tempnode);
			removed ++;
			tempnode=q->frontnode;



		}else
		{
			oldnode = q->frontnode;
			tempnode = oldnode->lower_node;
			break;
		}
	}
	while (tempnode != NULL) {
		if(q->comparer(tempnode->value, ptr)==0)
		{
			oldnode->lower_node=tempnode->lower_node;
//			free(tempnode->value);
			free(tempnode);
			removed ++;
		tempnode = oldnode->lower_node;

		}else
		{
			oldnode = tempnode;
			tempnode = oldnode->lower_node;
		}
	}
	q->size = q->size - removed;
	return removed;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	if(q->size<=index)
		return NULL;
	struct node_t *tempnode = q->frontnode;
	struct node_t *oldnode=NULL;
	while (index>0 && tempnode != NULL) {
		index--;
		oldnode = tempnode;
		tempnode = oldnode->lower_node;

	}
	void *temp_value= tempnode-> value;
	if(oldnode!=NULL)
	{
		oldnode ->lower_node = tempnode -> lower_node;
	}else
	{
		q->frontnode=tempnode -> lower_node;
	}
		free(tempnode);

	return temp_value;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	struct node_t *temp = q->frontnode;
  struct node_t *oldnode;
  while (temp!=NULL) {

			oldnode=temp;
      temp=oldnode->lower_node;
			free(oldnode);

    }

	//free(q->comparer) :maybe ?

	return;
}
