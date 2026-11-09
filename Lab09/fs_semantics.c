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
    char buf[16];
    memset(buf, 0, sizeof(buf));

    // 1. Create a file containing ABCDEFGH and close it
    int fd = open("test_dup.txt", O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("probe_dup: failed to create test_dup.txt\n");
        return;
    }
    write(fd, "ABCDEFGH", 8);
    close(fd);

    // 2. Open for reading as descriptor A, duplicate to B
    int fd_a = open("test_dup.txt", O_RDONLY);
    if (fd_a < 0) {
        printf("probe_dup: failed to open test_dup.txt as A\n");
        return;
    }
    int fd_b = dup(fd_a);
    if (fd_b < 0) {
        printf("probe_dup: failed to dup A to B\n");
        close(fd_a);
        return;
    }

    // 3. Read two bytes through A, then two through B
    memset(buf, 0, sizeof(buf));
    read(fd_a, buf, 2);
    printf("read(A, 2) = %s\n", buf); // expects "AB"

    memset(buf, 0, sizeof(buf));
    read(fd_b, buf, 2);
    printf("read(B, 2) = %s\n", buf); // expects "CD" (shared offset!)

    // 4. Open same filename independently as descriptor C
    int fd_c = open("test_dup.txt", O_RDONLY);
    if (fd_c < 0) {
        printf("probe_dup: failed to open test_dup.txt as C\n");
        close(fd_a);
        close(fd_b);
        return;
    }
    memset(buf, 0, sizeof(buf));
    read(fd_c, buf, 2);
    printf("read(C, 2) = %s\n", buf); // expects "AB" (independent offset = 0)

    // 5. Read another two bytes through A
    memset(buf, 0, sizeof(buf));
    read(fd_a, buf, 2);
    printf("read(A, 2) = %s\n", buf); // expects "EF" (shared offset continues)

    // 6. Close B, verify A is still valid
    close(fd_b);
    memset(buf, 0, sizeof(buf));
    read(fd_a, buf, 1);
    printf("close(B); read(A, 1) = %s\n", buf); // expects "G"

    close(fd_a);
    close(fd_c);
    unlink("test_dup.txt");
    printf("probe_dup: completed successfully\n");
}

/* Exercise B: use wait() to force a child read before the
   parent reads from the inherited descriptor. */
static void probe_fork(void)
{
    char buf[16];
    memset(buf, 0, sizeof(buf));

    // 1. Create file containing ABCDEFGH
    int fd = open("test_fork.txt", O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("probe_fork: failed to create test_fork.txt\n");
        return;
    }
    write(fd, "ABCDEFGH", 8);
    close(fd);

    // 2. Open once in parent before fork()
    int shared_fd = open("test_fork.txt", O_RDONLY);
    if (shared_fd < 0) {
        printf("probe_fork: failed to open test_fork.txt\n");
        return;
    }

    // 3. Fork: child reads 2 bytes, exits; parent waits and then reads 2 bytes
    int pid = fork();
    if (pid < 0) {
        printf("probe_fork: fork failed\n");
        close(shared_fd);
        return;
    }

    if (pid == 0) {
        // Child process
        memset(buf, 0, sizeof(buf));
        read(shared_fd, buf, 2);
        printf("child read inherited fd: %s\n", buf); // expects "AB"
        close(shared_fd);
        exit(0);
    } else {
        // Parent process: wait for child to complete
        wait(0);
        memset(buf, 0, sizeof(buf));
        read(shared_fd, buf, 2);
        printf("parent read inherited fd: %s\n", buf); // expects "CD"
        close(shared_fd);
    }

    // 4. Repeat with independent reopening
    pid = fork();
    if (pid < 0) {
        printf("probe_fork: second fork failed\n");
        return;
    }

    if (pid == 0) {
        int child_fd = open("test_fork.txt", O_RDONLY);
        memset(buf, 0, sizeof(buf));
        read(child_fd, buf, 2);
        printf("child independent open read: %s\n", buf); // expects "AB"
        close(child_fd);
        exit(0);
    } else {
        wait(0);
        int parent_fd = open("test_fork.txt", O_RDONLY);
        memset(buf, 0, sizeof(buf));
        read(parent_fd, buf, 2);
        printf("parent independent open read: %s\n", buf); // expects "AB"
        close(parent_fd);
    }

    unlink("test_fork.txt");
    printf("probe_fork: completed successfully\n");
}

/* Exercise C: create a file, add one hard link, unlink
   the two names, and use fstat on an open descriptor. */
