# Demonstrating Big and Little Endian Architectures

The problem with architectures' endianness becomes apparent when trying to transfer data between them.  When we 
constrain ourselves to using the same architecture for all data processing, we don't need to concern ourselves with 
the problems reading and writing the data to other architectures.

Most of our modern architectures are little endian, but some software runs on older machines using big endian 
architectures.  Besides big and little endian, there are other endianness formats such as middle endian and network 
byte order.  Data corruption is a by-product of data that is sent from one architecture to another without 
considering the endianness of the data.

For this code, I will be focusing on big and little endian architectures.

## Byte Swapping

Let's examine a simple case of a 4-byte integer.  Expressed as a series of bytes in big endian, from most 
significant byte to least significant byte, we get the following structure:

```[byte4][byte3][byte2][byte1]```

The most significant byte is the first byte (byte4).  Mathematically, the value of the integer would be byte4 * 
2<sup>24</sup> + byte3 * 2<sup>16</sup> + byte2 * 2<sup>8</sup> + byte1.

Compare this to little endian, in which the structure is reordered such that the least significant byte is first:

```[byte1][byte2][byte3][byte4]```

Mathematically, the value of the integer is calculated using the same formula as for the big endian architecture.

To convert to and from big endian and little endian, we need to swap the bytes of the fundamental data types.  Since 
we have the mathematical method for calculating the swap, we can use it to perform the full swap.  We'll use `ui` 
for the integer whose bytes we are swapping:

```((ui << 24) | ((ui << 8) & 0x00FF0000) | ((ui >> 8) & 0x0000FF00) | (ui >> 24))```

## Byte Swapping in Data Structures

Swapping for other data types requires us to be mindful of the number of bytes in the data type.  For integral 
types like `long`, four pairs of bytes need to be swapped (assuming 64 bits for the `long` data type).  For `char`, no 
swapping is necessary.  Swapping composite types is more difficult.  First, the integral data types nested within 
the composite type need to be swapped appropriately.  Secondly, word boundaries may be staggered when an integral 
type does not land on a word boundary.  Consider the following:

```
struct {
    char c;
    int i;
} s1;

struct __attribute__((__packed__)) {
    char c;
    int i;
} s2;

struct {
    char c;
    char pad1[3];
    int i;
} s3;

printf ("sizeof (struct { char c; int i; }):  %lu\n", sizeof (s1));
printf ("sizeof (struct __attribute__((__packed__)) { char c; int i; }):  %lu\n", sizeof (s2));
printf ("sizeof (struct { char c; char pad1[3]; int i; }):  %lu\n", sizeof (s3));
```

A typical C compiler will give a `sizeof (s1)` result of 8 bytes, `sizeof (s2)` result of 5 bytes, and `sizeof (s3)`
result of 8 bytes.  This important detail means that the location of `c` in `s1` is probably going to be different
based on the architecture.  We can solve the problem by using `__attribute__((__packed__))` on the definition of 
the structure or by adding pad fields.  I recommend against using pad fields as they tend to be uninitialized or 
lost in the number of padded fields required; this can lead to compiler warnings and sloppy, unreadable and 
harder-to-maintain code as the developer is left trying to decipher why a value ended up in the pad field.  Using 
`__attribute__((__packed__))` gives much more control to the developer on the location of the data in the data type 
without sacrificing the readability of the code.

## Bit Fields

Bit fields are another problem with these structures.  In big endian architecture the order of the bit fields is 
opposite that of little endian architecture.  Say the bit field looks like this for a little endian architecture:

```
struct __attribute__((__packed__)) {
    int field1: 1;
    int field2: 3;
    int field3: 1;
    int field4: 2;
}
```

The bit fields need to be swapped in order along the byte boundary for big endian:

```
struct __attribute__((__packed__)) {
    int field4: 2;
    int field3: 1;
    int field2: 3;
    int field1: 1;
}
```

I have not had a need to perform cross-byte bit fields in professional development, so I am not covering them here.

## Data Storage

Consistency in conversion is key.  The data needs to be written in a known byte order or have a byte order saved 
within the data structure.  Simplicity here allows us to express consistency across architectures.  My 
recommendation is to save data in one endianness and convert as needed upon reading or writing, but not both.  For 
example, if I want the data to be on disk in little endian format, I must convert the data on a big endian machine 
on both read and write; on a little endian machine, I need to do nothing else.  This is a design decision that must 
be considered when writing multi-architecture software.

## Examples

To prove these issues and their solutions, I wrote `endianness.c`.  I have compiled and run it on an AMD Ryzen 9 AI 
370-based system running Linux.  The big endian hardware I'm using for this project is an Apple iBook G4 running a 
PowerPC 7447a.  I have gcc installed on both systems.  The AMD computer sported 
`gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0` for the tests; the iBook used `powerpc-apple-darwin9-gcc-4.0.1 (GCC)
4.0.1 (Apple Inc. build 5465)`.

### Design

I wanted to show reading and writing to a binary file that contains two fully populated structures independent of 
architecture.  The first structure only uses fundamental data types; the second structure uses composite data types 
and bitfields so I can demonstrate the issues related to bitfield ordering.   I made the design decision to write the 
data in little endian format as most modern-day architectures are little endian.  As technology advances, the 
prevalence of little endian architectures is expected to continue, making this decision relevant for reducing excess 
computation.

