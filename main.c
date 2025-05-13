/*
 * Userspace program that communicates with the lr_acc device driver
 * through ioctls
 *
 * Stephen A. Edwards
 * Columbia University
 */

#include <stdio.h>
#include <time.h>
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
        (*data)[i] = (y << 4) | x;
    }

    return NUM_ENTRIES;
}

void set_lr_data(const lr_acc_arg_t *d)
{

  if (ioctl(lr_acc_fd, LR_ACC_WRITE_DATA, d))
  {
    fprintf(stderr, "ioctl(LR_ACC_SET_DATA) failed");
    return;
  }
}

void read_lr_data(lr_acc_read_data_t *d)
{
  if (ioctl(lr_acc_fd, LR_ACC_READ_DATA, d))
  {
    fprintf(stderr, "ioctl(LR_ACC_GET_DATA) failed");
    return;
  }
}

int main()
{
  lr_acc_arg_t vla;
  lr_acc_read_data_t obj;
  int i;
  static const char filename[] = "/dev/lr_acc";

  if ((lr_acc_fd = open(filename, O_RDWR)) == -1)
  {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }

  char *data = NULL;
  int n = read_data_from_array(&data);
  if (n < 0) {
      fprintf(stderr, "Error reading data\n");
      return -1;
  }

  fprintf(stderr, "Read %d data points\n", n);

  lr_acc_data_t *d = (lr_acc_data_t *)malloc(sizeof(lr_acc_data_t));

  clock_t start, end;
  double cpu_time_used;

  start = clock();

  vla.go = 1;
  vla.address = 0;

  set_lr_data(&vla);

  for (int i = 0; i < n; i++) {
      d->data = data[i];
      vla.data = *d;
      vla.address = i;
      vla.go = 0;

      set_lr_data(&vla);
  }

  end = clock();

  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  fprintf(stderr, "Time taken to send data: %f microseconds\n", cpu_time_used * 1e6);

  start = clock();
  vla.go = 1;
  vla.address = n;
  set_lr_data(&vla);  

  // fprintf(stderr, "Data sent to the device\n");

  // fprintf(stderr, "Waiting for the device to finish processing...\n");

  while (1)
  {
    read_lr_data(&obj);

    if (obj.master_done == 1)
      break;

    usleep(1); // Sleep for 1 microsecond
  }

  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  fprintf(stderr, "Time taken to process and read data: %f microseconds\n", cpu_time_used * 1e6);

  start = clock();

  double w0 = (double)obj.n0 / (double)obj.d;
  double w1 = (double)obj.n1 / (double)obj.d;

  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  fprintf(stderr, "Time taken to calculate weights: %f microseconds\n", cpu_time_used * 1e6);
  
  fprintf(stderr, "Device finished processing\n");
  fprintf(stderr, "Results:\n");
  fprintf(stderr, "d: %d\n", obj.d);
  fprintf(stderr, "n0: %d\n", obj.n0);
  fprintf(stderr, "n1: %d\n", obj.n1);
  fprintf(stderr, "s1: %d\n", obj.s1);
  fprintf(stderr, "s2: %d\n", obj.s2);
  fprintf(stderr, "s3: %d\n", obj.s3);
  fprintf(stderr, "s4: %d\n", obj.s4);
  fprintf(stderr, "s5: %d\n", obj.s5);
  fprintf(stderr, "Weights:\n");
  fprintf(stderr, "w0: %f\n", w0);
  fprintf(stderr, "w1: %f\n", w1);

  fprintf(stderr, "Freeing allocated memory\n");
  free(data);
  free(d);
  close(lr_acc_fd);
  fprintf(stderr, "Closed the device file\n");

  fprintf(stderr, "LR Accumulator Userspace program terminating\n");
  return 0;
}
