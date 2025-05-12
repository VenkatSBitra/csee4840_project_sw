/*
 * Userspace program that communicates with the lr_acc device driver
 * through ioctls
 *
 * Stephen A. Edwards
 * Columbia University
 */

#include <stdio.h>
#include "lr_acc.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int lr_acc_fd;

static const unsigned int raw_data[][2] = {
  {1, 15}, {1, 14}, {1, 15}, {1, 15}, {1, 15}, {1, 14}, {1, 15}, 
  {1, 15}, {1, 14}, {1, 15}, {1, 14}, {1, 15}, {1, 15}, {1, 13}, 
  {1, 15}, {1, 14}, {1, 15}, {1, 15}, {1, 15}, {1, 14}, {1, 14}, 
  {1, 15}, {1, 15}, {1, 15}, {1, 15}, {1, 15}, {1, 14}, {1, 15}, 
  {1, 15}, {1, 15}, {1, 15}, {1, 14}, {1, 15}, {1, 15}, {1, 15}, 
  {1, 14}, {1, 14}, {1, 15}, {1, 14}, {1, 15}, {1, 15}, {1, 15}, 
  {1, 14}, {1, 14}, {1, 14}, {1, 14}, {1, 14}, {1, 14}, {2, 12}, 
  {2, 12}, {2, 12}, {2, 13}, {2, 12}, {2, 12}, {2, 13}, {2, 12}, 
  {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 13}, {2, 12}, 
  {2, 13}, {2, 13}, {2, 13}, {2, 13}, {2, 13}, {2, 13}, {2, 13}, 
  {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 13}, {2, 13}, 
  {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 13}, {2, 12}, 
  {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, 
  {2, 12}, {2, 12}, {2, 12}, {2, 12}, {2, 12}, {3, 12}, {3, 13}, 
  {3, 13}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, 
  {3, 12}, {3, 12}, {3, 12}, {3, 11}, {3, 13}, {3, 12}, {3, 11}, 
  {3, 12}, {3, 12}, {3, 12}, {3, 11}, {3, 12}, {3, 12}, {3, 12}, {3, 13}, {3, 12}, {3, 12}, {3, 12}, {3, 13}, {3, 13}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 13}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {3, 13}, {3, 12}, {3, 12}, {3, 12}, {3, 12}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 11}, {4, 11}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 11}, {4, 10}, {4, 11}, {4, 11}, {4, 11}, {4, 10}, {4, 11}, {4, 11}, {4, 11}, {4, 11}, {4, 10}, {4, 10}, {4, 11}, {4, 10}, {4, 11}, {4, 11}, {4, 11}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 10}, {4, 11}, {4, 10}, {4, 11}, {4, 10}, {4, 11}, {4, 11}, {4, 11}, {4, 11}, {4, 11}, {4, 11}, {4, 11}, {5, 7}, {5, 7}, {5, 8}, {5, 9}, {5, 9}, {5, 8}, {5, 9}, {5, 9}, {5, 9}, {5, 9}, {5, 8}, {5, 9}, {5, 8}, {5, 10}, {5, 8}, {5, 10}, {5, 9}, {5, 9}, {5, 10}, {5, 10}, {5, 11}, {5, 10}, {5, 10}, {5, 10}, {5, 10}, {5, 9}, {5, 9}, {5, 9}, {5, 9}, {5, 9}, {5, 10}, {5, 10}, {5, 9}, {5, 9}, {5, 9}, {5, 9}, {5, 9}, {5, 10}, {5, 10}, {5, 9}, {5, 10}, {5, 10}, {5, 9}, {5, 10}, {5, 10}, {5, 9}, {5, 9}, {5, 9}, {6, 6}, {6, 6}, {6, 6}, {6, 7}, {6, 7}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 5}, {6, 5}, {6, 5}, {6, 6}, {6, 5}, {6, 5}, {6, 5}, {6, 5}, {6, 6}, {6, 5}, {6, 5}, {6, 5}, {6, 5}, {6, 5}, {6, 5}, {6, 5}, {6, 6}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {6, 8}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 7}, {7, 6}, {7, 6}, {7, 7}, {7, 7}, {7, 6}, {7, 6}, {7, 6}, {7, 7}, {7, 7}, {7, 6}, {7, 6}, {7, 5}, {7, 5}, {7, 5}, {7, 4}, {7, 5}, {7, 5}, {7, 4}, {7, 4}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 4}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {8, 2}, {8, 2}, {8, 1}, {8, 2}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 2}, {8, 1}, {8, 4}, {8, 4}, {8, 2}, {8, 0}, {8, 2}, {8, 2}, {8, 1}, {8, 2}, {8, 2}, {8, 2}, {8, 2}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 2}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 2}, {8, 1}, {8, 1}, {8, 2}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 2}, {8, 1}, {8, 1}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 1}, {9, 0}, {9, 0}, {9, 0}, {9, 1}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 2}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 1}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}, {9, 0}
};

