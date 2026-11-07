/*
 * CS3106L - LUIT file-system laboratory practice.
 * Student implementation skeleton. No host C library.
 * Integrate ONLY in a disposable copy of the LUIT source tree.
 */
#include "ulib.h"

static void help(void)
{
    printf("Usage: fs_semantics dup|fork|links|paths|errors\n");
}

/* Exercise A: compare shared descriptor offset after dup()
   with an independently opened file. */
static void probe_dup(void)
{
    /* TODO: create a file containing ABCDEFGH; close it;
     * open it for reading, dup the descriptor, and read two
     * bytes alternately from the original and duplicate.
     * Open the same path again and compare offsets.
     */
    printf("TODO: dup experiment\n");
}

/* Exercise B: use wait() to force a child read before the
   parent reads from the inherited descriptor. */
static void probe_fork(void)
{
    /* TODO: never rely on sleeps to order parent and child. */
    printf("TODO: fork experiment\n");
}

/* Exercise C: create a file, add one hard link, unlink
   the two names, and use fstat on an open descriptor. */
static void probe_links(void)
{
    /* TODO: inspect ino, nlink, and size at each stage. */
    printf("TODO: link and unlink experiment\n");
}

/* Exercise D: nested paths and working directory. */
static void probe_paths(void)
{
    /* TODO: verify relative, absolute, '.', and '..' paths. */
    printf("TODO: directory experiment\n");
}

/* Exercise E: distinguish EOF (0) from errors (-1). */
static void probe_errors(void)
{
    /* TODO: test bad fds and read/write permissions. */
    printf("TODO: error experiment\n");
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        help();
        exit(1);
    }
    if (strcmp(argv[1], "dup") == 0) probe_dup();
    else if (strcmp(argv[1], "fork") == 0) probe_fork();
    else if (strcmp(argv[1], "links") == 0) probe_links();
    else if (strcmp(argv[1], "paths") == 0) probe_paths();
    else if (strcmp(argv[1], "errors") == 0) probe_errors();
    else {
        help();
        exit(1);
    }
    exit(0);
}
