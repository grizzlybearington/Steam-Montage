/* See LICENSE file for copyright & license details. */
#define print_oom() \
    fprintf(stderr, "Error: OOM!\n" \
        "%s:%d %s\n", __FILE__, __LINE__, __func__)
