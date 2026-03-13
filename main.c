#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>


#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

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
#endif // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__


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


#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
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
#endif // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__


// Test data structure having integral data types and a bitfield.
struct __attribute__((__packed__)) BinaryData2 {
    char char_val;
    short short_val;

    union {
        int int_val;

        struct {
            char char_val1;
            char char_val2;
            unsigned int val1: 1;
            unsigned int val2: 1;
            unsigned int val3: 5;
            unsigned int val4: 1;
            char char_val3;
        } bitfield_val;
    } union_val;
};


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
    printf("  char: %c\n", data->char_val);
    printf("  short: %hd\n", data->short_val);
    printf("  int_val (bitfield_val as int): %d\n", data->union_val.int_val);
    printf("  bitfield_val:\n");
    printf("    char_val1: %c\n", data->union_val.bitfield_val.char_val1);
    printf("    char_val2: %c\n", data->union_val.bitfield_val.char_val2);
    printf("    val1 (bit): %u\n", data->union_val.bitfield_val.val1);
    printf("    val2 (bit): %u\n", data->union_val.bitfield_val.val2);
    printf("    val3 (3 bits): %u\n", data->union_val.bitfield_val.val3);
    printf("    val4 (bit): %u\n", data->union_val.bitfield_val.val4);
    printf("    char_val3: %c\n", data->union_val.bitfield_val.char_val3);
}


#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
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
    union {
        int int_val;

        struct __attribute__((__packed__)) {
            char char_val1;
            char char_val2;
            unsigned int val4: 1;
            unsigned int val3: 5;
            unsigned int val2: 1;
            unsigned int val1: 1;
            char char_val3;
        } bitfield_val;
    } union_val;

    data->short_val = swap_byte_order_short(data->short_val);

    // Swap the 4-byte union while also reversing the bitfields.
    union_val.bitfield_val.char_val1 = data->union_val.bitfield_val.char_val1;
    union_val.bitfield_val.char_val2 = data->union_val.bitfield_val.char_val2;
    union_val.bitfield_val.val4 = data->union_val.bitfield_val.val4;
    union_val.bitfield_val.val3 = data->union_val.bitfield_val.val3;
    union_val.bitfield_val.val2 = data->union_val.bitfield_val.val2;
    union_val.bitfield_val.val1 = data->union_val.bitfield_val.val1;
    union_val.bitfield_val.char_val3 = data->union_val.bitfield_val.char_val3;

    // Bytes are swapped; reassign to data->union_val.
    data->union_val.int_val = union_val.int_val;
}
#endif // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__


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
        perror("Error opening file for reading");
        return 1;
    }

    struct BinaryData1 data1;
    if (fread(&data1, sizeof(struct BinaryData1), 1, f) != 1) {
        perror("Error reading struct BinaryData1");
        return 1;
    }

    struct BinaryData2 data2;
    if (fread(&data2, sizeof(struct BinaryData2), 1, f) != 1) {
        perror("Error reading struct BinaryData2");
        return 1;
    }

    fclose(f);

    printf("Binary data for struct BinaryData1 read successfully:\n");
    print_binary_data_1(&data1);
    printf("\n");
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    swap_byte_order_for_binary_data_1(&data1);
    printf("Swapped Binary data for struct BinaryData1:\n");
    print_binary_data_2(&data2);
    printf("\n");
#endif  // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

    printf("Binary data for struct BinaryData2 read successfully:\n");
    print_binary_data_2(&data2);
    printf("\n");
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    swap_byte_order_for_binary_data_2(&data2);
    printf("Swapped Binary data for struct BinaryData2:\n");
    print_binary_data_2(&data2);
    printf("\n");
#endif  // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

    return 0;
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
        perror("Error opening file");
        return 1;
    }

    struct BinaryData1 data1 = {
        .char_val = 'A',
        .short_val = 14321,
        .int_val = 12345,
        .float_val = 12.34f,
        .double_val = 123.456
    };

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    swap_byte_order_for_binary_data_1(&data1);
#endif  // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

    if (fwrite(&data1, sizeof(struct BinaryData1), 1, f) != 1) {
        perror("Error writing struct BinaryData1");
        fclose(f);
        return 1;
    }

    struct BinaryData2 data2 = {
        .char_val = 'B',
        .short_val = 5678,
        .union_val = {
            .bitfield_val = {
                .char_val1 = 'X',
                .char_val2 = 'Y',
                .val1 = 0,
                .val2 = 1,
                .val3 = 16,
                .val4 = 1,
                .char_val3 = 'Z'
            }
        }
    };

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    swap_byte_order_for_binary_data_2(&data2);
#endif  // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

    if (fwrite(&data2, sizeof(struct BinaryData2), 1, f) != 1) {
        perror("Error writing struct BinaryData2");
        fclose(f);
        return 1;
    }

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
    printf("\n");

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    printf("Headers architecture is Little Endian.\n");
#else
    printf("Headers architecture is Big Endian.\n");
#endif
    printf("\n");

    printf("Size of BinaryData1:  %lu\n", sizeof(struct BinaryData1));
    printf("Size of BinaryData2:  %lu\n", sizeof(struct BinaryData2));
    printf("\n");

    return handle_options(argc, argv);
}
