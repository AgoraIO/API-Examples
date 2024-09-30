/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 23rd, 2018
 * Module:	Memory Management relative utilities header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MM_H__
#define __AOSL_MM_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * aosl memory management malloc like function
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *       size: the allocation size in bytes;
 * Return Value:
 *     non-NULL: success
 *         NULL: failure with aosl_errno set
 * Remarks:
 *    The memory allocated by this function must be freed
 *    via aosl_mm_free when finished using.
 **/
extern __aosl_api__ void *aosl_mm_malloc (size_t size);

/**
 * aosl memory management realloc like function
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *        ptr: the old pointer wanting to realloc;
 *       size: the new allocation size in bytes;
 * Return Value:
 *     non-NULL: success
 *         NULL: failure with aosl_errno set
 * Remarks:
 *    The contents will be unchanged in the range from the
 *    start of the region up to the minimum of the old and
 *    new sizes;
 *    If the new size is larger than the old size, the added
 *    memory will not be initialized;
 *    If ptr is NULL, then the call is equivalent to
 *    aosl_mm_malloc(size), for all values of size;
 *    If size is equal to zero, and ptr is not NULL, then the
 *    call is equivalent to aosl_mm_free(ptr);
 *    Unless ptr is NULL, it must have been returned by an
 *    earlier call to aosl_mm_malloc, aosl_mm_calloc, or
 *    aosl_mm_realloc;
 *    If this function fails, the original block is left
 *    untouched, it is not freed or moved;
 *    The memory allocated by this function must be freed
 *    via aosl_mm_free when finished using.
 **/
extern __aosl_api__ void *aosl_mm_realloc (void *ptr, size_t size);

/**
 * aosl memory management calloc like function
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *      nmemb: number of array elements count;
 *       size: the element size in bytes;
 * Return Value:
 *     non-NULL: success
 *         NULL: failure with aosl_errno set
 * Remarks:
 *    This function allocates memory for an array of nmemb
 *    elements of size bytes each and returns a pointer to
 *    the allocated memory, and the memory is set to zero;
 *    If nmemb or size is 0, then returns NULL;
 *    The memory allocated by this function must be freed
 *    via aosl_mm_free when finished using.
 **/
extern __aosl_api__ void *aosl_mm_calloc (size_t nmemb, size_t size);

/**
 * aosl memory management free like function
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *        ptr: the allocated pointer by aosl_mm_malloc;
 * Return Value:
 *       None.
 * Remarks:
 *    Do NOT try to free a pointer which is not allocated
 *    by aosl_mm_malloc via this function.
 **/
extern __aosl_api__ void aosl_mm_free (void *ptr);

/**
 * Preallocate the objects count to threshold for the memory pool specified by size.
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *       size: specify which the memory pool will be set threshold;
 *  threshold: specify the memory objects count will be preallocated;
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 * Remarks:
 *    If set the threshold first, then the following first time
 *    allocation operation will be faster than normal.
 **/
extern __aosl_api__ int aosl_mm_set_threshold (size_t size, size_t threshold);

/**
 * Set the max objects count to max_val for the memory pool specified by size.
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *       size: specify which the memory pool will be set threshold;
 *    max_val: specify the max memory objects count;
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 * Remarks:
 *    If the max objects were allocated, and the following allocation
 *    will return NULL if no objects was freed.
 **/
extern __aosl_api__ int aosl_mm_set_max (size_t size, size_t max_val);

/**
 * Shrink the memory pool specified by the size parameter.
 * Parameters:
 *       size: specify which the memory pool will be shrinked;
 * Return Value:
 *       None.
 **/
extern __aosl_api__ void aosl_mm_shrink (size_t size);

/**
 * Shrink all the created memory pools.
 * Parameters:
 *       None.
 * Return Value:
 *       None.
 **/
extern __aosl_api__ void aosl_mm_shrink_all (void);

/* the aosl freelist object type */
typedef struct _aosl_freelist_ *aosl_freelist_t;

