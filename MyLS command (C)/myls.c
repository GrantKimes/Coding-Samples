/*
 * ECE 322 HW1: myls command
 * Replacement for the ls command. Takes four possible intput options (-aslU).
 * Lists the contents of the inputted directory. 
 *
 * Grant Kimes
 * 9/16/15
 */


#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

const int max_len = 255;	// Max length of a string (char *[max_len]).
const int max_num_entries = 100;	// Max number of entries in the directory given.

const int U_bit = 1;		// Do a bitwise & with option_values to determine which options were inputted. 
const int s_bit = 2;
const int a_bit = 4;
const int l_bit = 8;


int calculate_options(char * c)
{
  if (c == NULL)	// If NULL, no options given.
    return 0;
  if (c[0] != '-')	// If it doesn't exist or doesn't start with '-', invalid input.
    return -1;
  int ans = 0, i = 1;

  while (c[i] != 0)
  {
    if (c[i] == 'U' && !(ans & U_bit))		// Unsorted output option.
      ans += U_bit;
    else if (c[i] == 's' && !(ans & s_bit))	// Sort by size option.
      ans += s_bit;
    else if (c[i] == 'a' && !(ans & a_bit))	// List hidden files option.
      ans += a_bit;
    else if (c[i] == 'l' && !(ans & l_bit))	// Long listing option.
      ans += l_bit;
    else
      return -1;				// Invalid input.

    i++;
  }
  return ans;
}

struct stat_and_name
{
  struct stat *S;
  const char *N;
};

void make_stat_and_name(struct stat_and_name * this_ptr, struct stat *s, const char *n)
{
  // Constructor for struct stat_and_name.
  this_ptr -> S = s;	// Pointer to a stat struct.
  this_ptr -> N = n;	// Pointer to name string.
}

void read_to_list(const char * pathname, struct stat_and_name * list[max_num_entries])
{
  // Take an empty list of stat_and_name structs. Load it with all directory entries inside the current directory. 

  DIR * curr_dir = opendir(pathname);			// Open directory of base pathname.
  struct dirent * dir_ent;				// Directory entry struct.
  int i = 0;						// Position in list array.
  char curr_pathname[max_len];				// Temp string of the name of the current file being looked at.
  struct stat * statbuf;				// Buffer for stat read function.
  struct stat_and_name * myStatN;			// Buffer for entry in list of stat_and_name structs.

  while ((dir_ent = readdir(curr_dir)) != NULL)		// Get next directory entry while there's still some available.
  {
    strcpy(curr_pathname, pathname);			// Append the name of the current file onto the directory's pathname.
    strcat(curr_pathname, "/");
    strcat(curr_pathname, dir_ent -> d_name);

    statbuf = malloc(sizeof(struct stat));			// Allocate space for stat struct.

    if (stat(curr_pathname, statbuf) == 0)		// Get stat of current file, check for success.
    {
      myStatN = malloc(sizeof(struct stat_and_name));	// Allocate space for stat_and_name struct.
      make_stat_and_name(myStatN, statbuf, dir_ent -> d_name);	// Initialize stat_and_name struct with current stat and name.
      list[i] = myStatN;
    }
    else
      printf("Unsuccessful stat read.\n");

    i++;						// Move to next index in list of stat_and_name structs.
    if (i == max_num_entries)
      break;
  }
}

void sort_list(struct stat_and_name * list[max_num_entries], int option_values)
{
  // Sort list of stat_and_name structs. If neither U_bit or s_bit set, default to alphabetical sorting.
  // If U_bit set, leave unsorted. If s_bit set, sort by size. -U overrides -s.

  int i, j, min_index;
  char sorting_option;				// Can be unsorted, by size, or alpabetically.
  struct stat_and_name * temp;			// Temp struct for swapping entries.
  temp = malloc(sizeof(struct stat_and_name));

  if ((option_values & U_bit) == U_bit)		// Unsorted.
    return;
  else if ((option_values & s_bit) == s_bit)	// Sort by size.
    sorting_option = 's';
  else
    sorting_option = 'a';			// Sort alphabetically.

  // Selection sort. 
  for (i = 0; i < max_num_entries; i++)
  {
    if (list[i] == NULL)			// Nothing else in the list, done sorting.
      return;
    min_index = i;				// Reset current min to i at the beginning of each loop.

    for (j = i; j < max_num_entries; j++)
    {
      if (list[j] == NULL)			// At the end of the inner loop, switch current min with i and go to outer loop.
      {
        temp = list[i];
        list[i] = list[min_index];
        list[min_index] = temp;
        break;
      }

      if (sorting_option == 'a')		// If sorting alphabetically, compare strings.
        if (strcmp(list[min_index]->N, list[j]->N) > 0)		// If current is smaller string than previous min, update min.
          min_index = j;

      if (sorting_option == 's')		// If sorting by size, compare sizes. 
        if (list[min_index]->S->st_size > list[j]->S->st_size)	// If current is smaller size than previous min, update min.
          min_index = j;

    }
  }
}

