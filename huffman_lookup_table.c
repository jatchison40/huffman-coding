#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define size of lookup table
#define CHARACTER_COUNT 128
#define LOOKUP_TABLE_SIZE 4096
#define LOOKUP_TABLE_INDEX_SIZE 12

// Node struct
struct TreeNode
{
    signed char data;
    int freq;

    struct TreeNode *left, *right, *child1, *child2;
};

// function declarations
struct TreeNode *create_node(signed char data, int freq);
struct TreeNode *create_parent_node(int freq1, int freq2);
void insert_node(signed char data, int freq);
void insert_parent_node(int data, int freq);
void create_node_linked_list(signed char character_array[], int character_frequency[]);
void create_huffman_tree();
void detach_node(struct TreeNode *node);
int is_leaf_node(struct TreeNode *node);
void extract_encode_bit_combination(struct TreeNode *node, signed char array[], int array_index);
void build_lookup_table();
int binary_to_int(signed char s[], int length);
int read_buffer_bits(int length, int offset, FILE *input_file);
void decode_with_lookup_table(char *input_filename, char *output_filename);
void encode_input_text(char *input_filename, char *output_filename);

// global variables
struct TreeNode *head = NULL;
struct TreeNode *tail = NULL;
signed char encode_array[CHARACTER_COUNT][CHARACTER_COUNT];
signed char encode_array_length[CHARACTER_COUNT];
signed char decode_lookup[LOOKUP_TABLE_SIZE][2];
int list_size = 0;

