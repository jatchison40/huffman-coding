#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARACTER_COUNT 128
#define LOOKUP_TABLE_SIZE 4096
#define LOOKUP_TABLE_INDEX_SIZE 12

struct TreeNode
{
  char data;
  int freq;

  struct TreeNode *left, *right, *child1, *child2;
};
void detach_node(struct TreeNode *node);
struct TreeNode *create_node(char data, int freq);
struct TreeNode *create_parent_node(int freq1, int freq2);
void insert_node(char data, int freq);
void insert_parent_node(int data, int freq);
void create_node_linked_list(char character_array[], int character_frequency[]);
void print_list();
void print_list_reverse();
void print_node(struct TreeNode *list_pointer);
void create_huffman_tree();
void detach_node(struct TreeNode *node);
int is_leaf_node(struct TreeNode *node);
void extract_encode_bit_combinaion(struct TreeNode *node, char array[], int array_index);

struct TreeNode *head = NULL;
struct TreeNode *tail = NULL;
char encode_array[CHARACTER_COUNT][CHARACTER_COUNT];
char encode_array_length[CHARACTER_COUNT];
char decode_lookup[LOOKUP_TABLE_SIZE][3];

int list_size = 0;

struct TreeNode *create_node(char data, int freq)
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

void insert_node(char data, int freq)
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

void create_node_linked_list(char character_array[], int character_frequency[])
{

  for (int i = 0; i < CHARACTER_COUNT; i++)
  {
    if (character_frequency[i] > 0)
    {
      insert_node(character_array[i], character_frequency[i]);
    }
  }
}

void print_list()
{
  struct TreeNode *list_pointer = head;
  while (list_pointer != NULL)
  {
    printf("|%c, %d| <-> ", list_pointer->data, list_pointer->freq);
    list_pointer = list_pointer->right;
  }
  printf("|NULL| List Size: %d\n", list_size);
}

void print_list_reverse()
{
  struct TreeNode *list_pointer = tail;
  while (list_pointer != NULL)
  {
    printf("|%c, %d| <-> ", list_pointer->data, list_pointer->freq);
    list_pointer = list_pointer->left;
  }
  printf("|NULL| List Size: %d\n", list_size);
}

void print_node(struct TreeNode *list_pointer)
{
  if (list_pointer != NULL)
  {
    printf("|%c, %d|\n", list_pointer->data, list_pointer->freq);
  }
}

