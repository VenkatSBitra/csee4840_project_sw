// #include <stdio.h>
// #include <stdlib.h>
// // #include "helper.h"

// int read_data(const char *fpath, int **X, int **Y) {
//     FILE *fp = fopen(fpath, "r");
//     if (fp == NULL) {
//         perror("Error opening file");
//         return -1;
//     }

//     int n;

//     // Read the first line to get the dimensions
//     fscanf(fp, "%d", &n);

//     // Allocate memory for the data
//     *X = (int *)malloc(n * sizeof(int));
//     *Y = (int *)malloc(n * sizeof(int));
//     if (X == NULL || Y == NULL) {
//         perror("Error allocating memory");
//         fclose(fp);
//         return -1;
//     }

//     // Read the data into the array
//     for (int i = 0; i < n; i++) {
//         double x, y;
//         fscanf(fp, "%le %le", &x, &y);
//         (*X)[i] = (int)(x);
//         (*Y)[i] = (int)(y);
//     }

//     fclose(fp);

//     return n;
// }

// // int piecewise_mult(short a1, short a2, short b1, short b2) {
// //     int sign = (int)((a1 & 0x8000) ^ (b1 & 0x8000)) << 16;
// //     a1 = a1 & 0x7FFF;
// //     b1 = b1 & 0x7FFF;

// //     int p2 = (int)a1 * b2;
// //     int p3 = (int)a2 * b1;
// //     int p4 = (int)a2 * b2;

// //     return (sign & 0x8000) ? (~(p4 + ((p3 + p2) >> 16))) + 1 : (p4 + ((p3 + p2) >> 16));
// // }

// int piecewise_mult(short a1, short a2, short b1, short b2) {
//     /* Reconstruct full 32-bit signed numbers */
//     int a_full = ((int)a1 << 16) | ((unsigned short)a2);
//     int b_full = ((int)b1 << 16) | ((unsigned short)b2);
    
//     /* Determine overall sign */
//     int sign = ((a_full < 0) ^ (b_full < 0)) ? -1 : 1;
    
//     /* Work with absolute values (in unsigned int) */
//     unsigned int abs_a = (a_full < 0) ? -a_full : a_full;
//     unsigned int abs_b = (b_full < 0) ? -b_full : b_full;
    
//     /* Split the absolute values into 16-bit parts */
//     short A1 = (short)(abs_a >> 16);      // high 16 bits (magnitude only)
//     short A2 = (short)(abs_a & 0xFFFF);     // low 16 bits
//     short B1 = (short)(abs_b >> 16);
//     short B2 = (short)(abs_b & 0xFFFF);
    
//     /* Compute the partial products. Note:
//          a = (A1 << 16) + A2,  b = (B1 << 16) + B2, so
//          a * b = (A1 * B1 << 32) + ((A1*B2 + A2*B1) << 16) + A2*B2.
//          The upper term (<<32) does not affect the lower 32 bits.
//     */
//     int p_low = A2 * B2;           // low16 x low16 product
//     int p_mid = (A1 * B2) + (A2 * B1);  // cross products
    
//     /* The lower 32 bits of the full product are:
//            result = (p_mid << 16) + p_low;
//        (We ignore the contribution from (A1*B1)<<32.)
//     */
//     int result = (p_mid << 16) + p_low;
    
//     return sign * result;
// }


// void acc_sum(int *acc, int xt, int yt) {
//     acc[0] = acc[0] + xt;
//     acc[1] = acc[1] + yt;
//     acc[2] = acc[2] + piecewise_mult(xt >> 16, xt & 0xFFFF, xt >> 16, xt & 0xFFFF);
//     acc[3] = acc[3] + piecewise_mult(xt >> 16, xt & 0xFFFF, yt >> 16, yt & 0xFFFF);
// }

// struct weights compute_weights(int n, int *acc) {
//     struct weights W;
//     double f = 1.0 / (n * acc[2] - acc[0] * acc[0]);
//     W.w2 = f * (n * acc[3] - acc[0] * acc[1]);
//     W.w1 = f * (acc[1] * acc[2] - acc[0] * acc[3]);
//     return W;
// }

// struct weights main_test(const char *fpath) {
//     int *X = NULL, *Y = NULL;
//     int acc[4];

//     // Initialize the accumulator
//     for (int i = 0; i < 4; i++) {
//         acc[i] = 0;
//     }

//     // Read data from file
//     int n = read_data(fpath, &X, &Y);

//     // Run the accumulator test
//     for (int i = 0; i < n; i++) {
//         acc_sum(acc, X[i], Y[i]);
//     }

//     // Compute the weights
//     struct weights W = compute_weights(n, acc);

//     // Print the computed weights
//     printf("Computed weights:\n");
//     printf("w1: %f, w2: %f\n", W.w1, W.w2);

//     // Free allocated memory
//     free(X);
//     free(Y);

//     return W;
// }