#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#if BYTE_ORDER == BIG_ENDIAN

/* swap_byte_order_short
 *
 * Description:  Swaps the high and low bytes of a short.
 *
 * Input:
 *     s:  The short value whose components should be swapped.
 *
 * Returns:
 *     The short value with high and low bytes swapped.
 */
static short swap_byte_order_short(const short s) {
    const uint16_t us = (uint16_t) s;
    return (short) ((us << 8) | (us >> 8));
}


/* swap_byte_order_int
 *
 * Description:  Swaps the bytes of an int from highest to lowest.
 *
 * Input:
 *     i:  The int value whose components should be swapped.
 *
 * Returns:
 *     The int value with the bytes swapped.
 */
static int swap_byte_order_int(const int i) {
    const uint32_t ui = (uint32_t) i;
    return (int) ((ui << 24) | ((ui << 8) & 0x00FF0000) | ((ui >> 8) & 0x0000FF00) | (ui >> 24));
}


/* swap_byte_order_float
 *
 * Description:  Swaps the bytes of a float from highest to lowest.
 *
 * Input:
 *     f:  The float value whose components should be swapped.
 *
 * Returns:
 *     The float value with the bytes swapped.
 */
static float swap_byte_order_float(const float f) {
    union {
        float f;
        int i;
    } u;
    u.f = f;
    u.i = swap_byte_order_int(u.i);
    return u.f;
}


/* swap_byte_order_double
 *
 * Description:  Swaps the bytes of a double from highest to lowest.
 *
 * Input:
 *     d:  The double value whose components should be swapped.
 *
 * Returns:
 *     The double value with the bytes swapped.
 */
static double swap_byte_order_double(const double d) {
    union {
        double d;
        uint64_t u;
    } val;
    val.d = d;
    uint64_t u = val.u;
    u = ((u << 56) & 0xFF00000000000000ULL) |
        ((u << 40) & 0x00FF000000000000ULL) |
        ((u << 24) & 0x0000FF0000000000ULL) |
        ((u << 8) & 0x000000FF00000000ULL) |
        ((u >> 8) & 0x00000000FF000000ULL) |
        ((u >> 24) & 0x0000000000FF0000ULL) |
        ((u >> 40) & 0x000000000000FF00ULL) |
        ((u >> 56) & 0x00000000000000FFULL);
    val.u = u;
    return val.d;
}
#endif // BYTE_ORDER == BIG_ENDIAN


// Test data structure having integral data types.
struct __attribute__((__packed__)) BinaryData1 {
    char char_val;
    short short_val;
    int int_val;
    float float_val;
    double double_val;
};


/* print_binary_data_1
 *
 * Description:  Prints the contents of a BinaryData1 structure.
 *
 * Input:
 *     data:  Pointer to the BinaryData1 structure to be printed.
 *
 * Returns:
 *     None.
 */
static void print_binary_data_1(const struct BinaryData1 *data) {
    printf("  char: %c\n", data->char_val);
    printf("  short: %hd\n", data->short_val);
    printf("  int: %d\n", data->int_val);
    printf("  float: %.2f\n", data->float_val);
    printf("  double: %.3f\n", data->double_val);
}


#if BYTE_ORDER == BIG_ENDIAN
/* swap_byte_order_for_binary_data_1
 *
 * Description:  Swaps the byte order of the components of a BinaryData1 structure.
 *
 * Input:
 *     data:  Pointer to the BinaryData1 structure whose components should be swapped.
 *
 * Returns:
 *     None.
 */
static void swap_byte_order_for_binary_data_1(struct BinaryData1 *data) {
    // No need to swap data->char_val.
    data->short_val = swap_byte_order_short(data->short_val);
    data->int_val = swap_byte_order_int(data->int_val);
    data->float_val = swap_byte_order_float(data->float_val);
    data->double_val = swap_byte_order_double(data->double_val);
}
#endif // BYTE_ORDER == BIG_ENDIAN


// Test data structure having integral data types and a bitfield.
struct __attribute__((__packed__)) BinaryData2 {
    char byte_val1;
    short short_val;
    char byte_val2;
    char byte_val3;
    union {
        int bitfields_val;
        struct {
#if BYTE_ORDER == LITTLE_ENDIAN
            unsigned int val1: 1;
            unsigned int val2: 1;
            unsigned int val3: 5;
            unsigned int val4: 1;
#else
            unsigned int val4: 1;
            unsigned int val3: 5;
            unsigned int val2: 1;
            unsigned int val1: 1;
#endif // BYTE_ORDER == LITTLE_ENDIAN
        } bitfields;
    } union_val;
};


