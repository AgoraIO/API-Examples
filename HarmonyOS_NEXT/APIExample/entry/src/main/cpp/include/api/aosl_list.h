/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 16th, 2018
 * Module:	AOSL list implementation header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_LIST_H__
#define __AOSL_LIST_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>


#ifdef __cplusplus
extern "C" {
#endif


#define AOSL_LIST_POISON1  ((void *)((uintptr_t)0x00100100 + 1))
#define AOSL_LIST_POISON2  ((void *)((uintptr_t)0x00200200 + 3))


struct aosl_list_head {
	struct aosl_list_head *next;
	struct aosl_list_head *prev;
};


#define AOSL_LIST_HEAD_INIT(name) { &(name), &(name) }

#define AOSL_DEFINE_LIST_HEAD(name) \
	struct aosl_list_head name = AOSL_LIST_HEAD_INIT(name)

static __inline__ void aosl_list_head_init(struct aosl_list_head *list)
{
	list->next = list;
	list->prev = list;
}

static __inline__ void aosl_list_head_poison (struct aosl_list_head *list)
{
	list->next = (struct aosl_list_head *)AOSL_LIST_POISON1;
	list->prev = (struct aosl_list_head *)AOSL_LIST_POISON2;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __aosl_list_add(struct aosl_list_head *new_node,
			      struct aosl_list_head *prev,
			      struct aosl_list_head *next)
{
	next->prev = new_node;
	new_node->next = next;
	new_node->prev = prev;
	prev->next = new_node;
}

/**
 * aosl_list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void aosl_list_add(struct aosl_list_head *new_node, struct aosl_list_head *head)
{
	__aosl_list_add(new_node, head, head->next);
}


/**
 * aosl_list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void aosl_list_add_tail(struct aosl_list_head *new_node, struct aosl_list_head *head)
{
	__aosl_list_add(new_node, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __aosl_list_del(struct aosl_list_head * prev, struct aosl_list_head * next)
{
	next->prev = prev;
	prev->next = next;
}


/**
 * aosl_list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: aosl_list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void __aosl_list_del_entry(struct aosl_list_head *entry)
{
	__aosl_list_del(entry->prev, entry->next);
}

static inline void aosl_list_del(struct aosl_list_head *entry)
{
	__aosl_list_del(entry->prev, entry->next);
	aosl_list_head_poison (entry);
}

/**
 * aosl_list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void aosl_list_replace(struct aosl_list_head *old,
				struct aosl_list_head *new_node)
{
	new_node->next = old->next;
	new_node->next->prev = new_node;
	new_node->prev = old->prev;
	new_node->prev->next = new_node;
}

static inline void aosl_list_replace_init(struct aosl_list_head *old,
					struct aosl_list_head *new_node)
{
	aosl_list_replace(old, new_node);
	aosl_list_head_init(old);
}

/**
 * aosl_list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void aosl_list_del_init(struct aosl_list_head *entry)
{
	__aosl_list_del_entry(entry);
	aosl_list_head_init(entry);
}

/**
 * aosl_list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void aosl_list_move(struct aosl_list_head *list, struct aosl_list_head *head)
{
	__aosl_list_del_entry(list);
	aosl_list_add(list, head);
}

/**
 * aosl_list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void aosl_list_move_tail(struct aosl_list_head *list,
				  struct aosl_list_head *head)
{
	__aosl_list_del_entry(list);
	aosl_list_add_tail(list, head);
}

/**
 * aosl_list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int aosl_list_is_last(const struct aosl_list_head *list,
				const struct aosl_list_head *head)
{
	return list->next == head;
}

/**
 * aosl_list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int aosl_list_empty(const struct aosl_list_head *head)
{
	return head->next == head;
}

/**
 * aosl_list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using aosl_list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is aosl_list_del_init(). Eg. it cannot be used
 * if another CPU could re-aosl_list_add() it.
 */
static inline int aosl_list_empty_careful(const struct aosl_list_head *head)
{
	struct aosl_list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * aosl_list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void aosl_list_rotate_left(struct aosl_list_head *head)
{
	struct aosl_list_head *first;

	if (!aosl_list_empty(head)) {
		first = head->next;
		aosl_list_move_tail(first, head);
	}
}

/**
 * aosl_list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int aosl_list_is_singular(const struct aosl_list_head *head)
{
	return !aosl_list_empty(head) && (head->next == head->prev);
}

static inline void __aosl_list_cut_position(struct aosl_list_head *list,
		struct aosl_list_head *head, struct aosl_list_head *entry)
{
	struct aosl_list_head *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

/**
 * aosl_list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void aosl_list_cut_position(struct aosl_list_head *list,
		struct aosl_list_head *head, struct aosl_list_head *entry)
{
	if (aosl_list_empty(head))
		return;
	if (aosl_list_is_singular(head) &&
		(head->next != entry && head != entry))
		return;
	if (entry == head)
		aosl_list_head_init(list);
	else
		__aosl_list_cut_position(list, head, entry);
}

static inline void __aosl_list_splice(const struct aosl_list_head *list,
				 struct aosl_list_head *prev,
				 struct aosl_list_head *next)
{
	struct aosl_list_head *first = list->next;
	struct aosl_list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * aosl_list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void aosl_list_splice(const struct aosl_list_head *list,
				struct aosl_list_head *head)
{
	if (!aosl_list_empty(list))
		__aosl_list_splice(list, head, head->next);
}

/**
 * aosl_list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void aosl_list_splice_tail(struct aosl_list_head *list,
				struct aosl_list_head *head)
{
	if (!aosl_list_empty(list))
		__aosl_list_splice(list, head->prev, head);
}

/**
 * aosl_list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void aosl_list_splice_init(struct aosl_list_head *list,
				    struct aosl_list_head *head)
{
	if (!aosl_list_empty(list)) {
		__aosl_list_splice(list, head, head->next);
		aosl_list_head_init(list);
	}
}

/**
 * aosl_list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void aosl_list_splice_tail_init(struct aosl_list_head *list,
					 struct aosl_list_head *head)
{
	if (!aosl_list_empty(list)) {
		__aosl_list_splice(list, head->prev, head);
		aosl_list_head_init(list);
	}
}

/**
 * aosl_list_entry - get the struct for this entry
 * @ptr:	the &struct aosl_list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define aosl_list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * aosl_list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define aosl_list_first_entry(ptr, type, member) \
	aosl_list_entry((ptr)->next, type, member)

/**
 * aosl_list_for_each	-	iterate over a list
 * @pos:	the &struct aosl_list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define aosl_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * aosl_list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct aosl_list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define aosl_list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * aosl_list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct aosl_list_head to use as a loop cursor.
 * @n:		another &struct aosl_list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define aosl_list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * aosl_list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct aosl_list_head to use as a loop cursor.
 * @n:		another &struct aosl_list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define aosl_list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * aosl_list_for_each_entry_t - iterate over list of given type
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define aosl_list_for_each_entry_t(type, pos, head, member)				\
	for (pos = aosl_list_entry((head)->next, type, member);	\
	     &pos->member != (head); 	\
	     pos = aosl_list_entry(pos->member.next, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define aosl_list_for_each_entry(pos, head, member)				\
	for (pos = aosl_list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = aosl_list_entry(pos->member.next, typeof(*pos), member))
#endif

/**
 * aosl_list_for_each_entry_reverse_t - iterate backwards over list of given type.
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define aosl_list_for_each_entry_reverse_t(type, pos, head, member)			\
	for (pos = aosl_list_entry((head)->prev, type, member);	\
	     &pos->member != (head); 	\
	     pos = aosl_list_entry(pos->member.prev, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define aosl_list_for_each_entry_reverse(pos, head, member)			\
	for (pos = aosl_list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = aosl_list_entry(pos->member.prev, typeof(*pos), member))
#endif

/**
 * aosl_list_prepare_entry_t - prepare a pos entry for use in aosl_list_for_each_entry_continue()
 * @type:   the type name.
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in aosl_list_for_each_entry_continue().
 */
#define aosl_list_prepare_entry_t(type, pos, head, member) \
	((pos) ? : aosl_list_entry(head, type, member))

#ifdef __GNUC__
/**
 * aosl_list_prepare_entry - prepare a pos entry for use in aosl_list_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in aosl_list_for_each_entry_continue().
 */
#define aosl_list_prepare_entry(pos, head, member) \
	((pos) ? : aosl_list_entry(head, typeof(*pos), member))
#endif

/**
 * aosl_list_for_each_entry_continue_t - continue iteration over list of given type
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define aosl_list_for_each_entry_continue_t(type, pos, head, member) 		\
	for (pos = aosl_list_entry(pos->member.next, type, member);	\
	     &pos->member != (head);	\
	     pos = aosl_list_entry(pos->member.next, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define aosl_list_for_each_entry_continue(pos, head, member) 		\
	for (pos = aosl_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = aosl_list_entry(pos->member.next, typeof(*pos), member))
#endif

/**
 * aosl_list_for_each_entry_continue_reverse_t - iterate backwards from the given point
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define aosl_list_for_each_entry_continue_reverse_t(type, pos, head, member)		\
	for (pos = aosl_list_entry(pos->member.prev, type, member);	\
	     &pos->member != (head);	\
	     pos = aosl_list_entry(pos->member.prev, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define aosl_list_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = aosl_list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = aosl_list_entry(pos->member.prev, typeof(*pos), member))
#endif

/**
 * aosl_list_for_each_entry_from_t - iterate over list of given type from the current point
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define aosl_list_for_each_entry_from_t(type, pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = aosl_list_entry(pos->member.next, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define aosl_list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = aosl_list_entry(pos->member.next, typeof(*pos), member))
#endif

/**
 * aosl_list_for_each_entry_safe_t - iterate over list of given type safe against removal of list entry
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define aosl_list_for_each_entry_safe_t(type, pos, n, head, member)			\
	for (pos = aosl_list_entry((head)->next, type, member),	\
		n = aosl_list_entry(pos->member.next, type, member);	\
	     &pos->member != (head); 					\
	     pos = n, n = aosl_list_entry(n->member.next, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define aosl_list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = aosl_list_entry((head)->next, typeof(*pos), member),	\
		n = aosl_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = aosl_list_entry(n->member.next, typeof(*n), member))
#endif

/**
 * aosl_list_for_each_entry_safe_continue_t - continue list iteration safe against removal
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define aosl_list_for_each_entry_safe_continue_t(type, pos, n, head, member) 		\
	for (pos = aosl_list_entry(pos->member.next, type, member), 		\
		n = aosl_list_entry(pos->member.next, type, member);		\
	     &pos->member != (head);						\
	     pos = n, n = aosl_list_entry(n->member.next, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define aosl_list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = aosl_list_entry(pos->member.next, typeof(*pos), member), 		\
		n = aosl_list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = aosl_list_entry(n->member.next, typeof(*n), member))
#endif

/**
 * aosl_list_for_each_entry_safe_from_t - iterate over list from current point safe against removal
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define aosl_list_for_each_entry_safe_from_t(type, pos, n, head, member) 			\
	for (n = aosl_list_entry(pos->member.next, type, member);		\
	     &pos->member != (head);						\
	     pos = n, n = aosl_list_entry(n->member.next, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define aosl_list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = aosl_list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = aosl_list_entry(n->member.next, typeof(*n), member))
#endif

/**
 * aosl_list_for_each_entry_safe_reverse_t - iterate backwards over list safe against removal
 * @type:   the type name.
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define aosl_list_for_each_entry_safe_reverse_t(type, pos, n, head, member)		\
	for (pos = aosl_list_entry((head)->prev, type, member),	\
		n = aosl_list_entry(pos->member.prev, type, member);	\
	     &pos->member != (head); 					\
	     pos = n, n = aosl_list_entry(n->member.prev, type, member))

#ifdef __GNUC__
/**
 * aosl_list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define aosl_list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = aosl_list_entry((head)->prev, typeof(*pos), member),	\
		n = aosl_list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = aosl_list_entry(n->member.prev, typeof(*n), member))
#endif

/**
 * aosl_list_safe_reset_next_t - reset a stale aosl_list_for_each_entry_safe loop
 * @type:   the type name.
 * @pos:	the loop cursor used in the aosl_list_for_each_entry_safe loop
 * @n:		temporary storage used in aosl_list_for_each_entry_safe
 * @member:	the name of the list_struct within the struct.
 *
 * aosl_list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and aosl_list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define aosl_list_safe_reset_next_t(type, pos, n, member)				\
	n = aosl_list_entry(pos->member.next, type, member)

#ifdef __GNUC__
/**
 * aosl_list_safe_reset_next - reset a stale aosl_list_for_each_entry_safe loop
 * @pos:	the loop cursor used in the aosl_list_for_each_entry_safe loop
 * @n:		temporary storage used in aosl_list_for_each_entry_safe
 * @member:	the name of the list_struct within the struct.
 *
 * aosl_list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and aosl_list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define aosl_list_safe_reset_next(pos, n, member)				\
	n = aosl_list_entry(pos->member.next, typeof(*pos), member)
#endif

static __inline__ struct aosl_list_head *aosl_list_head (struct aosl_list_head *head)
{
	if (!aosl_list_empty (head))
		return head->next;

	return NULL;
}

static __inline__ struct aosl_list_head *aosl_list_next (struct aosl_list_head *head, struct aosl_list_head *node)
{
	node = node->next;
	if (node != head)
		return node;

	return NULL;
}

static __inline__ struct aosl_list_head *aosl_list_prev (struct aosl_list_head *head, struct aosl_list_head *node)
{
	node = node->prev;
	if (node != head)
		return node;

	return NULL;
}

static inline struct aosl_list_head *aosl_list_tail (struct aosl_list_head *head)
{
	if (!aosl_list_empty (head))
		return head->prev;

	return NULL;
}

#ifdef __GNUC__
#define aosl_list_head_entry(list, type, member) \
({ \
	struct aosl_list_head *__$p$ = aosl_list_head (list); \
	type *entry = (__$p$ != NULL) ? aosl_list_entry (__$p$, type, member) : NULL; \
	entry; \
})

#define aosl_list_next_entry(list, pos, type, member) \
({ \
	struct aosl_list_head *__$p$ = aosl_list_next (list, &(pos)->member); \
	type *entry = (__$p$ != NULL) ? aosl_list_entry (__$p$, type, member) : NULL; \
	entry; \
})

#define aosl_list_prev_entry(list, pos, type, member) \
({ \
	struct aosl_list_head *__$p$ = aosl_list_prev (list, &(pos)->member); \
	type *entry = (__$p$ != NULL) ? aosl_list_entry (__$p$, type, member) : NULL; \
	entry; \
})

#define aosl_list_tail_entry(list, type, member) \
({ \
	struct aosl_list_head *__$p$ = aosl_list_tail (list); \
	type *entry = (__$p$ != NULL) ? aosl_list_entry (__$p$, type, member) : NULL; \
	entry; \
})
#else
#define aosl_list_head_entry(list, type, member) \
	((aosl_list_head (list) != NULL) ? aosl_list_entry (aosl_list_head (list), type, member) : NULL)

#define aosl_list_next_entry(list, pos, type, member) \
	((aosl_list_next (list, &(pos)->member) != NULL) ? aosl_list_entry (aosl_list_next (list, &(pos)->member), type, member) : NULL)

#define aosl_list_prev_entry(list, pos, type, member) \
	((aosl_list_prev (list, &(pos)->member) != NULL) ? aosl_list_entry (aosl_list_prev (list, &(pos)->member), type, member) : NULL)

#define aosl_list_tail_entry(list, type, member) \
	((aosl_list_tail (list) != NULL) ? aosl_list_entry (aosl_list_tail (list), type, member) : NULL)
#endif

static inline struct aosl_list_head *aosl_list_remove_head (struct aosl_list_head *head)
{
	if (!aosl_list_empty (head)) {
		struct aosl_list_head *__$p$ = head->next;
		aosl_list_del (__$p$);
		return __$p$;
	}

	return NULL;
}

static inline struct aosl_list_head *aosl_list_remove_head_init (struct aosl_list_head *head)
{
	if (!aosl_list_empty (head)) {
		struct aosl_list_head *__$p$ = head->next;
		aosl_list_del_init (__$p$);
		return __$p$;
	}

	return NULL;
}

#ifdef __GNUC__
#define aosl_list_remove_head_entry(list, type, member) \
({ \
	struct aosl_list_head *__$p$ = aosl_list_remove_head (list); \
	type *entry = (__$p$ != NULL) ? aosl_list_entry (__$p$, type, member) : NULL; \
	entry; \
})

#define aosl_list_remove_head_entry_init(list, type, member) \
({ \
	struct aosl_list_head *__$p$ = aosl_list_remove_head_init (list); \
	type *entry = (__$p$ != NULL) ? aosl_list_entry (__$p$, type, member) : NULL; \
	entry; \
})
#else
#define aosl_list_remove_head_entry(list, type, member) \
	((aosl_list_head (list) != NULL) ? aosl_list_entry (aosl_list_remove_head (list), type, member) : NULL)

#define aosl_list_remove_head_entry_init(list, type, member) \
	((aosl_list_head (list) != NULL) ? aosl_list_entry (aosl_list_remove_head_init (list), type, member) : NULL)
#endif

static inline struct aosl_list_head *aosl_list_remove_tail (struct aosl_list_head *head)
{
	if (!aosl_list_empty (head)) {
		struct aosl_list_head *__$p$ = head->prev;
		aosl_list_del (__$p$);
		return __$p$;
	}

	return NULL;
}

static inline struct aosl_list_head *aosl_list_remove_tail_init (struct aosl_list_head *head)
{
	if (!aosl_list_empty (head)) {
		struct aosl_list_head *__$p$ = head->prev;
		aosl_list_del_init (__$p$);
		return __$p$;
	}

	return NULL;
}

#ifdef __GNUC__
#define aosl_list_remove_tail_entry(list, type, member) \
({ \
	struct aosl_list_head *__$p$ = aosl_list_remove_tail (list); \
	type *entry = (__$p$ != NULL) ? aosl_list_entry (__$p$, type, member) : NULL; \
	entry; \
})

#define aosl_list_remove_tail_entry_init(list, type, member) \
({ \
	struct aosl_list_head *__$p$ = aosl_list_remove_tail_init (list); \
	type *entry = (__$p$ != NULL) ? aosl_list_entry (__$p$, type, member) : NULL; \
	entry; \
})
#else
#define aosl_list_remove_tail_entry(list, type, member) \
	((aosl_list_tail (list) != NULL) ? aosl_list_entry (aosl_list_remove_tail (list), type, member) : NULL)

#define aosl_list_remove_tail_entry_init(list, type, member) \
	((aosl_list_tail (list) != NULL) ? aosl_list_entry (aosl_list_remove_tail_init (list), type, member) : NULL)
#endif


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_LIST_H__ */