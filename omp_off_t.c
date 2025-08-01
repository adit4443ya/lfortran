#ifdef USE_GPU
  #include <cuda_runtime.h>
#else
  #include "cpu_impl.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Kernel function for the compute loop
#ifdef USE_GPU
__global__ 
#endif
void compute_kernel(float *a_data, float *b_data, int n, 
                    int a_stride, int a_offset, int a_lower_bound,
                    int b_stride, int b_offset, int b_lower_bound) {
    
    // Calculate global thread ID
    int i = blockIdx.x * blockDim.x + threadIdx.x + 1; // +1 for Fortran 1-based indexing
    
    if (i <= n) {
        // Calculate array indices with Fortran-style stride and offset
        int a_idx = (0 + (a_stride * (i - a_lower_bound))) + a_offset;
        int b_idx = (0 + (b_stride * (i - b_lower_bound))) + b_offset;
        
        // Compute: a(i) = real(i) + b(i)*340
        a_data[a_idx] = (float)i + b_data[b_idx] * 340.0f;
    }
}

#ifndef USE_GPU
// CPU kernel wrapper to unpack arguments
void compute_kernel_wrapper(void **args) {
    // Unpack arguments from void** array
    float *a_data = *(float**)args[0];
    float *b_data = *(float**)args[1];
    int n = *(int*)args[2];
    int a_stride = *(int*)args[3];
    int a_offset = *(int*)args[4];
    int a_lower_bound = *(int*)args[5];
    int b_stride = *(int*)args[6];
    int b_offset = *(int*)args[7];
    int b_lower_bound = *(int*)args[8];
    
    // Call the actual kernel
    compute_kernel(a_data, b_data, n, a_stride, a_offset, a_lower_bound,
                  b_stride, b_offset, b_lower_bound);
}
#endif


struct dimension_descriptor
{
    int32_t lower_bound, length, stride;
};

struct r32
{
    float *data;
    struct dimension_descriptor dims[32];
    int32_t n_dims;
    int32_t offset;
    bool is_allocated;
};


int main(int argc, char* argv[])
{
#ifndef USE_GPU
    cpu_runtime_init();
#endif
    
    struct r32 a_value;
    struct r32* a = &a_value;
    a->n_dims = 1;
    a->offset = 0;
    a->dims[0].lower_bound = 1;
    a->dims[0].length = 10000000;
    a->dims[0].stride = 1;
    a->data = (float*) malloc(a->dims[0].length * sizeof(float));
    a->is_allocated = true;
    
    struct r32 b_value;
    struct r32* b = &b_value;
    b->n_dims = 1;
    b->offset = 0;
    b->dims[0].lower_bound = 1;
    b->dims[0].length = 10000000;
    b->dims[0].stride = 1;
    b->data = (float*) malloc(b->dims[0].length * sizeof(float));
    b->is_allocated = true;
    
    // Initialize b array
    for (int i = 0; i < b->dims[0].length; i++) {
        b->data[i] = 5.0f;
    }
    
    float *d_a_data = NULL;
    float *d_b_data = NULL;

    // Execute target region
    size_t a_data_size = a->dims[0].length * sizeof(float);
    size_t b_data_size = b->dims[0].length * sizeof(float);
    
    // Memory allocation on device
    cudaError_t err;
    err = cudaMalloc((void**)&d_a_data, a_data_size);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed for a_data: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    
    err = cudaMalloc((void**)&d_b_data, b_data_size);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed for b_data: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    
    // Memory transfer: Host to Device
    // Note: for map(tofrom:a), we might skip copying uninitialized data, but for safety we copy it
    err = cudaMemcpy(d_a_data, a->data, a_data_size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy H2D failed for a_data: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    
    err = cudaMemcpy(d_b_data, b->data, b_data_size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy H2D failed for b_data: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    
    // Kernel execution configuration
    int n = 10000000;
    int threads_per_block = 32;
    int blocks = (n + threads_per_block - 1) / threads_per_block;
    
    dim3 grid_dim = {blocks, 1, 1};
    dim3 block_dim = {threads_per_block, 1, 1};
    
    // Kernel arguments
    void *kernel_args[] = {
        &d_a_data,
        &d_b_data, 
        &n,
        &(a->dims[0].stride),
        &(a->offset),
        &(a->dims[0].lower_bound),
        &(b->dims[0].stride),
        &(b->offset),
        &(b->dims[0].lower_bound)
    };
    
    
    // Launch kernel
    err = cudaLaunchKernel((void*)compute_kernel, grid_dim, block_dim, 
                          kernel_args, 0, NULL);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaLaunchKernel failed: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    
    // Wait for kernel completion
    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize failed: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    
    // Memory transfer: Device to Host
    err = cudaMemcpy(a->data, d_a_data, a_data_size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy D2H failed for a_data: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    
    err = cudaMemcpy(b->data, d_b_data, b_data_size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy D2H failed for b_data: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    
    // Cleanup device memory
    cudaFree(d_a_data);
    cudaFree(d_b_data);
    
    // Verification (Fortran 1-based indexing)
    int idx5 = (0 + (a->dims[0].stride * (5 - a->dims[0].lower_bound))) + a->offset;
    printf("a[5] = %f, b[5] = %f\n", a->data[idx5+1000000], b->data[idx5]);
    
    if (a->data[idx5] != 1705.0f) {
        fprintf(stderr, "ERROR STOP: Expected a[5] = 1705.0, got %f\n", a->data[idx5]);
        exit(1);
    }
    if (b->data[idx5] != 5.0f) {
        fprintf(stderr, "ERROR STOP: Expected b[5] = 5.0, got %f\n", b->data[idx5]);
        exit(1);
    }
    // Cleanup
    free(a->data);
    free(b->data);
    
#ifndef USE_GPU
    cpu_runtime_cleanup();
#endif
    
    return 0;
}
//gcc -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/ -c /DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime/lfortran_intrinsics.c -o intrinsic.o
//gcc  -fopenmp -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/ cpu_impl.c omp_off_gen.c intrinsic.o -lm -o a && ./a && rm a


// nvcc -O2 -x cu -DUSE_GPU -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/  -c omp_off_gen.c -o omp_off_gen.o
// nvcc intrinsic.o omp_off_gen.o -lm -o a && ./a && rm ./a
