#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memblock.h"
memblock::memblock(void)
{
}


memblock::~memblock(void)
{
}





//////////////////////////////////////////////////////////
mem_block *memblock::mem_block_new(unsign_i const size)
{
    mem_block* block;

    block = (mem_block*) malloc(sizeof(mem_block));
	assert(block);

   if (!mem_block_init(block, size)) {
        if (block)
            free(block);
        return NULL;
    } else {
        return block;
    }
}

/* Destroy an existing mem_block, and everything it contains. */
void memblock::mem_block_free(mem_block * const blockP) 
{
	assert(blockP);		
    assert(blockP->_block);

    mem_block_clean(blockP);
    free(blockP);
}


/* Initialize the contents of the provided xmlrpc_mem_block. */
bool memblock::mem_block_init(mem_block * const blockP, unsign_i const size)
{
    assert(blockP);

	blockP->_used=0;

    blockP->_size = size;
    if (size < BLOCK_ALLOC_MIN)
        blockP->_allocated = BLOCK_ALLOC_MIN;
    else
        blockP->_allocated = size;

    blockP->_block = (void*) malloc(blockP->_allocated);
	assert(blockP->_block);

	return (blockP->_block?true:false);
}


/* Deallocate the contents of the provided mem_block, but not
   the block itself.
*/
void memblock::mem_block_clean(mem_block * const blockP) 
{
    assert(blockP != NULL);
    assert(blockP->_block != NULL);

    free(blockP->_block);
    blockP->_block = 0;
}


/* Get the size of the mem_block. */
unsign_i memblock::mem_block_size(const mem_block * const blockP)
{
    assert(blockP != NULL);
    return blockP->_size;
}



/* Get the contents of the mem_block. */
void* memblock::mem_block_contents(const mem_block * const blockP) 
{
    assert(blockP != NULL);
    return blockP->_block;
}



/* Resize an xmlrpc_mem_block, preserving as much of the contents as
   possible.
*/
bool memblock::mem_block_resize (mem_block * const blockP, unsign_i const size)
{
	unsign_i proposed_alloc;
	void* new_block;

    assert(blockP != NULL);

    /* Check to see if we already have enough space. Maybe we'll get lucky. */
//    if (size <= blockP->_allocated) {
//        blockP->_size = size;
//        return true;
//    }

    /* Calculate a new allocation size. */
#ifdef EFENCE
	proposed_alloc = size+blockP->_size;
#else
    proposed_alloc = blockP->_allocated;
    while (proposed_alloc < size && proposed_alloc <= BLOCK_ALLOC_MAX)
        proposed_alloc *= 2;
#endif /* DEBUG_MEM_ERRORS */

	if (proposed_alloc > BLOCK_ALLOC_MAX){
		abort();
		return false;
	}

    /* Allocate our new memory block. */
	new_block = (void*) malloc(proposed_alloc);
	assert(new_block);
	if(!new_block){
		return false;
	}

    /* Copy over our data and update the xmlrpc_mem_block struct. */
    memcpy(new_block, blockP->_block, blockP->_size);
    free(blockP->_block);
    blockP->_block     = new_block;
//	blockP->_block		= realloc(blockP->_block, proposed_alloc);
    blockP->_size      = proposed_alloc;
    blockP->_allocated = proposed_alloc;
	return true;
}

bool memblock::mem_block_append(mem_block * const blockP, const void * const data, unsign_i const len)
{
	unsign_i size;
	bool result;
    
    assert(blockP != NULL);

    size = blockP->_size;
    result = mem_block_resize(blockP, size + len);
	if(!result)
		return false;

    memcpy(((unsigned char*) blockP->_block) + size, data, len); 
	return true;
}

// push one byte
void memblock::mem_block_push_byte(mem_block * const blockP, const BYTE data)
{
	size_t used;
	size_t size;

	assert(blockP != NULL);

	used = blockP->_used;
	size = blockP->_size;

	if((used+1) > size)
	{
		mem_block_resize(blockP, 256);
	}

	blockP->_used = used+1;

	*((PBYTE)blockP->_block+used)=data;
}

bool memblock::mem_block_push(mem_block * const blockP, const void * const data, unsign_i const len)
{
	size_t used;
	size_t size;

	assert(blockP != NULL);

	used = blockP->_used;
	size = blockP->_size;

	if(used+len > size)
	{
		size = used+len+256;
		blockP->_block = realloc(blockP->_block, size);
		if(!blockP->_block)
			return false;
		blockP->_size = size;
		blockP->_allocated = size;
	}

	memcpy((unsigned char*)blockP->_block + used, data, len);
	blockP->_used = used+len;
	return true;
}

unsign_i memblock::mem_block_used(const mem_block * const blockP)
{
	assert(blockP != NULL);
    return blockP->_used;
}

void memblock::mem_block_reset_used(mem_block * const blockP)
{
	unsign_i used;

	assert(blockP != NULL);
	used = 0;

    blockP->_used = used;
}
