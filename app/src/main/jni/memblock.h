#pragma once
#define BLOCK_ALLOC_MIN (16)
#define BLOCK_ALLOC_MAX (128 * 1024 * 1024)
#define EFENCE
typedef unsigned char       BYTE;
typedef BYTE                *PBYTE;
typedef unsigned int        unsign_i;
typedef unsigned short      WORD;
typedef BYTE                *LPBYTE;
typedef struct _mem_block 
{
	unsign_i	_size;
	unsign_i	_allocated;
	void*	_block;

	unsign_i _used;
} mem_block;

class memblock
{
public:
	memblock(void);
	~memblock(void);
public:
	mem_block *mem_block_new(unsign_i const size);
	void mem_block_free(mem_block * const blockP);
	bool mem_block_init(mem_block * const blockP, unsign_i const size);
	void mem_block_clean(mem_block * const blockP);
	unsign_i mem_block_size(const mem_block * const blockP);
	void * mem_block_contents(const mem_block * const blockP);
	bool mem_block_resize (mem_block * const blockP, unsign_i const size);
	bool mem_block_append(mem_block * const blockP, const void * const data, unsign_i const len);
	void mem_block_push_byte(mem_block * const blockP, const BYTE data);
	bool mem_block_push(mem_block * const blockP, const void * const data, unsign_i const len);
	unsign_i mem_block_used(const mem_block * const blockP);
	void mem_block_reset_used(mem_block * const blockP);
};