/* bits
 *
 * Description:  Converts an unsigned integer to its binary representation as a string.
 *
 * Inputs:
 *     val:  The unsigned integer to convert to binary.
 *     num_bits:  The number of bits to represent in the binary string or zero to free the buffer.
 *
 * Returns:
 *     A pointer to a string containing the binary representation of the input integer or NULL if the number of bits is
 *     zero or the allocation fails.
 */
static char *bits(const unsigned int val, const unsigned int num_bits) {
    static char *bits_str = NULL;
    unsigned int i;

    if (num_bits == 0) {
        free (bits_str);
        bits_str = NULL;
        return NULL;
    }

    char *temp_bits_str = (char *) realloc (bits_str, num_bits+1);
    if (temp_bits_str == NULL) {
        fprintf(stderr, "Error allocating memory for bits string.\n");
        return NULL;
    }

    bits_str = temp_bits_str;

    for (i = 0; i < num_bits; i++) {
        bits_str[num_bits - i - 1] = (val & (1 << i)) ? '1' : '0';
    }
    bits_str[num_bits] = '\0';

    return bits_str;
}


/* print_binary_data_2
 *
 * Description:  Prints the contents of a BinaryData2 structure.
 *
 * Input:
 *     data:  Pointer to the BinaryData2 structure to be printed.
 *
 * Returns:
 *     None.
 */
static void print_binary_data_2(const struct BinaryData2 *data) {
    printf("  byte_val1: %c\n", data->byte_val1);
    printf("  short_val: %hd\n", data->short_val);
    printf("  byte_val2: %c\n", data->byte_val2);
    printf("  byte_val3: %c\n", data->byte_val3);
    // printf("  int_val (bitfield_val as int): %d\n", data->union_val.int_val);
    printf("  union_val:\n");
    printf("    bitfields:\n");
    printf("       val1: %u\n", data->union_val.bitfields.val1);
    printf("       val2: %u\n", data->union_val.bitfields.val2);
#if BYTE_ORDER == LITTLE_ENDIAN
    printf("       val3: %s %u\n", bits (data->union_val.bitfields.val3, 5),
        data->union_val.bitfields.val3 & 0xFF);
#else
    printf("       val3: %s %u\n", bits (data->union_val.bitfields.val3 & 0xFF, 5),
        data->union_val.bitfields.val3 & 0xFF);
#endif // BYTE_ORDER == LITTLE_ENDIAN
    printf("       val4: %u\n", data->union_val.bitfields.val4);
#if BYTE_ORDER == LITTLE_ENDIAN
    printf("    int_val (8 bits): %s %u\n", bits (data->union_val.bitfields_val, 8),
        data->union_val.bitfields_val & 0xFF);
#else
    printf("    int_val (8 bits): %s %u\n", bits (data->union_val.bitfields_val >> 24, 8),
        data->union_val.bitfields_val >> 24);
#endif // BYTE_ORDER == LITTLE_ENDIAN
}


#if BYTE_ORDER == BIG_ENDIAN
/* swap_byte_order_for_binary_data_2
 *
 * Description:  Swaps the byte order of the components of a BinaryData2 structure.
 *
 * Input:
 *     data:  Pointer to the BinaryData2 structure whose components should be swapped.
 *
 * Returns:
 *     None.
 */
static void swap_byte_order_for_binary_data_2(struct BinaryData2 *data) {
    data->short_val = swap_byte_order_short(data->short_val);
}
#endif // BYTE_ORDER == BIG_ENDIAN


/* test_bitfield_val2:  Test the bitfield_val2 field of the BinaryData2 struct.
 *
 * This demonstrates the significance of byte ordering when using bit masks.  There is only textual output.
 */
