#ifndef MIAUDIO
#define MIAUDIO
#include <miniaudio/miniaudio.h>
#include <fft.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <assert.h>
#include <sys/wait.h>
#include <complex.h>
#include <math.h>
// const double PI = acos(-1);

#define BARS 127
#define NSAMPLES 1024
#define FFT_SIZE NSAMPLES
typedef struct {
	ma_decoder decoder;
	ma_uint64  position;
	ma_uint64  srate;
	float 	   *samples;
	ma_uint64  totalFrames;
	double     duration;
	uint32_t   framecount;
	TComplexF fft_[FFT_SIZE]; // Needs to be loaded each frame.
	TComplexF Input[FFT_SIZE];
	float fft_abs[FFT_SIZE];
	float spectrum[FFT_SIZE];     // Needs to be loaded each frame.
	size_t spec_sz;
} BladeAudio;

void   init_audio(BladeAudio *audio);
double get_frames_as_seconds(ma_decoder *decoder, ma_uint64 frame);

#endif // MIAUDIO