/**
 * Create a freelist object for efficient memory management.
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *     obj_size: the memory object size in bytes for this freelist;
 *      namefmt: printf like format for the freelist object name;
 *          ...: printf like ... args;
 * Return Value:
 *     non-NULL: success
 *         NULL: failure with aosl_errno set
 * Remarks:
 *    The max name length of freelist object is 32 including
 *    the final '\0'.
 **/
extern __aosl_api__ aosl_freelist_t aosl_freelist_create (size_t obj_size, const char *namefmt, ...);

/**
 * Create a freelist object for efficient memory management.
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *     obj_size: the memory object size in bytes for this freelist;
 *      namefmt: vprintf like format for the freelist object name;
 *         args: vprintf like args;
 * Return Value:
 *     non-NULL: success
 *         NULL: failure with aosl_errno set
 * Remarks:
 *    The max name length of freelist object is 32 including
 *    the final '\0'.
 **/
extern __aosl_api__ aosl_freelist_t aosl_freelist_create_args (size_t obj_size, const char *namefmt, va_list args);

/**
 * Allocate memory from the specified freelist object.
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *           fl: the freelist object;
 * Return Value:
 *     non-NULL: success
 *         NULL: failure with aosl_errno set
 * Remarks:
 *    The allocated memory must be freed by aosl_freelist_free
 *    after finished using.
 **/
extern __aosl_api__ void *aosl_freelist_alloc (aosl_freelist_t fl);

/**
 * Free memory to the specified freelist object.
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *           fl: the freelist object;
 *          ptr: the memory pointer will be freed;
 * Return Value:
 *        None.
 * Remarks:
 *    The ptr must be allocated from fl.
 **/
extern __aosl_api__ void aosl_freelist_free (aosl_freelist_t fl, void *ptr);

/**
 * Preallocate the objects count to threshold for the specified freelist object.
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *         fl: the freelist object which will be set threshold;
 *  threshold: specify the memory objects count will be preallocated;
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 * Remarks:
 *    If set the threshold first, then the following first time
 *    allocation operation will be faster than normal.
 **/
extern __aosl_api__ int aosl_freelist_set_threshold (aosl_freelist_t fl, size_t threshold);

/**
 * Set the max objects count to max_val for the specified freelist object.
 * Note: this is memory pool based implementation, so it
 *       can decrease the fragmentation.
 * Parameters:
 *         fl: the freelist object which will be set max memory objects count;
 *    max_val: specify the max memory objects count;
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 * Remarks:
 *    If the max objects were allocated, and the following allocation
 *    will return NULL if no objects was freed.
 **/
extern __aosl_api__ int aosl_freelist_set_max (aosl_freelist_t fl, size_t max_val);

/**
 * Shrink the freelist specified by the fl.
 * Parameters:
 *         fl: the freelist object which will be shrinked;
 * Return Value:
 *       None.
 **/
extern __aosl_api__ void aosl_freelist_shrink (aosl_freelist_t fl);

/**
 * Destroy the freelist specified by the fl.
 * Parameters:
 *         fl: the freelist object which will be destroyed;
 * Return Value:
 *          0: success
 *         <0: failure with aosl_errno set
 * Remarks:
 *    If there are allocated objects from this freelist, then it
 *    could not be destroyed, so the destroy call would fail.
 **/
extern __aosl_api__ int aosl_freelist_destroy (aosl_freelist_t fl);


extern __aosl_api__ void *aosl_malloc (size_t size);
extern __aosl_api__ void aosl_free (void *ptr);
extern __aosl_api__ void *aosl_calloc (size_t nmemb, size_t size);
extern __aosl_api__ void *aosl_realloc (void *ptr, size_t size);

extern __aosl_api__ char *aosl_strdup (const char *s);

extern __aosl_api__ int aosl_page_size (void);

#define AOSL_PAGE_SIZE (aosl_page_size ())
#define PAGE_ALIGNED_ADDR(a) ((void *)((uintptr_t)(a) & (uintptr_t)(intptr_t)~(AOSL_PAGE_SIZE - 1)))
#define PAGE_ALIGNED_SIZE(s) ((size_t)((size_t)(s) + (AOSL_PAGE_SIZE - 1)) & (uintptr_t)(intptr_t)~(AOSL_PAGE_SIZE - 1))


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_MM_H__ */