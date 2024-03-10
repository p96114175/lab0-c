#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list_sort.c"
/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_node = malloc(sizeof(struct list_head));
    if (!new_node) {
        free(new_node);
        return NULL;
    }
    INIT_LIST_HEAD(new_node);
    return new_node;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l || list_empty(l)) {
        free(l);
        return;
    }
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        q_release_element(entry);
    }
    free(l);
}
/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    size_t len = strlen(s) + 1;
    new_element->value = malloc(len * sizeof(char));
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    memcpy(new_element->value, s, len);
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return NULL;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return NULL;
    size_t len = strlen(s) + 1;
    new_element->value = malloc(len * sizeof(char));
    if (!new_element->value) {
        free(new_element);
        return NULL;
    }
    memcpy(new_element->value, s, strlen(s) + 1);
    list_add_tail(&new_element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;
    element_t *entry = list_first_entry(head, element_t, list);
    list_del(&entry->list);
    if (sp)
        strncpy(sp, entry->value, bufsize - 1);
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;
    element_t *entry = list_last_entry(head, element_t, list);
    list_del(&entry->list);
    if (sp) {
        strncpy(sp, entry->value, bufsize - 1);
    }
    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *cur;
    list_for_each (cur, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return NULL;
    struct list_head **indir = &head;
    for (struct list_head *cur = head->next;
         (*indir)->prev != cur && (*indir) != cur; indir = &(*indir)->prev) {
        cur = cur->next;
    }
    struct list_head *mid = (*indir)->prev;
    list_del(mid);
    q_release_element(list_entry(mid, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return NULL;
    element_t *cur, *safe;
    bool del_final = false;
    list_for_each_entry_safe (cur, safe, head, list) {
        if (cur->list.next != head && strcmp(cur->value, safe->value) == 0) {
            list_del(&cur->list);
            q_release_element(cur);
            del_final = true;
        } else if (del_final) {
            list_del(&cur->list);
            q_release_element(cur);
            del_final = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *node;
    list_for_each (node, head) {
        if (node->next == head)
            break;
        list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;
    struct list_head *node, *safe, head_to, *head_from = head;
    unsigned char count = 0;
    INIT_LIST_HEAD(&head_to);
    list_for_each_safe (node, safe, head) {
        count++;
        if (count != k) {
            list_cut_position(&head_to, head_from, node);
            q_reverse(&head_to);
            list_splice_init(&head_to, head_from);
            head_from = safe->prev;
            count = 0;
        }
    }
}
// Merge the elements
struct list_head *merge(struct list_head *l1,
                        struct list_head *l2,
                        bool descend)
{
    // merge sorted l1 and sorted l2
    struct list_head temp;
    struct list_head *cur = &temp;
    if (!l1 && !l2)
        return NULL;
    while (l1 && l2) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) < 0) {
            cur->next = l1;
            l1 = l1->next;
        } else {
            cur->next = l2;
            l2 = l2->next;
        }
        cur = cur->next;
    }
    if (l1)
        cur->next = l1;
    if (l2)
        cur->next = l2;
    return temp.next;
}
// Devide elements of queue into two parts.
struct list_head *q_divide(struct list_head *head, bool descend)
{
    if (!head->next)
        return head;
    struct list_head *fast = head->next;
    struct list_head *slow = head;
    struct list_head *l1, *l2;
    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    // sort each list
    l1 = q_divide(head, descend);
    l2 = q_divide(fast, descend);
    return merge(l1, l2, descend);
}



/* Sort elements of queue in ascending order */
void _q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = q_divide(head->next, descend);
    struct list_head *cur = head, *safe = head->next;
    while (safe) {
        safe->prev = cur;
        cur = safe;
        safe = safe->next;
    }
    cur->next = head;
    head->prev = cur;
}
void q_sort(struct list_head *head, bool descend)
{
    list_sort(head, list_cmp);
    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    element_t *r = list_entry(head->prev, element_t, list);
    element_t *l = list_entry(head->prev->prev, element_t, list);
    while (&l->list != head) {
        if (strcmp(l->value, r->value) > 0) {
            list_del(&l->list);
        } else {
            l = list_entry(l->list.prev, element_t, list);
            r = list_entry(r->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    element_t *r = list_entry(head->prev, element_t, list);
    element_t *l = list_entry(head->prev->prev, element_t, list);
    while (&l->list != head) {
        if (strcmp(l->value, r->value) < 0) {
            list_del(&l->list);
        } else {
            l = list_entry(l->list.prev, element_t, list);
            r = list_entry(r->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return false;
    queue_contex_t *init = container_of(head->next, queue_contex_t, chain);
    struct list_head *cur = head->next->next;
    for (; cur != head; cur = cur->next) {
        queue_contex_t *queue = container_of(cur, queue_contex_t, chain);
        list_splice_init(queue->q, init->q);
        queue->size = 0;
    }
    q_sort(init->q, descend);
    init->size = q_size(init->q);
    return init->size;
}

static inline void list_swap(struct list_head *a, struct list_head *b)
{
    if (list_empty(a) && list_empty(b))
        return;
    struct list_head *b_positon = b->prev;
    list_del(b);
    b->next = a->next;
    b->prev = a->prev;
    b->next->prev = b;
    b->prev->next = b;
    if (b_positon == a)
        b_positon = b;
    list_add(a, b_positon);
}
/* Fisher-Yates shuffle Algorithm */
bool q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *curr = head->next;
    int size = q_size(head);
    while (size > 1) {
        int random = rand() % size;
        struct list_head *temp = curr->prev;
        struct list_head *old = curr;
        while (random--) {  // find the random node.
            old = old->next;
        }
        if (curr != old)
            list_swap(curr, old);  // Exchange elements.
        if (temp != old)
            curr = temp;
        size--;
    }
    return true;
}
