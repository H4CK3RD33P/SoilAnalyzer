#pragma once
#include <cstdarg>
#include <cstdint>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class SVM {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        float kernels[18] = { 0 };
                        float decisions[6] = { 0 };
                        int votes[4] = { 0 };
                        kernels[0] = compute_kernel(x,   36.0  , 83.0  , 77.0 );
                        kernels[1] = compute_kernel(x,   43.0  , 80.0  , 70.0 );
                        kernels[2] = compute_kernel(x,   34.0  , 91.0  , 90.0 );
                        kernels[3] = compute_kernel(x,   20.0  , 50.0  , 71.0 );
                        kernels[4] = compute_kernel(x,   23.0  , 51.0  , 65.0 );
                        kernels[5] = compute_kernel(x,   22.0  , 48.0  , 70.0 );
                        kernels[6] = compute_kernel(x,   24.0  , 50.0  , 70.0 );
                        kernels[7] = compute_kernel(x,   40.0  , 86.0  , 78.0 );
                        kernels[8] = compute_kernel(x,   34.0  , 85.0  , 78.0 );
                        kernels[9] = compute_kernel(x,   32.0  , 75.0  , 75.0 );
                        kernels[10] = compute_kernel(x,   41.0  , 74.0  , 68.0 );
                        kernels[11] = compute_kernel(x,   37.0  , 88.0  , 62.0 );
                        kernels[12] = compute_kernel(x,   18.0  , 50.0  , 70.0 );
                        kernels[13] = compute_kernel(x,   22.0  , 50.0  , 72.0 );
                        kernels[14] = compute_kernel(x,   23.0  , 58.0  , 75.0 );
                        kernels[15] = compute_kernel(x,   21.0  , 60.0  , 60.0 );
                        kernels[16] = compute_kernel(x,   21.0  , 48.0  , 66.0 );
                        kernels[17] = compute_kernel(x,   25.0  , 55.0  , 71.0 );
                        decisions[0] = -4.463751885359
                        + kernels[0] * 0.000632452942
                        + kernels[1] * 0.00097590059
                        + kernels[4] * -5.8889829e-05
                        + kernels[6] * -0.001549463703
                        ;
                        decisions[1] = -32.925504453502
                        + kernels[0]
                        + kernels[1]
                        + kernels[2] * 0.178449283675
                        - kernels[7]
                        + kernels[8] * -0.499873201601
                        + kernels[10] * -0.650641501669
                        + kernels[11] * -0.027934580405
                        ;
                        decisions[2] = -5.759398496241
                        + kernels[0] * 0.002506265664
                        + kernels[14] * -0.002506265664
                        ;
                        decisions[3] = 6.019607843137
                        + kernels[6] * 0.002801120448
                        + kernels[9] * -0.002801120448
                        ;
                        decisions[4] = 25.257718914726
                        + kernels[3]
                        + kernels[4] * 0.526949636172
                        + kernels[5] * 0.11099611832
                        + kernels[6]
                        + kernels[12] * -0.098730189215
                        - kernels[13]
                        - kernels[16]
                        + kernels[17] * -0.539215565277
                        ;
                        decisions[5] = -7.857877818051
                        + kernels[9] * 0.005421389173
                        + kernels[14] * -0.005051764555
                        + kernels[15] * -0.000369624617
                        ;
                        votes[decisions[0] > 0 ? 0 : 1] += 1;
                        votes[decisions[1] > 0 ? 0 : 2] += 1;
                        votes[decisions[2] > 0 ? 0 : 3] += 1;
                        votes[decisions[3] > 0 ? 1 : 2] += 1;
                        votes[decisions[4] > 0 ? 1 : 3] += 1;
                        votes[decisions[5] > 0 ? 2 : 3] += 1;
                        int val = votes[0];
                        int idx = 0;

                        for (int i = 1; i < 4; i++) {
                            if (votes[i] > val) {
                                val = votes[i];
                                idx = i;
                            }
                        }

                        return idx;
                    }

                    /**
                    * Predict readable class name
                    */
                    const char* predictLabel(float *x) {
                        return idxToLabel(predict(x));
                    }

                    /**
                    * Convert class idx to readable name
                    */
                    const char* idxToLabel(uint8_t classIdx) {
                        switch (classIdx) {
                            case 0:
                            return "Rice";
                            case 1:
                            return "Wheat";
                            case 2:
                            return "Bajra";
                            case 3:
                            return "Gram";
                            default:
                            return "Houston we have a problem";
                        }
                    }

                protected:
                    /**
                    * Compute kernel between feature vector and support vector.
                    * Kernel type: linear
                    */
                    float compute_kernel(float *x, ...)
                    {
                        va_list w;
                        va_start(w, x);
                        float kernel = 0.0;

                        for (uint16_t i = 0; i < 3; i++)
                        {
                            kernel += x[i] * va_arg(w, double);
                        }

                        va_end(w);
                        return kernel;
                    }
                };
            }
        }
    }
