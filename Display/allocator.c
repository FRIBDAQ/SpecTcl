/*
** Facility:
**   Xamine support routines.
** Abstract:
**   allocator.c
**      This file implements an arena based memory manager subsystem.
**      At present an arena which can be managed is a fixed size region
**      of memory
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "allocator.h"
#include "client.h"
#define remove  al_remove
/*
** data structures used:
*/

typedef struct _node {
                 struct _node *next;
		 long          size;
	       } node;			/* memory node. */

/* 
** Local storage: 
*/

static node empty = { NULL, 0 };


/*
** Functional Description:
**  findprior:
**    This local function tries to locate a node in the list which
**    is physically just prior to a chunk of storage being released.
**    The idea is that the free list will be sorted by address so
**    that buddy compaction is done easily.
** Formal Parameters:
**   arenaid arena:
**     The arena pointer.
**   caddr_t s:
**     memory being released.
** Returns:
**   Prior node or arena if s should be first.
*/
static node *findprior(arenaid arena, caddr_t s)
{
  node *n = (node *)*arena;

  /* if the first node is after us, return the arena pointer: */

  if( (node *)s < n ) return (node *)arena;

  while( n->next != NULL) {
    if((node *)s < n->next ) return n;
    n = n->next;
  }

  /* If here, than we're at the end, so return n (tack on end of list). */

  return n;
}

/*
** Functional Description:
**  remove:
**    Local function to remove a specified node from the free list.
** Formal Parameters:
**   arenaid arena:
**      Points to the arena free list pointer.
**   node *st_node:
**      The node to remove.
*/
static void remove(arenaid arena, node *st_node)
{
  node *n = (node *)*arena;

  /* Check for first node: */

  if( n == st_node ) {
    *arena = (caddr_t)st_node->next;
  }
  else {
    while(n->next != st_node) {	/* Look for prior node... */
      n = n->next;
    }
    n->next = st_node->next;	/* Unlink st_node from the chain. */
  }
}

/*
** Functional Description:
**  alloc_init:
**    Initializes an allocator managed arena of memory.
** Formal Parameters:
**   caddr_t arena:
**     Points to the arena to manage.
**   int size:
**     the number of bytes in the arena.
** returns:
**    caddr_t arena_id:
**      A pointer to an opaque data structure called the arena control block.
**      This will be NULL If the arena control block could not be allocated
**      and filled in.
*/
arenaid alloc_init(caddr_t arena, int size)
{
  node **first;

  /* Allocate the node pointer.   */

  first = (node **)malloc(sizeof(first));
  if(first == NULL) return NULL;

  /* Fill in the node pointer and make all of the arena a single entry
  ** Free list.
  */

  *first = (node *)arena;
  (*first)->next = NULL;
  (*first)->size = size - sizeof(node);


  return (arenaid)first;
}

/*
** Functional Description:
**   alloc_get:
**     Gets a block from the free list of the arena.
**     We search for an exact match.  If that's not found then we
**     allocate from the part of the largest free region.
** Formal parameters:
**   caddr_t arena_id:
**    an arena identifier returned from alloc_init.
**  int size:
**    Number of bytes requested.
** Returns:
**   Pointer to the region allocated or null if there was no memory left.
*/
caddr_t alloc_get(arenaid arena_id, int size)
{
  node *free = (node *)*arena_id;	/* Get the free list pointer. */
  node *largest = &empty;	/* Point to the largest free node. */

  do {				/* Look for exact match and largest blk. */
    if(free->size == size) {	/* Exact match */
      char *block;
      block = (char *)free;
      block += sizeof(node);
      remove(arena_id, free);	/* Remove node from free list. */
      return ((caddr_t)block);
    }
    if(free->size > largest->size)
      largest = free;
    free = free->next;
  } while (free != (node *)NULL);
  
  /* When control falls here, largest points to the largest 
  ** Chunk of free memory in the list.  If it's large enough, then
  ** we allocate the storage from the top part of the block
  ** (makes handling the list easier).
  */
  
  if(largest->size < size)
    return (caddr_t)NULL;		/* Not enough contiguous memory. */
  
  {
    node *st_node;		/* Will point to block storage node. */
    char *block;		/* WIll point to the user storage.   */

    block = (char *)largest;
    block += (largest->size - size); /* Points to node part of user storage */
    st_node = (node *)block;
    block += sizeof(node);	/* Point to the storage itself. */
    st_node->next = NULL;
    st_node->size = size;
    largest->size -= size + sizeof(node);
    return (caddr_t)block;
  }
}

/*
** Functional Description:
**   alloc_free:
**     Free storage that has been allocated from an arena.  To do this
**     we keep the list address ordered and then coalesce adjacent free
**     regions.
** Formal Parameters:
**  arenaid arena_id:
**    The arena identifier, really a pointer to the pointer to the first
**    node in the free list.
**  caddr_t storage:
**    The storage to free.  It is assumed to be preceded by a node
**    structure with an accurate byte count.
*/
void alloc_free(arenaid arena_id, caddr_t storage)
{
  node *prior;
  node *st_node;

  st_node = (node *)(((char *)storage) - sizeof(node));

  prior  = findprior(arena_id, storage); /* Locate prior node. */

  /*  Handle the special case where this should be the first node */

  if(prior == (node *)arena_id) {
    node *nxt = (node *)*arena_id;
    st_node->next = nxt;     /* First thread this node on the list */
    *arena_id = (caddr_t)st_node;
    if( (node *)(storage + st_node->size) == nxt) { /* Coalesce if possible */
      st_node->size += nxt->size + sizeof(node);
      st_node->next  = nxt->next;
    }
    return;
  }

  /* At this point, prior points to the node prior to our insertion point */
  /* prior->next points to the one after our insertion point.            */

  /* Attempt to coalesce with prior... if coalescable, then st_node will */
  /* be adjusted to point to prior, if not then st_node will be threaded on */
  /* after the prior node.  Either way, we'll be set up to coalesce st_node */
  /* with prior->next if possible.                                       */
  {
    char *p = (char *)prior;
    p += sizeof(node);
    if ((node *)p == st_node) {
      prior->size += st_node->size + sizeof(node); /* Coalesce and */
      st_node = prior;		/* Fake out the code to come. */
    }
    else {
      st_node->next = prior->next; /* Thread released node onto list. */
      prior->next   = st_node;
    }
    
    /* Now we attempt to coalesce st_node and st_node->next */
    
    p = (char *)st_node;
    p += sizeof(node);
    if(p == (char *)st_node->next) {	/* If st_node is last then this is never true */
      st_node->size += st_node->next->size + sizeof(node);
      st_node->next  = st_node->next->next;
    }
  }
}

/*
** Functional Description:
**   alloc_done:
**     Finish using a memory allocation arena... all this does is free
**     the storage associated with the arena.
** Formal Parameters:
**        caddr_t arena_id:
**           identifier of the memory arena.
*/
void alloc_done(arenaid arena_id)
{
  free(arena_id);
}
