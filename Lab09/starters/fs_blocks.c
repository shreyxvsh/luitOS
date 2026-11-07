/*
 * CS3106L - LUIT file-system block-boundary and readback study.
 * Student implementation skeleton. The include below is valid when
 * copied into LUIT/user/, with kernel/fs.h at LUIT/kernel/fs.h.
 */
#include "ulib.h"
#include "../kernel/fs.h"  /* Defines BSIZE, NDIRECT; uint32 comes from ulib.h */

#define SCRATCH_N 1024
static char buf[SCRATCH_N];

/* Deterministic byte pattern, independent of execution order. */
static char pattern(uint32 offset)
{
    return (char)('A' + ((offset * 7u + 3u) % 26u));
}

/* Fill buf with the bytes expected at logical file offset. */
static void fill(uint32 offset, int n)
{
    for (int i = 0; i < n; i++)
        buf[i] = pattern(offset + (uint32)i);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    /*
     * TODO:
     * 1. Use the imported constants: BSIZE and NDIRECT.
     *    Choose file lengths NDIRECT*BSIZE-1,
     *    NDIRECT*BSIZE, NDIRECT*BSIZE+1,
     *    and (NDIRECT+1)*BSIZE+17.
     * 2. Write deterministic patterns with partial-block writes.
     * 3. Check fstat().size and close/reopen each file.
     * 4. Read back in chunk sizes 1, 37, 512, and 1024 bytes.
     * 5. Verify every byte using pattern(logical_offset).
     * 6. Unlink experiment files after successful verification.
     *
     * Read user/ulib.h and kernel/fs.h from YOUR LUIT tree.
     * This template intentionally does not implement the solution.
     */
    fill(0, 8);
    printf("Template ready: first=%c, eighth=%c\n", buf[0], buf[7]);
    exit(0);
}
