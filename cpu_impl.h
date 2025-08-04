#ifndef CPU_IMPL_H
#define CPU_IMPL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <math.h>

// CUDA Runtime API Emulation for CPU
typedef enum {
    cudaSuccess = 0,
    cudaErrorMemoryAllocation = 2,
    cudaErrorInvalidValue = 11
} cudaError_t;

// Device execution configuration
typedef struct {
    unsigned int x, y, z;
} dim3;

// Thread and block index emulation
typedef struct {
    unsigned int x, y, z;
} uint3;

// Global thread identifiers (CPU emulation)
extern __thread uint3 threadIdx;
extern __thread uint3 blockIdx;
extern __thread dim3 blockDim;
extern __thread dim3 gridDim;

// Memory management API
cudaError_t cudaMalloc(void **devPtr, size_t size);
cudaError_t cudaFree(void *devPtr);
cudaError_t cudaMemcpy(void *dst, const void *src, size_t count, int kind);
cudaError_t cudaDeviceSynchronize(void);

// Memory copy kinds
#define cudaMemcpyHostToDevice 1
#define cudaMemcpyDeviceToHost 2
#define cudaMemcpyDeviceToDevice 3

// Kernel launch emulation - NOTE: Changed function signature
cudaError_t cudaLaunchKernel(void *func, dim3 gridDim, dim3 blockDim, 
                            void **args, size_t sharedMem, void *stream);

// Error handling
const char* cudaGetErrorString(cudaError_t error);

// Device synchronization
#define __syncthreads() _Pragma("omp barrier")

// Memory allocation tracking structure
typedef struct {
    void *cpu_ptr;
    void *device_ptr;
    size_t size;
    int is_allocated;
} memory_tracker_t;


// Initialization function
void cpu_runtime_init(void);
void cpu_runtime_cleanup(void);

#endif // CPU_IMPL_H

// gcc -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/ -c /DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime/lfortran_intrinsics.c -o intrinsic.o
// gcc  -fopenmp -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/ cpu_impl.c omp_off_gen.c intrinsic.o -lm -o a && ./a && rm a


// nvcc -O2 -x cu -DUSE_GPU -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/  -c omp_off_gen.c -o omp_off_gen.o
// nvcc intrinsic.o omp_off_gen.o -lm -o a && ./a && rm ./a

// clang -fopenmp --offload-arch=sm_89 -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime -I/DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/ /DATA/prajjwal/Dynamic-MIS/Fully_Dynamic_MIS/ofld/lfortran/src/libasr/runtime/lfortran_intrinsics.c omp_off.c --libomptarget-nvptx-bc-path="$LIBOMPTARGET_NVPTX_BC_PATH" -L"$CONDA_PREFIX/lib" -L"$CONDA_PREFIX/lib/clang/18.1.8/lib" -Wl,-rpath,"$CONDA_PREFIX/lib" -lm -o a && ./a
