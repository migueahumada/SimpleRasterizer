__kernel void vectorAdd(__global const float* A,
                        __global const float* B,
                        __global float* C,
                        const unsigned int numElements)
{
  int i = get_global_id(0);
  C[i] = A[i] + B[i];
}

__kernel void vectorSub(__global const float* A,
                        __global const float* B,
                        __global float* C,
                        const unsigned int numElements)
{
  int i = get_global_id(0);
  C[i] = A[i] - B[i];

}