/**
 * Define the function needed by list_sort.
 **/
typedef int (*list_cmp_func_t)(const struct list_head *,
                               const struct list_head *);
#define unlikely
#define IS_ERR_VALUE(x) \
    unlikely((unsigned long) (void *) (x) >= (unsigned long) -MAX_ERRNO)
#define likely(x) __builtin_expect(!!(x), 1)