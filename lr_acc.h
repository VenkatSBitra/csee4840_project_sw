#ifndef _lr_acc_H
#define _lr_acc_H

#include <linux/ioctl.h>
#include <stdint.h>

typedef struct {
  uint8_t data;
} lr_acc_data_t;

typedef struct {
  lr_acc_data_t data;
  short address;
  uint8_t go;
} lr_acc_arg_t;
  

#define LR_ACC_MAGIC 'q'

/* ioctls and their arguments */
#define LR_ACC_WRITE_DATA _IOW(LR_ACC_MAGIC, 1, lr_acc_arg_t)
#define LR_ACC_READ_DATA  _IOR(LR_ACC_MAGIC, 2, lr_acc_arg_t)

#endif