static void test_bitfield_val2(void) {
    struct BinaryData2 data = {
        .byte_val1 = '\0',
        .short_val = 0,
        .byte_val2 = '\0',
        .byte_val3 = '\0',
        .union_val.bitfields_val = 0
    };
    data.union_val.bitfields.val2 = 1;

#if BYTE_ORDER == LITTLE_ENDIAN
#define BITFIELDS_VAL2_MASK 0x00000002
#else
#define BITFIELDS_VAL2_MASK 0x02000000
#endif // BYTE_ORDER == LITTLE_ENDIAN

    printf ("data.union_val.bitfields_val: %d (0x%08x)\n", data.union_val.bitfields_val,
        data.union_val.bitfields_val);
    printf ("data.union_val.bitfields_val: %s\n", bits (data.union_val.bitfields_val,
        sizeof (data.union_val.bitfields_val)*8));
    printf ("Testing bitfield_val2 from the union_val using 0x%08x as a mask:   %s\n",
        BITFIELDS_VAL2_MASK,
        data.union_val.bitfields_val & BITFIELDS_VAL2_MASK ? "True" : "False");
    printf ("\n");
}


/* print_msb_of_int
 *
 * Description:  Prints the most-significant byte using a BinaryData2 structure.  There is only textual output.
 */
static void print_msb_of_int(void) {
    const struct BinaryData2 data = {
        .byte_val1 = '\0',
        .short_val = 0,
        .byte_val2 = '\0',
        .byte_val3 = '\0',
        .union_val.bitfields = {
            .val1 = 0,
            .val2 = 0,
            .val3 = 0,
            .val4 = 0,
        }
    };

    printf ("data.union_val.bitfields_val: %d (0x%08x)\n",
        data.union_val.bitfields_val, data.union_val.bitfields_val);
    printf ("\n");
}


/* read_binary_data:  Read binary data from a file and swap byte order if necessary.
 *
 * Since most architectures are little-endian, this function swaps byte order for big-endian systems.
 * This is a bit unconventional as network byte order is big-endian, but again, most systems are little-endian.
 *
 * Parameters:
 *     filename: The name of the file to read.
 *
 * Returns:
 *     0 on success, 1 on failure.
 */
static int read_binary_data(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "Error opening file \"%s\" for reading.\n", filename);
        return 1;
    }

    struct BinaryData1 data1;
    if (fread(&data1, sizeof(struct BinaryData1), 1, f) != 1) {
        fprintf(stderr, "Error reading struct BinaryData1.\n");
        fclose(f);
        return 1;
    }

#if BYTE_ORDER == BIG_ENDIAN
    swap_byte_order_for_binary_data_1(&data1);
#endif  // BYTE_ORDER == BIG_ENDIAN

    printf("Binary data for struct BinaryData1 read successfully:\n");
    print_binary_data_1(&data1);
    printf("\n");

    // Read any number of BinaryData2 structures remaining in the file.
    do {
        struct BinaryData2 data2;

        if ((!feof (f)) && (fread(&data2, sizeof(struct BinaryData2), 1, f) != 1)) {
            fprintf(stderr, "Error reading struct BinaryData2.\n");
            fclose (f);
            return 1;
        }

#if BYTE_ORDER == BIG_ENDIAN
        swap_byte_order_for_binary_data_2(&data2);
#endif  // BYTE_ORDER == BIG_ENDIAN

        printf("Binary data for struct BinaryData2 read successfully:\n");
        print_binary_data_2(&data2);
        printf("\n");
    } while (!feof(f));

    fclose(f);

    return 0;
}

static void populate_and_write_binary_data_2 (FILE *f, const int val1, const int val2, const int val3, const int val4) {
    static short count = 0;

    struct BinaryData2 data2 = {
        .byte_val1 = 'B',
        .short_val = ++count,
        .byte_val2 = 'X',
        .byte_val3 = 'Y',
        .union_val.bitfields = {
            .val1 = val1,
            .val2 = val2,
            .val3 = val3,
            .val4 = val4,
        }
    };

    printf("Wrote struct BinaryData2:\n");
    print_binary_data_2(&data2);
    printf("\n");

#if BYTE_ORDER == BIG_ENDIAN
    swap_byte_order_for_binary_data_2(&data2);
#endif  // BYTE_ORDER == BIG_ENDIAN

    fwrite(&data2, sizeof(struct BinaryData2), 1, f);
}


/* write_test_binary_data:  Writes predefined binary data to a file and swap byte order if necessary.
 *
 * Since most architectures are little-endian, this function swaps byte order for big-endian systems.
 * This is a bit unconventional as network byte order is big-endian, but again, most systems are little-endian.
 *
 * Parameters:
 *     filename:  The name of the file to write.
 *
 * Returns:
 *     0 on success, 1 on failure.
 */
