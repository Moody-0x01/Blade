#include <fft.h>
#include <blade.h>

double get_frames_as_seconds(ma_decoder *decoder, ma_uint64 frame) {
	return (double)((double)frame / decoder->outputSampleRate);
}

void init_audio(BladeAudio *audio) {

	ma_decoder_get_length_in_pcm_frames(&audio->decoder, &audio->totalFrames);
	size_t N = (audio->totalFrames * audio->decoder.outputChannels);
	audio->samples = (float *)malloc(sizeof(float) * N);
	ma_uint64 read = 0;
	ma_decoder_read_pcm_frames(&audio->decoder, audio->samples,
							audio->totalFrames, &read);
	audio->spec_sz  = 0;
	assert(read == audio->totalFrames);
	audio->duration = get_frames_as_seconds(
		&audio->decoder, audio->totalFrames * audio->decoder.outputChannels);
}
