#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARACTER_COUNT 128

struct TreeNode
{
  signed char data;
  int freq;

  struct TreeNode *left, *right, *child1, *child2;
};

struct TreeNode *create_node(signed char data, int freq);
struct TreeNode *create_parent_node(int freq1, int freq2);
void insert_node(signed char data, int freq);
void insert_parent_node(int data, int freq);
void create_node_linked_list(signed char character_array[], int character_frequency[]);
void print_list();
void print_list_reverse();
void print_node(struct TreeNode *list_pointer);
void create_huffman_tree();
void detach_node(struct TreeNode *node);
int is_leaf_node(struct TreeNode *node);
void extract_encode_bit_combinaion(struct TreeNode *node, signed char array[], int array_index);
void load_frequency(char *input_filename, int character_frequency[]);
void encode_input_text(char *input_filename, char *output_filename);
void pure_huffman_decoding(char *input_filename, char *output_filename);

struct TreeNode *head = NULL;
struct TreeNode *tail = NULL;
unsigned char encode_array[CHARACTER_COUNT][CHARACTER_COUNT];
unsigned char encode_array_length[CHARACTER_COUNT];

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
  int count = 0;
  int i;
  for (i = 0; i < CHARACTER_COUNT; i++)
  {
    if (character_frequency[i] > 0)
    {
      count++;
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

void extract_encode_bit_combinaion(struct TreeNode *node, signed char array[], int array_index)
{
  if (node != NULL)
  {
    if (is_leaf_node(node))
    {
      array[array_index] = '\0';
      int bit_combination = 0;
      int bit_count = 0;
      int prefix = 0;
      int prefix_flag = 0;
      int i;
      for (i = 0; i < array_index; i++)
      {
        encode_array[node->data][i] = array[i];
        bit_combination = bit_combination << 1 | array[i];
        bit_count++;
      }

      encode_array_length[node->data] = array_index;
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

void load_frequency(char *input_filename, int character_frequency[])
{
  FILE *fp = fopen(input_filename, "rb");
  if (fp == NULL)
  {
    printf("Error: Filename could not be opened\n");
    exit(2);
  }
  signed char c = fgetc(fp);
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

  signed char input_c = fgetc(input_file);
  signed char output_byte_buffer = 0;
  int bitcount = 0;
  while (input_c != EOF)
  {

    int i;
    for (i = 0; i < encode_array_length[input_c]; i++)
    {
      if (bitcount == 8)
      {
        fputc(output_byte_buffer, output_file);
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
    while (bitcount != 8)
    {
      output_byte_buffer = output_byte_buffer << 1;
      bitcount++;
    }
    fputc(output_byte_buffer, output_file);
  }
  // chars 0x00 and 0xFF will signal the end of file
  fputc(0x00, output_file);
  fputc(0xFF, output_file);
  // add padding count
  fputc(padding_bits_count, output_file);

  fclose(input_file);
  fclose(output_file);
}

void pure_huffman_decoding(char *input_filename, char *output_filename)
{
  FILE *input_file = fopen(input_filename, "rb");
  FILE *output_file = fopen(output_filename, "w");

  if (input_file == NULL || output_file == NULL)
  {
    printf("Error: Filename could not be opened\n");
    exit(4);
  }

  struct TreeNode *pointer = head;
  signed char input_c = fgetc(input_file);
  signed char input_c2 = fgetc(input_file);
  signed char input_c3 = fgetc(input_file);
  signed char buffer_byte = input_c;
  int bit_count = 0;
  int bit;
  int EOF_flag = 1;

  int prefix_flag = 0;
  int prefix = 0;
  int bit_combination = 0;
  int bit_combination_count = 0;
  while (1)
  {
    bit = 1 & (buffer_byte >> 7);
    bit_combination = (bit_combination << 1) | (1 & (buffer_byte >> 7));
    buffer_byte = buffer_byte << 1;
    bit_count++;
    bit_combination_count++;

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

    if (bit_count == 8)
    {
      if (EOF_flag)
      {
        input_c = input_c2;
        input_c2 = input_c3;
        input_c3 = fgetc(input_file);
        bit_count = 0;
        buffer_byte = input_c;
      }
      else
      {
        break;
      }

      if ((input_c2 == 0x00) && (input_c3 == 0xFF))
      {
        EOF_flag = 0;
        bit_count += fgetc(input_file);
      }
    }
  }

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

  signed char character_array[CHARACTER_COUNT];
  int character_frequency[CHARACTER_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 170, 7821, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17691, 240, 213, 283, 150, 200, 222, 254, 842, 905, 350, 601, 6741, 2660, 6265, 442, 713, 1240, 1380, 890, 610, 633, 348, 250, 249, 306, 775, 220, 250, 1603, 226, 1426, 350, 2877, 991, 803, 952, 652, 461, 486, 2146, 4308, 235, 342, 842, 605, 850, 706, 463, 250, 516, 1829, 2844, 300, 300, 1140, 1577, 1223, 566, 220, 220, 195, 182, 245, 280, 27205, 7806, 7620, 10686, 20645, 7903, 8699, 17764, 17418, 1202, 8370, 21745, 17516, 18484, 15880, 12287, 890, 37464, 16860, 15368, 19214, 6842, 15965, 1102, 16208, 442, 235, 233, 235, 0, 0};
  signed char temp_array[CHARACTER_COUNT];

  int i;
  for (i = 0; i < CHARACTER_COUNT; i++)
  {
    character_array[i] = i;
  }

  create_node_linked_list(character_array, character_frequency);
  create_huffman_tree();
  extract_encode_bit_combinaion(head, temp_array, 0);

  encode_input_text(argv[1], argv[2]);
  pure_huffman_decoding(argv[2], "decoded.txt");
}
