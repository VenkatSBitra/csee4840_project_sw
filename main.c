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

int read_data(const char *fpath, uint8_t **data) {
  FILE *fp = fopen(fpath, "r");
  if (fp == NULL) {
      perror("Error opening file");
      return -1;
  }

  int n;

  // Read the first line to get the dimensions
  fscanf(fp, "%d", &n);

  // Allocate memory for the data
  *data = (uint8_t *)malloc(n * sizeof(uint8_t));
  if (data == NULL) {
      perror("Error allocating memory");
      fclose(fp);
      return -1;
  }

  // Read the data into the array
  for (int i = 0; i < n; i++) {
      uint8_t x, y;
      fscanf(fp, "%u %u", &y, &x);
      (*data)[i] = (x << 4) | y;
  }

  fclose(fp);

  return n;
}

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
    perror("ioctl(lr_acc_SET_DATA) failed");
    return;
  }
}

int main()
{
  lr_acc_arg_t vla;
  int i;
  static const char filename[] = "/dev/lr_acc";

  uint8_t *data = NULL;
  int n = read_data("preprocessed_data.txt", &data);
  if (n < 0) {
      fprintf(stderr, "Error reading data\n");
      return -1;
  }

  lr_acc_data_t d;

  for (int i = 0; i < n; i++) {
      d.data = data[i];
      vla.data = d;
      vla.address = i;
      vla.go = 0;

      set_lr_data(&vla);
  }

  vla.go = 1;
  set_lr_data(&vla);

  printf("VGA ball Userspace program started\n");

  if ((lr_acc_fd = open(filename, O_RDWR)) == -1)
  {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }

  printf("initial state: ");

  

  printf("LR Accumulator Userspace program terminating\n");
  return 0;
}
