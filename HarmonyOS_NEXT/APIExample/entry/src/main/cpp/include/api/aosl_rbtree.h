/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 28th, 2018
 * Module:	AOSL Red-Black tree header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef	__AOSL_RBTREE_H__
#define	__AOSL_RBTREE_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif


struct aosl_rb_node {
	uintptr_t rb_parent_color;
	struct aosl_rb_node *rb_right;
	struct aosl_rb_node *rb_left;
};

typedef int (*aosl_rb_node_cmp_t) (struct aosl_rb_node *rb_node, struct aosl_rb_node *node, va_list args);

struct aosl_rb_root {
	struct aosl_rb_node *rb_node;
	aosl_rb_node_cmp_t rb_cmp;
	uintptr_t count;
};


#define	aosl_rb_entry(ptr, type, member) container_of(ptr, type, member)

extern __aosl_api__ void aosl_rb_root_init (struct aosl_rb_root *root, aosl_rb_node_cmp_t cmp);


extern __aosl_api__ struct aosl_rb_node **aosl_vfind_rb_links (struct aosl_rb_root *root, struct aosl_rb_node **rb_parent,
	struct aosl_rb_node **pprev, struct aosl_rb_node **pnext, struct aosl_rb_node *node, va_list args);

extern __aosl_api__ struct aosl_rb_node **aosl_find_rb_links (struct aosl_rb_root *root, struct aosl_rb_node **rb_parent,
		struct aosl_rb_node **pprev, struct aosl_rb_node **pnext, struct aosl_rb_node *node, ...);

extern __aosl_api__ struct aosl_rb_node *aosl_vfind_rb_node (struct aosl_rb_root *root, struct aosl_rb_node *node, va_list args);
extern __aosl_api__ struct aosl_rb_node *aosl_find_rb_node (struct aosl_rb_root *root, struct aosl_rb_node *node, ...);

extern __aosl_api__ void aosl_rb_insert_node (struct aosl_rb_root *root, struct aosl_rb_node *node, ...);
extern __aosl_api__ struct aosl_rb_node *aosl_rb_remove (struct aosl_rb_root *root, struct aosl_rb_node *node, ...);

extern __aosl_api__ void aosl_rb_erase (struct aosl_rb_root *root, struct aosl_rb_node *node);

typedef int (*aosl_rb_walk_func_t) (struct aosl_rb_node *node, void *arg);

extern __aosl_api__ void aosl_rb_traverse_dlr (struct aosl_rb_root *root, aosl_rb_walk_func_t func, void *arg);
extern __aosl_api__ void aosl_rb_traverse_ldr (struct aosl_rb_root *root, aosl_rb_walk_func_t func, void *arg);
extern __aosl_api__ void aosl_rb_traverse_lrd (struct aosl_rb_root *root, aosl_rb_walk_func_t func, void *arg);

/* Only for reverse order traverse. */
extern __aosl_api__ void aosl_rb_traverse_rdl (struct aosl_rb_root *root, aosl_rb_walk_func_t func, void *arg);


#ifdef __cplusplus
}
#endif

#endif /* __AOSL_RBTREE_H__ */
