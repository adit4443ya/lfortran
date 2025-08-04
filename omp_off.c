#include <inttypes.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <lfortran_intrinsics.h>


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




// Implementations
int main(int argc, char* argv[])
{
    _lpython_set_argv(argc, argv);
    int32_t __libasr_index_0_;
    struct r32 a_value;
    struct r32* a = &a_value;
    float *a_data;
    a->data = a_data;
    a->n_dims = 1;
    a->offset = 0;
    a->dims[0].lower_bound = 1;
    a->dims[0].length = 0;
    a->dims[0].stride = 1;
    struct r32 b_value;
    struct r32* b = &b_value;
    float *b_data;
    b->data = b_data;
    b->n_dims = 1;
    b->offset = 0;
    b->dims[0].lower_bound = 1;
    b->dims[0].length = 0;
    b->dims[0].stride = 1;
    int32_t i;
    a->n_dims = 1;
    a->dims[0].lower_bound = 1;
    a->dims[0].length = 10000000;
    a->dims[0].stride = 1;
    a->data = (float*) _lfortran_malloc(1*a->dims[0].length*sizeof(float));
    a->is_allocated = true;
    b->n_dims = 1;
    b->dims[0].lower_bound = 1;
    b->dims[0].length = 10000000;
    b->dims[0].stride = 1;
    b->data = (float*) _lfortran_malloc(1*b->dims[0].length*sizeof(float));
    b->is_allocated = true;
    for (__libasr_index_0_=((int32_t)b->dims[1-1].lower_bound); __libasr_index_0_<=((int32_t) b->dims[1-1].length + b->dims[1-1].lower_bound - 1); __libasr_index_0_++) {
        b->data[((0 + (b->dims[0].stride * (__libasr_index_0_ - b->dims[0].lower_bound))) + b->offset)] = (float)(5);
    }
#pragma omp target  map(tofrom: a->data[0:a->dims[0].length-1]) map(to: a->dims[0].lower_bound, a->dims[0].length, a->dims[0].stride) map(to: a->n_dims) map(from: a->offset) map(tofrom: b->data[0:b->dims[0].length-1]) map(to: b->dims[0].lower_bound, b->dims[0].length, b->dims[0].stride) map(to: b->n_dims) map(from: b->offset)
#pragma omp teams 
#pragma omp distribute parallel for 
    for (i=1; i<=10000000; i++) {
        a->data[((0 + (a->dims[0].stride * (i - a->dims[0].lower_bound))) + a->offset)] = (float)(i) + b->data[((0 + (b->dims[0].stride * (i - b->dims[0].lower_bound))) + b->offset)]*(float)(340);
    }



    printf("%f%s%f\n", a->data[((0 + (a->dims[0].stride * (5 - a->dims[0].lower_bound))) + a->offset)], " ", b->data[((0 + (b->dims[0].stride * (5 - b->dims[0].lower_bound))) + b->offset)]);
    if (a->data[((0 + (a->dims[0].stride * (5 - a->dims[0].lower_bound))) + a->offset)] != (float)(1705)) {
        fprintf(stderr, "ERROR STOP");
        exit(1);
    }
    if (b->data[((0 + (b->dims[0].stride * (5 - b->dims[0].lower_bound))) + b->offset)] != (float)(5)) {
        fprintf(stderr, "ERROR STOP");
        exit(1);
    }
    // FIXME: implicit deallocate(a, b, );
    return 0;
}