I wanted to show the effect of the `__attribute__((__packed__))` attribute on the size of the structure, though this
doesn't particularly contribute to the issue of endianness except for padding as I've discussed in the "Byte 
Swapping in Data Structures" section.

Next, I wanted to show the effect of the bitfield ordering on the data.  I had professional experience in bit 
operations across architectures and therefore wanted to show how endianness changes the location of bits for masking.

I also wanted to show the byte location of the data in an integer related to the most significant byte.

Finally, I needed to actually write the data to a binary file.  Although this was actually the first thing for this 
code, the other tests were necessary to prove the location of the data is different for each architecture.

### Compiling and Executing

I have a simple script to compile and run the program.  I wrote the tests on the AMD system, so my high-level idea for
running the program is:

```
# Compile on AMD.
gcc -Wall -Werror -o endianness endianness.c

# Writes data to little endian file.
./endianness -w little_endian_output.bin > little_endian_stdout.txt 2> little_endian_stderr.txt
./endianness -r little_endian_output.bin > little_endian_read_little_endian.txt 2>> little_endian_stderr.txt
./endianness -r big_endian_output.bin > little_endian_read_big_endian.txt 2>> little_endian_stderr.txt

# Copies program to iBook.
scp endianness.c little_endian_output.bin ibook.local:Projects/endianness

# Compile and run on iBook.  build.sh is a simple script to compile and run the program on data from both architectures.
ssh ibook.local "cd Projects/endianness; build.sh"

# Copy output from iBook to AMD for comparison.
scp ibook.local:Projects/endianness/<big endian output and error files> .

# Run on AMD again on the big endian output binary.

# Compare output from iBook and AMD....
``` 

The comparison of the output from the four runs should be nearly identical except for locations in the code in which 
byte order output is explicitly output, such as the output of the endianness of the process architecture and the 
tests against a specific bit.

### Big Endian Output

The output here was run using the command `./endianness -w big_endian_output.bin` from data written by the iBook's
program, truncated to the second output of the BinaryData2 output.

```
Calculated architecture is Big Endian.
Architecture defined in headers is Big Endian.

The effect of packing on structure size:
  sizeof (struct { char c; int i; }):  8
  sizeof (struct __attribute__((__packed__)) { char c; int i; }):  5
  sizeof (struct { char c; pad1[3]; int i; }):  8

Size of BinaryData1:  19
Size of BinaryData2:  9

data.union_val.bitfields_val: 33554432 (0x02000000)
data.union_val.bitfields_val: 00000010000000000000000000000000
Testing bitfield_val2 from the union_val using 0x02000000 as a mask:   True

data.union_val.bitfields_val: 0 (0x00000000)

Wrote struct BinaryData1:
  char: A
  short: 14321
  int: 12345
  float: 12.34
  double: 123.456

Wrote struct BinaryData2:
  byte_val1: B
  short_val: 1
  byte_val2: X
  byte_val3: Y
  union_val:
    bitfields:
       val1: 0
       val2: 0
       val3: 00000 0
       val4: 0
    int_val (8 bits): 00000000 0

Wrote struct BinaryData2:
  byte_val1: B
  short_val: 2
  byte_val2: X
  byte_val3: Y
  union_val:
    bitfields:
       val1: 1
       val2: 0
       val3: 00000 0
       val4: 0
    int_val (8 bits): 00000001 1
```

### Little Endian Output

The output here was run on the AMD system and truncated to match the big endian output.

```
Calculated architecture is Little Endian.
Architecture defined in headers is Little Endian.

The effect of packing on structure size:
  sizeof (struct { char c; int i; }):  8
  sizeof (struct __attribute__((__packed__)) { char c; int i; }):  5
  sizeof (struct { char c; pad1[3]; int i; }):  8

Size of BinaryData1:  19
Size of BinaryData2:  9

data.union_val.bitfields_val: 2 (0x00000002)
data.union_val.bitfields_val: 00000000000000000000000000000010
Testing bitfield_val2 from the union_val using 0x00000002 as a mask:   True

data.union_val.bitfields_val: 0 (0x00000000)

Wrote struct BinaryData1:
  char: A
  short: 14321
  int: 12345
  float: 12.34
  double: 123.456

Wrote struct BinaryData2:
  byte_val1: B
  short_val: 1
  byte_val2: X
  byte_val3: Y
  union_val:
    bitfields:
       val1: 0
       val2: 0
       val3: 00000 0
       val4: 0
    int_val (8 bits): 00000000 0

Wrote struct BinaryData2:
  byte_val1: B
  short_val: 2
  byte_val2: X
  byte_val3: Y
  union_val:
    bitfields:
       val1: 1
       val2: 0
       val3: 00000 0
       val4: 0
    int_val (8 bits): 00000001 1
```

### Written Data Differences

I used the command `cmp -l big_endian_output.bin little_endian_output.bin` to compare the two files.  As expected, 
there was no output and the return code was 0, meaning the files were identical.

### Reading the Data

There were a total of six runs of the code -- one of writing the data per architecture, and two of reading the data 
from each architecture on each program.  The output from the reads are compared to each other; none were different.  
This proves that the data is read correctly regardless of the architecture.
