CS3106L - LUIT File-System Laboratory Practice (NON-GRADED)
Student starter files. Read the handout before you begin.

FILES
  fs_semantics.c   modes: dup | fork | links | paths | errors
  fs_blocks.c      direct-to-indirect block-boundary exercise

INSTRUCTIONS
  1. Use a disposable LUIT source tree that permits user-program additions.
     Do not edit protected files or the Makefile of a graded-lab distribution.
  2. Copy fs_semantics.c and fs_blocks.c into its user/ directory.
  3. In that scratch tree, append user/fs_semantics and user/fs_blocks
     to UPROGS in its Makefile.
  4. Build and boot:
       make
       make qemu CPUS=1
  5. Implement all TODOs and run the modes described in the handout.
  6. Remove your test files when you finish.

INCLUDE ORDER IN fs_blocks.c
  #include "ulib.h"
  #include "../kernel/fs.h"

ulib.h must come first: fs.h uses LUIT's integer typedefs. The second
include imports BSIZE and NDIRECT. Do not hard-code their values.
