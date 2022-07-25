#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARACTER_COUNT 128

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
    if (character_frequency[i] >= 0)
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
      strcpy(encode_array[node->data], array);
      printf("Node:%c, Code:", node->data);
      for (int i = 0; i < array_index; i++)
      {
        printf("%c", array[i]);
      }
      printf("\n");
      return;
    }
    if (node->child1 != NULL)
    {
      array[array_index] = '0';
      extract_encode_bit_combinaion(node->child1, array, array_index + 1);
    }
    if (node->child2 != NULL)
    {
      array[array_index] = '1';
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
  FILE *output_file = fopen(output_filename, "w");
  if (input_file == NULL || output_file == NULL)
  {
    printf("Error: Filename could not be opened\n");
    exit(3);
  }

  char input_c = fgetc(input_file);
  while (input_c != EOF)
  {
    fputs(encode_array[input_c], output_file);
    input_c = fgetc(input_file);
  }
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
  char input_c = fgetc(input_file);
  while (input_c != EOF)
  {
    if (input_c == '0')
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

    input_c = fgetc(input_file);
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

  char character_array[CHARACTER_COUNT];
  int character_frequency[CHARACTER_COUNT];
  char temp_array[CHARACTER_COUNT];

  for (int i = 0; i < CHARACTER_COUNT; i++)
  {
    character_array[i] = i;
    character_frequency[i] = 0;
  }

  load_frequency(argv[1], character_frequency);                  // creates array with frequency of each character in text file
  create_node_linked_list(character_array, character_frequency); // creates doubly linked list of nodes containning char and freq
  create_huffman_tree();
  printf("Node:%c, Code:", head->data);
  extract_encode_bit_combinaion(head, temp_array, 0);

  encode_input_text(argv[1], argv[2]);
  decode_file(argv[2], "decoded.txt");
}