#define NUM_ENTRIES (sizeof(raw_data) / sizeof(raw_data[0]))

int read_data_from_array(char **data) {
    *data = (char *)malloc(NUM_ENTRIES * sizeof(char));
    if (*data == NULL) {
        perror("Error allocating memory");
        return -1;
    }

    for (int i = 0; i < NUM_ENTRIES; i++) {
        unsigned int y = raw_data[i][0];
        unsigned int x = raw_data[i][1];
        (*data)[i] = (x << 4) | y;
    }

    return NUM_ENTRIES;
}


// int read_data(const char *fpath, char **data) {
//   FILE *fp = fopen(fpath, "r");
//   if (fp == NULL) {
//       perror("Error opening file");
//       return -1;
//   }

//   int n;

//   // Read the first line to get the dimensions
//   fscanf(fp, "%d", &n);

//   // Allocate memory for the data
//   *data = (char *)malloc(n * sizeof(char));
//   if (data == NULL) {
//       perror("Error allocating memory");
//       fclose(fp);
//       return -1;
//   }

//   // Read the data into the array
//   for (int i = 0; i < n; i++) {
//       char x, y;
//       fscanf(fp, "%u %u", &y, &x);
//       (*data)[i] = (x << 4) | y;
//   }

//   fclose(fp);

//   return n;
// }

/* Read and print the background color */
// void print_background_color()
// {
//   lr_acc_arg_t vla;

//   if (ioctl(lr_acc_fd, LR_ACC_READ_BACKGROUND, &vla))
//   {
//     perror("ioctl(lr_acc_READ_BACKGROUND) failed");
//     return;
//   }
//   printf("%02x %02x %02x\n",
//          vla.background.red, vla.background.green, vla.background.blue);
// }

/* Set the background color */
// void set_background_color(const lr_acc_color_t *c)
// {
//   lr_acc_arg_t vla;
//   vla.background = *c;
//   if (ioctl(lr_acc_fd, LR_ACC_WRITE_BACKGROUND, &vla))
//   {
//     perror("ioctl(lr_acc_SET_BACKGROUND) failed");
//     return;
//   }
// }

// void set_ball_position(const lr_acc_position_t *p)
// {
//   lr_acc_arg_t vla;
//   vla.position = *p;
//   if (ioctl(lr_acc_fd, LR_ACC_WRITE_POSITION, &vla))
//   {
//     perror("ioctl(lr_acc_SET_POSITION) failed");
//     return;
//   }
// }

void set_lr_data(const lr_acc_arg_t *d)
{

  if (ioctl(lr_acc_fd, LR_ACC_WRITE_DATA, d))
  {
    perror("ioctl(LR_ACC_SET_DATA) failed");
    return;
  }
}

int main()
{
  lr_acc_arg_t vla;
  int i;
  static const char filename[] = "/dev/lr_acc";

  if ((lr_acc_fd = open(filename, O_RDWR)) == -1)
  {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }

  char *data = NULL;
  // int n = read_data("/root/csee4840_project_sw/preprocessed_data.txt", &data);
  int n = read_data_from_array(&data);
  if (n < 0) {
      fprintf(stderr, "Error reading data\n");
      return -1;
  }

  printf("Read %d data points\n", n);

  lr_acc_data_t d;

  for (int i = 0; i < n; i++) {
      d.data = data[i];
      vla.data = d;
      vla.address = i;
      vla.go = 0;

      printf("Setting data: %d\n", data[i]);

      set_lr_data(&vla);
  }

  vla.go = 1;
  set_lr_data(&vla);  

  printf("LR Accumulator Userspace program terminating\n");
  return 0;
}