struct TreeNode *create_node(signed char data, int freq)
{
    struct TreeNode *temp = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    temp->left = temp->right = temp->child1 = temp->child2 = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

struct TreeNode *create_parent_node(int freq1, int freq2)
{
    struct TreeNode *temp = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    temp->left = temp->right = temp->child1 = temp->child2 = NULL;
    temp->data = -1;
    temp->freq = freq1 + freq2;
    return temp;
}

void insert_node(signed char data, int freq)
{
    if (head == NULL)
    {
        head = tail = create_node(data, freq);
    }
    else
    {
        tail->right = create_node(data, freq);
        tail->right->left = tail;
        tail = tail->right;
    }
    list_size++;
}

void insert_parent_node(int data, int freq)
{
    if (head == NULL)
    {
        head = tail = create_parent_node(data, freq);
    }
    else
    {
        tail->right = create_parent_node(data, freq);
        tail->right->left = tail;
        tail = tail->right;
    }
    list_size++;
}

void create_node_linked_list(signed char character_array[], int character_frequency[])
{

    int i;
    for (i = 0; i < CHARACTER_COUNT; i++)
    {
        if (character_frequency[i] > 0)
        {
            insert_node(character_array[i], character_frequency[i]);
        }
    }
}

void create_huffman_tree()
{
    struct TreeNode *list_pointer = head;
    struct TreeNode *smallest_pointer = head;
    struct TreeNode *second_smallest_pointer = NULL;

    if (list_size == 1 && is_leaf_node(list_pointer))
    {
        insert_parent_node(smallest_pointer->freq, 0);
        tail->child1 = smallest_pointer;
        detach_node(smallest_pointer);
    }

    while (list_size > 1)
    {
        // find 2 smallest nodes
        while (list_pointer != NULL)
        {
            if (list_pointer != smallest_pointer)
            {
                if (list_pointer->freq < smallest_pointer->freq)
                {
                    second_smallest_pointer = smallest_pointer;
                    smallest_pointer = list_pointer;
                }
                else if ((second_smallest_pointer == NULL) || (list_pointer->freq >= smallest_pointer->freq && list_pointer->freq < second_smallest_pointer->freq))
                {
                    second_smallest_pointer = list_pointer;
                }
            }
            list_pointer = list_pointer->right;
        }
        insert_parent_node(smallest_pointer->freq, second_smallest_pointer->freq);
        tail->child1 = smallest_pointer;
        tail->child2 = second_smallest_pointer;
        detach_node(smallest_pointer);
        detach_node(second_smallest_pointer);
        list_pointer = smallest_pointer = head;
        second_smallest_pointer = NULL;
    }
}

void detach_node(struct TreeNode *node)
{
    if (list_size == 1)
    {
        return;
    }
    if (node == head)
    {
        head = node->right;
        node->right->left = NULL;
        node->right = NULL;
    }
    else if (node == tail)
    {
        tail = node->left;
        node->left->right = NULL;
        node->left = NULL;
    }
    else
    {
        node->left->right = node->right;
        node->right->left = node->left;
        node->left = NULL;
        node->right = NULL;
    }
    list_size--;
}

int is_leaf_node(struct TreeNode *node)
{
    return node->data != -1;
}

void extract_encode_bit_combination(struct TreeNode *node, signed char array[], int array_index)
{
    if (node != NULL)
    {
        if (is_leaf_node(node))
        {
            array[array_index] = '\0';
            int i;
            for (i = 0; i <= array_index; i++)
            {
                encode_array[node->data][i] = array[i];
            }
            encode_array_length[node->data] = array_index;
            return;
        }
        if (node->child1 != NULL)
        {
            array[array_index] = 0;
            extract_encode_bit_combination(node->child1, array, array_index + 1);
        }
        if (node->child2 != NULL)
        {
            array[array_index] = 1;
            extract_encode_bit_combination(node->child2, array, array_index + 1);
        }
    }
    return;
}

void build_lookup_table()
{
    struct TreeNode *p = head;
    int index;
    for (index = 0; index < LOOKUP_TABLE_SIZE; index++)
    {
        int length = 0;
        int leaf_found = 0;

        // loops through all lookup table indexes and traverses tree following the bits of the index
        int bit_index;
        for (bit_index = LOOKUP_TABLE_INDEX_SIZE; bit_index > 0; bit_index--)
        {
            int digit = 1 << (bit_index - 1) & index;
            if (digit == 0)
            {
                p = p->child1;
            }
            else
            {
                p = p->child2;
            }
            length++;

            // If a leaf node is found, record the character and traversal length in the lookup table
            if (is_leaf_node(p))
            {
                decode_lookup[index][0] = p->data;
                decode_lookup[index][1] = length;

                p = head;
                leaf_found = 1;

                break;
            }
        }
        if (!leaf_found)
        {
            // This condition is entered if there were bit codes longer that our lookup table length
        }
    }
}

int binary_to_int(signed char s[], int length)
{
    // convert binary string to decimal representation so that we can index lookup table
    int value = 0;
    int bitValue = 1;
    int i;
    for (i = length - 1; i >= 0; i--)
    {
        if (s[i] == '1')
            value += bitValue;

        bitValue <<= 1;
    }
    return value;
}

int read_buffer_bits(int length, int offset, FILE *input_file)
{
    // create char array of proper length to hold bits
    signed char binary_array[length];

    int read_bits = 0;
    int first_byte_offset = offset;

    // read first applicable byte from file and shift it by the bit offset
    signed char inputbyte = fgetc(input_file);
    inputbyte = inputbyte << offset;

    // Since we need to read LOOKUP_TABLE_INDEX_SIZE bits, might need to read multiple bytes
    while (read_bits < length)
    {
        int bit_index;
        for (bit_index = 7; bit_index >= first_byte_offset; bit_index--)
        {
            // reads bit by bit from byte and fills binary_array until binary_array is full
            int digit = 1 << (bit_index)&inputbyte;
            if (digit == 0)
            {
                binary_array[read_bits] = '0';
            }
            else
            {
                binary_array[read_bits] = '1';
            }
            read_bits++;

            if (read_bits == length)
            {
                break;
            }
        }
        first_byte_offset = 0;
        inputbyte = fgetc(input_file);
    }

    // Convert the binary_array (char array) to equivalent integer representation
    int number = binary_to_int(binary_array, length);

    // At the end of file if there were not enough remaining bits to fill binary_array, shift number by unfilled space
    if (length < LOOKUP_TABLE_INDEX_SIZE)
    {
        number = number << (LOOKUP_TABLE_INDEX_SIZE - length);
    }
    return number;
}

void decode_with_lookup_table(char *input_filename, char *output_filename)
{
    FILE *input_file = fopen(input_filename, "rb");  // an encoded file
    FILE *output_file = fopen(output_filename, "w"); // Where we print decoded bytes

    if (input_file == NULL || output_file == NULL)
    {
        printf("Error: Filename could not be opened\n");
        exit(4);
    }

    // read in total bits encoded from first 4 bytes
    long total_bits = 0;
    total_bits = total_bits | fgetc(input_file) << 24;
    total_bits = total_bits | fgetc(input_file) << 16;
    total_bits = total_bits | fgetc(input_file) << 8;
    total_bits = total_bits | fgetc(input_file);

    register int processed_bits = 0;
    int offset;
    int current_byte;
    int ending_bits = -1;
    int length = LOOKUP_TABLE_INDEX_SIZE;
    int lookup_index;

    while (1)
    {
        // seek to correct byte
        offset = processed_bits % 8;
        current_byte = processed_bits / 8;
        fseek(input_file, current_byte + 4, SEEK_SET);

        // Check if we are at end of file and adust length accordingly
        if (processed_bits + LOOKUP_TABLE_INDEX_SIZE >= total_bits)
        {
            length = LOOKUP_TABLE_INDEX_SIZE - (processed_bits + LOOKUP_TABLE_INDEX_SIZE - total_bits);
        }

        // Get lookup index
        lookup_index = read_buffer_bits(length, offset, input_file);

        // output decoded char to file and update processed bit count
        // processed_bits = processed_bits + decode_lookup[lookup_index][1];
        processed_bits = processed_bits + decode_lookup[lookup_index][1];
        fputc(decode_lookup[lookup_index][0], output_file);

        if (processed_bits == total_bits)
        {
            return;
        }
    }
}

void encode_input_text(char *input_filename, char *output_filename)
{
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");
    if (input_file == NULL || output_file == NULL)
    {
        printf("Error: Filename could not be opened\n");
        exit(3);
    }

    // save 4 bytes at the beginning to go back and fill in number of bits and end
    fputc(0, output_file);
    fputc(0, output_file);
    fputc(0, output_file);
    fputc(0, output_file);

    signed char input_c = fgetc(input_file);
    signed char output_byte_buffer = 0;
    int bitcount = 0;
    int total_bits = 0;
    while (input_c != EOF)
    {
        // this translates code contained in encode_array to its binary equivalent, then outputs the bits as a char every 8 bits
        int i;
        for (i = 0; i < encode_array_length[input_c]; i++)
        {
            if (bitcount == 8)
            {
                fputc(output_byte_buffer, output_file);
                total_bits = total_bits + 8;
                output_byte_buffer = 0;
                bitcount = 0;
            }

            output_byte_buffer = output_byte_buffer << 1 | encode_array[input_c][i];
            bitcount++;
        }

        input_c = fgetc(input_file);
    }
    int padding_bits_count = 0;
    if (bitcount != 0)
    {
        padding_bits_count = 8 - bitcount;
        total_bits = total_bits + bitcount;
        fputc(0, output_file);
        fseek(output_file, -1, SEEK_END);
        fputc(output_byte_buffer << padding_bits_count, output_file);
    }

    // This adds the total number of bits encoded to the first 4 bytes of the encoded file
    fseek(output_file, 0, SEEK_SET);
    fputc((signed char)(total_bits >> 24 & 0xFF), output_file);
    fputc((signed char)(total_bits >> 16 & 0xFF), output_file);
    fputc((signed char)(total_bits >> 8 & 0xFF), output_file);
    fputc((signed char)(total_bits & 0xFF), output_file);

    fclose(input_file);
    fclose(output_file);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("ERROR! Incorrect number of arguments. Try: ./huffman <input.txt> <output.txt>\n");
        exit(1);
    }

