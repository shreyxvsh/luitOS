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

static void test_boundary(const char *filename, uint32 target_len)
{
    struct stat st;
    printf("Testing %s: len=%d bytes\n", filename, target_len);

    // 1. Write using uneven chunk lengths (37, 513, 1024)
    int fd = open(filename, O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("FAIL: open for write %s\n", filename);
        return;
    }

    uint32 written = 0;
    int write_chunks[] = {37, 513, 1024};
    int chunk_idx = 0;

    while (written < target_len) {
        uint32 remaining = target_len - written;
        uint32 chunk = (uint32)write_chunks[chunk_idx % 3];
        if (chunk > remaining) chunk = remaining;

        fill(written, (int)chunk);
        int w = write(fd, buf, (int)chunk);
        if (w != (int)chunk) {
            printf("FAIL: short write %s at offset %d (w=%d, exp=%d)\n",
                   filename, written, w, (int)chunk);
            close(fd);
            return;
        }
        written += chunk;
        chunk_idx++;
    }

    // 2. Check fstat logical file size
    if (fstat(fd, &st) < 0) {
        printf("FAIL: fstat %s\n", filename);
        close(fd);
        return;
    }
    if (st.size != target_len) {
        printf("FAIL: size mismatch %s (got %d, expected %d)\n",
               filename, (int)st.size, target_len);
        close(fd);
        return;
    }
    close(fd);

    // 3. Reopen for reading
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("FAIL: open for read %s\n", filename);
        return;
    }

    // 4. Read back using chunk sizes 1, 37, 512, and 1024 bytes
    uint32 read_total = 0;
    int read_chunks[] = {1, 37, 512, 1024};
    int read_idx = 0;

    while (read_total < target_len) {
        uint32 req = (uint32)read_chunks[read_idx % 4];
        read_idx++;

        int r = read(fd, buf, (int)req);
        if (r <= 0) break;

        for (int i = 0; i < r; i++) {
            char exp = pattern(read_total + (uint32)i);
            if (buf[i] != exp) {
                printf("FAIL: data mismatch in %s at byte %d (got %c, exp %c)\n",
                       filename, read_total + (uint32)i, buf[i], exp);
                close(fd);
                unlink(filename);
                return;
            }
        }
        read_total += (uint32)r;
    }

    if (read_total != target_len) {
        printf("FAIL: total read %d != expected %d in %s\n",
               read_total, target_len, filename);
        close(fd);
        unlink(filename);
        return;
    }

    // Verify EOF returns 0
    int eof_r = read(fd, buf, 1);
    if (eof_r != 0) {
        printf("FAIL: EOF read returned %d instead of 0\n", eof_r);
    }

    close(fd);
    unlink(filename);
    printf("PASS: %s verified byte-for-byte\n", filename);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    // Case P: NDIRECT * BSIZE - 1
    uint32 P = NDIRECT * BSIZE - 1;
    // Case Q: NDIRECT * BSIZE
    uint32 Q = NDIRECT * BSIZE;
    // Case R: NDIRECT * BSIZE + 1
    uint32 R = NDIRECT * BSIZE + 1;
    // Case S: (NDIRECT + 1) * BSIZE + 17
    uint32 S = (NDIRECT + 1) * BSIZE + 17;

    printf("Starting block boundary tests (BSIZE=%d, NDIRECT=%d)\n", BSIZE, NDIRECT);
    test_boundary("case_P.dat", P);
    test_boundary("case_Q.dat", Q);
    test_boundary("case_R.dat", R);
    test_boundary("case_S.dat", S);

    printf("All block boundary tests PASS.\n");
    exit(0);
}