void print_list(struct stat_and_name * list[max_num_entries], int option_values)
{
  // Print out list of entries in current directory. -l makes long listing. -a includes hidden files.

  int i;
  int include_hidden = option_values & a_bit;	// -a is selected.
  int long_listing = option_values & l_bit;	// -l is selected.

  for (i = 0; i < max_num_entries; i++)
  {
    if (list[i] == NULL)			// Reached the last entry in list. 
      return;
    
    if (long_listing == l_bit)			// If on long listing mode, list all necessary values.
    {
      if (include_hidden == a_bit || list[i]->N[0] != '.')	// Long listing if: not a hidden file, or is hidden and include_hidden is on.
      {
        mode_t M = list[i]->S->st_mode;				// Print file type and permissions.
        switch(M & S_IFMT) 
        {
	  case S_IFREG: printf("-");	break;
          case S_IFDIR:	printf("d");	break;
	  case S_IFLNK: printf("l");	break;
	  default:	printf("u");	break;
        }
	if (M & 256) printf("r"); else printf("-");
	if (M & 128) printf("w"); else printf("-");
	if (M & 64)  printf("x"); else printf("-");
	if (M & 32)  printf("r"); else printf("-");
	if (M & 16)  printf("w"); else printf("-");
	if (M & 8)   printf("x"); else printf("-");
	if (M & 4)   printf("r"); else printf("-");
	if (M & 2)   printf("w"); else printf("-");
	if (M & 1)   printf("x"); else printf("-");

        printf("  %ld  ", (long) list[i]->S->st_nlink);		// Link count.
        printf("%ld  ", (long) list[i]->S->st_uid);		// UID.
        printf("%ld\t", (long) list[i]->S->st_gid);		// GID.
        printf("%lld\t", (long long) list[i]->S->st_size);	// Size in bytes.
        printf("%s\t\t", list[i]->N);				// File name.
        printf("%s", ctime(&list[i]->S->st_mtime));		// Last modification time.
      }
    }
    else if (include_hidden == a_bit || list[i]->N[0] != '.')	// Regular listing if either: listing all or not a hidden file.
      printf("%s\n" , list[i]->N);
  }
}

int main(int argc, char * argv[])
{
  char * options, * pathname;

  // Determine which parameters have been given as input, and if there are options or a directory selected.
  if (argc == 1)		// No parameters given, default to current directory. 
  {
    options = NULL;
    pathname = (char *) getwd(NULL);
  }
  else  if (argc == 2)		// One parameter given: either options or a directory.
  {
    options = argv[1];
    if (calculate_options(options) == -1)	// Not valid options, therefore a directory.
    {
      options = NULL;
      pathname = argv[1];
    }
    else
      pathname = (char *) getwd(NULL);		// Valid options, directory is current working directory. 
  }
  else if (argc == 3)		// Parameters include options and directory. 
  {
    options = argv[1];
    pathname = argv[2];
  }
  else				// Invalid parameters.
  {
    printf("Invalid number of arguments. Input any possible options (-Usal) and a valid directory path.\n");
    return;
  }

  //printf("  Directory being listed: %s\n\n" , pathname);

  int option_values = calculate_options(options);	// Returns int for bitwise comparisons to determine which options were selected.
  if (option_values == -1)				// Returns -1 if invalid options.
  {
    printf("Invalid parameter input. Valid options are -Usal\n");
    return;
  }

  DIR * curr_dir = opendir(pathname);			// Open directory.
  if (curr_dir == NULL)
  {
    printf("Invalid directory input. Format:\n");
    printf("myls [-aslU] [directory path]\n");
    return 0;
  }

  struct stat_and_name * list[max_num_entries];		// Declare array of pointers to stat_and_name structs.
  read_to_list(pathname, list);				// Read all files in directory, and add them to list of stat_and_name structs.

  sort_list(list, option_values);			// Sort list, returning new list in correct order. -U and -s will affect output.
  print_list(list, option_values);			// Print list. -a and -l will affect output. 
  printf("\n");

  if (closedir(curr_dir) == -1)
    printf("Error closing directory.\n");		// Close directory. 
}

