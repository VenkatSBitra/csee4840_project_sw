#ifndef _lr_acc_H
#define _lr_acc_H

#include <linux/ioctl.h>

typedef struct {
  char data;
} lr_acc_data_t;

typedef struct {
  lr_acc_data_t data;
  int address;
  char go;
} lr_acc_arg_t;

typedef struct {
  int master_done;
  int d, n0, n1, s1, s2, s3, s4, s5;
} lr_acc_read_data_t;
  

#define LR_ACC_MAGIC 'q'

/* ioctls and their arguments */
#define LR_ACC_WRITE_DATA _IOW(LR_ACC_MAGIC, 1, lr_acc_arg_t)
#define LR_ACC_READ_DATA  _IOR(LR_ACC_MAGIC, 2, lr_acc_read_data_t)

#endif