static void probe_links(void)
{
    struct stat st;
    char buf[16];
    memset(buf, 0, sizeof(buf));

    // 1. Create fs_life_a and write known content
    int fd = open("fs_life_a", O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("probe_links: failed to create fs_life_a\n");
        return;
    }
    write(fd, "LINKDATA", 8);
    close(fd);

    // Open a descriptor for reading that we will keep open throughout
    int retained_fd = open("fs_life_a", O_RDONLY);
    if (retained_fd < 0) {
        printf("probe_links: failed to open fs_life_a for read\n");
        return;
    }

    fstat(retained_fd, &st);
    printf("After create: ino=%d, nlink=%d, size=%d\n", st.ino, st.nlink, (int)st.size);

    // 2. Create second link: fs_life_b
    if (link("fs_life_a", "fs_life_b") < 0) {
        printf("probe_links: link failed\n");
        close(retained_fd);
        return;
    }

    int fd_b = open("fs_life_b", O_RDONLY);
    fstat(fd_b, &st);
    printf("After link:   ino=%d, nlink=%d, size=%d\n", st.ino, st.nlink, (int)st.size);
    close(fd_b);

    // 3. Unlink first name (fs_life_a)
    unlink("fs_life_a");
    fstat(retained_fd, &st);
    printf("After unlk 1: ino=%d, nlink=%d, size=%d\n", st.ino, st.nlink, (int)st.size);

    // 4. Unlink second name (fs_life_b): nlink drops to 0!
    unlink("fs_life_b");
    fstat(retained_fd, &st);
    printf("After unlk 2: ino=%d, nlink=%d, size=%d\n", st.ino, st.nlink, (int)st.size);

    // Verify open fails on deleted names
    if (open("fs_life_a", O_RDONLY) >= 0 || open("fs_life_b", O_RDONLY) >= 0) {
        printf("FAIL: open succeeded on unlinked path\n");
    } else {
        printf("open on unlinked paths rejected as expected\n");
    }

    // Retained descriptor is STILL USABLE and can read data
    int r = read(retained_fd, buf, 8);
    buf[8] = '\0';
    printf("Read from retained descriptor: %d bytes, content: %s\n", r, buf);

    // 5. Close final descriptor: inode and blocks now become reclaimable
    close(retained_fd);
    printf("probe_links: completed successfully\n");
}

/* Exercise D: nested paths and working directory. */
static void probe_paths(void)
{
    struct stat st1, st2, st3;

    if (mkdir("fs_tree") < 0) {
        printf("probe_paths: mkdir fs_tree failed\n");
        return;
    }
    if (mkdir("fs_tree/inner") < 0) {
        printf("probe_paths: mkdir fs_tree/inner failed\n");
        return;
    }

    int fd = open("fs_tree/inner/file.txt", O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("probe_paths: create file failed\n");
        return;
    }
    write(fd, "NESTED", 6);
    close(fd);

    stat("fs_tree/inner/file.txt", &st1);
    printf("Initial stat: ino=%d, size=%d\n", st1.ino, (int)st1.size);

    // Change directory to inner
    if (chdir("fs_tree/inner") < 0) {
        printf("probe_paths: chdir failed\n");
        return;
    }

    // Open via relative path
    fd = open("file.txt", O_RDONLY);
    if (fd < 0) {
        printf("probe_paths: open relative failed\n");
    } else {
        fstat(fd, &st2);
        printf("Relative open: ino=%d (matches: %d)\n", st2.ino, st1.ino == st2.ino);
        close(fd);
    }

    // Open via '.' path
    fd = open("./file.txt", O_RDONLY);
    if (fd < 0) {
        printf("probe_paths: open ./file.txt failed\n");
    } else {
        fstat(fd, &st3);
        printf("Dot path open: ino=%d (matches: %d)\n", st3.ino, st1.ino == st3.ino);
        close(fd);
    }

    // Attempt to unlink nonempty directory (must fail)
    int err = unlink("../inner");
    printf("Unlink nonempty dir returned: %d (expected -1)\n", err);

    // Clean up
    unlink("file.txt");
    chdir("../..");
    unlink("fs_tree/inner");
    unlink("fs_tree");
    printf("probe_paths: completed successfully\n");
}

/* Exercise E: distinguish EOF (0) from errors (-1). */
static void probe_errors(void)
{
    char buf[16];
    int fd;

    // 1. Read from descriptor opened O_WRONLY -> error (-1)
    fd = open("err_file.txt", O_CREATE | O_WRONLY);
    int r1 = read(fd, buf, 1);
    printf("Read O_WRONLY fd:   %d (expected -1)\n", r1);
    close(fd);

    // 2. Write to descriptor opened O_RDONLY -> error (-1)
    fd = open("err_file.txt", O_RDONLY);
    int r2 = write(fd, "X", 1);
    printf("Write O_RDONLY fd:  %d (expected -1)\n", r2);

    // 3. Read at EOF -> returns 0 (not -1!)
    char eof_buf[16];
    read(fd, eof_buf, sizeof(eof_buf)); // read to EOF
    int r3 = read(fd, eof_buf, 1);
    printf("Read at EOF:        %d (expected 0)\n", r3);
    close(fd);
    unlink("err_file.txt");

    // 4. Open nonexistent path without O_CREATE -> error (-1)
    int r4 = open("no_such_file_99999.txt", O_RDONLY);
    printf("Open nonexistent:   %d (expected -1)\n", r4);

    // 5. Read from invalid descriptor -> error (-1)
    int r5 = read(99, buf, 1);
    printf("Read invalid fd 99: %d (expected -1)\n", r5);

    // 6. Hard link to directory -> error (-1)
    mkdir("err_test_dir");
    int r6 = link("err_test_dir", "err_dir_link");
    printf("Link to directory:  %d (expected -1)\n", r6);

    // 7. Unlink nonempty directory -> error (-1)
    mkdir("err_test_dir/sub");
    int r7 = unlink("err_test_dir");
    printf("Unlink non-empty:   %d (expected -1)\n", r7);

    unlink("err_test_dir/sub");
    unlink("err_test_dir");
    printf("probe_errors: completed successfully\n");
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