static int write_test_binary_data(const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "Error opening file \"%s\".\n", filename);
        return 1;
    }

    struct BinaryData1 data1 = {
        .char_val = 'A',
        .short_val = 14321,
        .int_val = 12345,
        .float_val = 12.34f,
        .double_val = 123.456
    };

    printf("Wrote struct BinaryData1:\n");
    print_binary_data_1(&data1);
    printf("\n");

#if BYTE_ORDER == BIG_ENDIAN
    swap_byte_order_for_binary_data_1(&data1);
#endif  // BYTE_ORDER == BIG_ENDIAN

    if (fwrite(&data1, sizeof(struct BinaryData1), 1, f) != 1) {
        fprintf(stderr, "Error writing struct BinaryData1.\n");
        fclose(f);
        return 1;
    }

    populate_and_write_binary_data_2(f, 0, 0, 0, 0);
    populate_and_write_binary_data_2(f, 1, 0, 0, 0);
    populate_and_write_binary_data_2(f, 0, 1, 0, 0);
    populate_and_write_binary_data_2(f, 0, 0, 1 << 0, 0);
    populate_and_write_binary_data_2(f, 0, 0, 1 << 1, 0);
    populate_and_write_binary_data_2(f, 0, 0, 1 << 2, 0);
    populate_and_write_binary_data_2(f, 0, 0, 1 << 3, 0);
    populate_and_write_binary_data_2(f, 0, 0, 1 << 4, 0);
    populate_and_write_binary_data_2(f, 0, 0, 0, 1);

    fclose(f);

    printf("Binary file '%s' created with BinaryData1 and BinaryData2 content.\n", filename);

    return 0;
}


/* handle_options:  Parses command-line options, setting flags and extracting a filename.  Finally, calls
 *     read_binary_data or write_test_binary_data based on the provided flags and filename.
 *
 * Parameters:
 *     argc:  The number of command-line arguments.
 *     argv:  The array of command-line arguments.
 *
 * Returns:
 *     0 on success, 1 on argument error.
 */
static int handle_options(const int argc, char *argv[]) {
    int opt;
    bool read_flag = false;
    bool write_flag = false;
    const char *filename = NULL;

    while ((opt = getopt(argc, argv, "rw")) != -1) {
        switch (opt) {
            case 'r':
                read_flag = true;
                break;
            case 'w':
                write_flag = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-r] [-w] <filename>\n", argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        filename = argv[optind];
    } else {
        fprintf(stderr, "Error: Filename is a required field.\n");
        fprintf(stderr, "Usage: %s [-r] [-w] <filename>\n", argv[0]);
        return 1;
    }

    if (read_flag && write_flag) {
        fprintf(stderr, "Error: Specify either reading or writing, but not both.\n");
        return 1;
    }

    if (!read_flag && !write_flag) {
        fprintf(stderr, "Error: Specify either -r (read) or -w (write).\n");
        return 1;
    }

    if (write_flag) {
        return write_test_binary_data(filename);
    } else {
        return read_binary_data(filename);
    }
}


int main(const int argc, char *argv[]) {
    uint32_t i = 1;
    const char *c = (char *) &i;
    if (*c) {
        printf("Calculated architecture is Little Endian.\n");
    } else {
        printf("Calculated architecture is Big Endian.\n");
    }
#if BYTE_ORDER == LITTLE_ENDIAN
    printf("Architecture defined in headers is Little Endian.\n");
#else
    printf("Architecture defined in headers is Big Endian.\n");
#endif
    printf("\n");

    printf ("The effect of packing on structure size:\n");
    printf ("  sizeof (struct { char c; int i; }):  %lu\n", sizeof (struct { char c; int i; }));
    printf ("  sizeof (struct __attribute__((__packed__)) { char c; int i; }):  %lu\n",
        sizeof (struct __attribute__((__packed__)) { char c; int i; }));
    printf ("  sizeof (struct { char c; pad1[3]; int i; }):  %lu\n",
        sizeof (struct { char c; char pad1[3]; int i; }));
    printf ("\n");

    printf("Size of BinaryData1:  %lu\n", sizeof(struct BinaryData1));
    printf("Size of BinaryData2:  %lu\n", sizeof(struct BinaryData2));
    printf("\n");

    test_bitfield_val2();
    print_msb_of_int();

    // Frees the static memory in the bits function.
    bits (0, 0);

    return handle_options(argc, argv);
}