    clock_t start_t, end_t;
    long total_t;

    // Reads alphabet and frequencies
    signed char character_array[CHARACTER_COUNT];
    int character_frequency[CHARACTER_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 170, 7821, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17691, 240, 213, 283, 150, 200, 222, 254, 842, 905, 350, 601, 6741, 2660, 6265, 442, 713, 1240, 1380, 890, 610, 633, 348, 250, 249, 306, 775, 220, 250, 1603, 226, 1426, 350, 2877, 991, 803, 952, 652, 461, 486, 2146, 4308, 235, 342, 842, 605, 850, 706, 463, 250, 516, 1829, 2844, 300, 300, 1140, 1577, 1223, 566, 220, 220, 195, 182, 245, 280, 57205, 7806, 7620, 10686, 40645, 7903, 8699, 17764, 17418, 1202, 8370, 17745, 17516, 18484, 15880, 12287, 890, 37464, 16860, 15368, 19214, 6842, 15965, 1102, 16208, 442, 235, 233, 235, 0, 0};
    signed char temp_array[CHARACTER_COUNT];

    int i;
    for (i = 0; i < CHARACTER_COUNT; i++)
    {
        character_array[i] = i;
    }

    // Created doubly linked list with all chars in alphabet as nodes
    create_node_linked_list(character_array, character_frequency);

    // build huffman tree based on frequency of char
    create_huffman_tree();

    // Traverses tree and generates bit codes for each char
    extract_encode_bit_combination(head, temp_array, 0);

    // encodes file
    encode_input_text(argv[1], argv[2]);

    // build lookup table for decoding
    build_lookup_table();

    // decodes using lookup table
    start_t = clock();
    decode_with_lookup_table(argv[2], "decoded.txt");
    end_t = clock();

    total_t = (end_t - start_t);
    printf("Total time taken by CPU: %ld\n", total_t);
    return 0;
}
