/*
 * harmonic_fft.h
 *
 *  Created on: 9Sep.,2018
 *      Author: Joseph
 */

#ifndef APPLICATION_USER_CORE_HARMONIC_FFT_H_
#define APPLICATION_USER_CORE_HARMONIC_FFT_H_

#include "main.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define FFT_SAMPLE_LENGTH 6400
#define FFT_BINS (FFT_SAMPLE_LENGTH / 2)

extern float32_t fftOutput[FFT_SAMPLE_LENGTH / 2];
extern float32_t fftMax;
extern uint32_t fftMaxIndex;

void fft_calculate();

#endif /* APPLICATION_USER_CORE_HARMONIC_FFT_H_ */