void create_huffman_tree()
{
  struct TreeNode *list_pointer = head;
  struct TreeNode *smallest_pointer = head;
  struct TreeNode *second_smallest_pointer = NULL;

  if (list_size == 1 && is_leaf_node(list_pointer))
  {
    insert_parent_node(smallest_pointer->freq, 0); // this doesnt make sense
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
    // printf("Loop done\n");
    insert_parent_node(smallest_pointer->freq, second_smallest_pointer->freq);
    tail->child1 = smallest_pointer;
    tail->child2 = second_smallest_pointer;
    detach_node(smallest_pointer);
    detach_node(second_smallest_pointer);
    list_pointer = smallest_pointer = head;
    second_smallest_pointer = NULL;
    // print_list();
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

void extract_encode_bit_combinaion(struct TreeNode *node, char array[], int array_index)
{
  if (node != NULL)
  {
    if (is_leaf_node(node))
    {
      array[array_index] = '\0';
      for (int i = 0; i <= array_index; i++)
      {
        encode_array[node->data][i] = array[i];
      }
      encode_array_length[node->data] = array_index;
      // printf("Node:%c, Code:", node->data);
      for (int i = 0; i < array_index; i++)
      {
        // printf("%d", array[i]);
      }
      // printf("\n");
      return;
    }
    if (node->child1 != NULL)
    {
      array[array_index] = 0;
      extract_encode_bit_combinaion(node->child1, array, array_index + 1);
    }
    if (node->child2 != NULL)
    {
      array[array_index] = 1;
      extract_encode_bit_combinaion(node->child2, array, array_index + 1);
    }
  }
  return;
}

void build_lookup_table()
{
  struct TreeNode *p = head;
  for (int index = 0; index < LOOKUP_TABLE_SIZE; index++)
  {
    int length = 0;
    int leaf_found = 0;
    for (int bit_index = LOOKUP_TABLE_INDEX_SIZE; bit_index > 0; bit_index--)
    {
      int digit = 1 << (bit_index - 1) & index;
      // printf("%d", digit % 10);
      if (digit == 0)
      {
        p = p->child1;
      }
      else
      {
        p = p->child2;
      }
      length++;
      if (is_leaf_node(p))
      {
        decode_lookup[index][0] = p->data;
        decode_lookup[index][1] = length;

        // printf("char: %c size: %d", p->data, length);
        p = head;
        leaf_found = 1;

        break;
      }
    }
    if (!leaf_found)
    {
      printf("NOT FOUND %d \n", index);
    }

    // printf("\n");
  }
}

int binary_to_int(char s[], int length)
{
  // convert binary string to decimal representation so that we can index lookup table
  int value = 0;
  int bitValue = 1;

  for (int i = length - 1; i >= 0; i--)
  {
    if (s[i] == '1')
      value += bitValue;

    bitValue <<= 1;
  }
  return value;
}

int read_buffer_bits(int length, int offset, FILE *input_file)
// returns lookup table index
{
  char binary_array[length];

  int read_bits = 0;
  char inputbyte = fgetc(input_file);
  if (inputbyte == EOF)
  {
    return -1;
  }
  inputbyte = inputbyte << offset;

  while (read_bits < length && inputbyte != EOF)
  { // continue reading bits until we have read enough

    // printf("%c\n", inputbyte);

    for (int bit_index = 7; bit_index >= (0 + offset); bit_index--)
    {

      int digit = 1 << (bit_index)&inputbyte; // gives the bit at position bit_index in inputbyte
      // printf("%d", digit % 10);
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
        // if we have filled up binary_array
        break;
      }
    }
    inputbyte = fgetc(input_file);
  }
  for (int i = 0; i < length; i++)
  {
    printf("%c", binary_array[i]);
  }
  printf("\n");

  int number = binary_to_int(binary_array, length);
  // printf("\n%d\n", number);
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

  int processed_bits = 0;
  int offset;
  int current_byte;

  while (1)
  {
    offset = processed_bits % 8;
    current_byte = processed_bits / 8;
    // printf("%d", offset);

    fseek(input_file, current_byte, SEEK_SET);

    int lookup_index = read_buffer_bits(LOOKUP_TABLE_INDEX_SIZE, offset, input_file);
    // printf("%d\n", lookup_index);
    if (lookup_index == -1)
    {
      printf("REACHED END OF FILE");
      break;
    }

    fputc(decode_lookup[lookup_index][0], output_file);
    processed_bits = processed_bits + decode_lookup[lookup_index][1];
  }
}

void load_frequency(char *input_filename, int character_frequency[])
{
  FILE *fp = fopen(input_filename, "rb");
  if (fp == NULL)
  {
    printf("Error: Filename could not be opened\n");
    exit(2);
  }
  char c = fgetc(fp);
  while (c != EOF)
  {
    character_frequency[c]++;
    c = fgetc(fp);
  }
  fclose(fp);
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

  char input_c = fgetc(input_file);
  char output_byte_buffer = 0;
  int bitcount = 0;
  while (input_c != EOF)
  {

    // printf("--> %c, %d, = ", input_c, encode_array_length[input_c]);
    for (int i = 0; i < encode_array_length[input_c]; i++)
    {
      // printf("%d", encode_array[input_c][i]);
      if (bitcount == 8)
      {
        fputc(output_byte_buffer, output_file);
        output_byte_buffer = 0;
        bitcount = 0;
      }

      output_byte_buffer = output_byte_buffer << 1 | encode_array[input_c][i];
      bitcount++;
    }
    //////this translates code contained in encode_array to its binary equivalent, then outputs the bits as a char every 8 bits
    // printf("\n");
    input_c = fgetc(input_file);
  }
  int padding_bits_count = 0;
  if (bitcount != 0)
  {
    padding_bits_count = 8 - bitcount;
    // printf("PADDING BITS COUNT: %d\n", padding_bits_count);
    while (bitcount != 8)
    {
      output_byte_buffer = output_byte_buffer << 1;
      bitcount++;
    }
    fputc(output_byte_buffer, output_file);
  }
  // fputc(0, output_file);
  // fputc(0xFF, output_file);
  // fputc(padding_bits_count, output_file);
  /////this outputs any remaining bits leftover with padding bits, then outputs 0000 0000 1111 1111 {# of padding bits}

  fclose(input_file);
  fclose(output_file);
}

void decode_file(char *input_filename, char *output_filename)
{
  FILE *input_file = fopen(input_filename, "rb");
  FILE *output_file = fopen(output_filename, "w");

  if (input_file == NULL || output_file == NULL)
  {
    printf("Error: Filename could not be opened\n");
    exit(4);
  }

  struct TreeNode *pointer = head;
  unsigned char input_c = fgetc(input_file);
  unsigned char input_c2 = fgetc(input_file);
  unsigned char input_c3 = fgetc(input_file);
  // printf("-----------------------\n");
  // printf("%d\n", input_c);
  // printf("%d\n", input_c2);
  // printf("%d\n", input_c3);
  // printf("-----------------------\n");
  int buffer_byte = input_c;
  int bit_count = 8;
  int bit;
  int flag = 1;
  while (1)
  {
    bit = 1 & (buffer_byte >> 7);
    buffer_byte = buffer_byte << 1;
    bit_count--;
    // printf("%d\n", bit);

    if (bit == 0)
    {
      pointer = pointer->child1;
    }
    else
    {
      pointer = pointer->child2;
    }
    if (is_leaf_node(pointer))
    {
      fputc(pointer->data, output_file);
      pointer = head;
    }
    if (bit_count == 0)
    {
      // printf("\n");
      if (flag)
      {
        input_c = input_c2;
        input_c2 = input_c3;
        input_c3 = fgetc(input_file);
        bit_count = 8;
        buffer_byte = input_c;
      }
      else
      {
        break;
      }

      if ((input_c2 == 0x00) && (input_c3 == 0xFF))
      {
        flag = 0;
        bit_count = 8 - fgetc(input_file);
      }
    }
  }

  fclose(input_file);
  fclose(output_file);
}

int main(int argc, char *argv[])
{
  // if (argc != 3)
  // {
  // 	printf("ERROR! Incorrect number of arguments. Try: ./huffman <input.txt> <output.txt>\n");
  // 	exit(1);
  // }

  char character_array[CHARACTER_COUNT];
  int character_frequency[CHARACTER_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 170, 7821, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17691, 240, 213, 283, 150, 200, 222, 254, 842, 905, 350, 601, 6741, 2660, 6265, 442, 713, 1240, 1380, 890, 610, 633, 348, 250, 249, 306, 775, 220, 250, 1603, 226, 1426, 350, 2877, 991, 803, 952, 652, 461, 486, 2146, 4308, 235, 342, 842, 605, 850, 706, 463, 250, 516, 1829, 2844, 300, 300, 1140, 1577, 1223, 566, 220, 220, 195, 182, 245, 280, 57205, 7806, 7620, 10686, 40645, 7903, 8699, 17764, 17418, 1202, 8370, 17745, 17516, 18484, 15880, 12287, 890, 37464, 16860, 15368, 19214, 6842, 15965, 1102, 16208, 442, 235, 233, 235, 0, 0};
  char temp_array[CHARACTER_COUNT];

  for (int i = 0; i < CHARACTER_COUNT; i++)
  {
    character_array[i] = i;
  }

  create_node_linked_list(character_array, character_frequency); // creates doubly linked list of nodes containning char and freq
  create_huffman_tree();
  extract_encode_bit_combinaion(head, temp_array, 0);

  encode_input_text(argv[1], argv[2]);

  build_lookup_table();
  decode_with_lookup_table(argv[2], "decoded.txt");
  //   decode_file(argv[2], "decoded.txt");
}
