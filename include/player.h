#ifndef MIPLAYER
#define MIPLAYER
#include <audio.h>
#include <fft.h>
// TODO: Make it so the player can load not only one audio file.
// but it can get all the files that are present in the same directory and are audio files.
typedef struct {
	// FileBrowser_t fb;
	BladeAudio   audio;
	char 	  file[512];
	float     volume;
	uint8_t   play;
	uint8_t   quit;
	ma_device dev;
	pthread_mutex_t Mutx; // a mutex to synchronize the plyer and visualizer.
} BladeAudioPlayer;

// NOTE: This api will abstract the miniaudio api and make it more easy to work with.
BladeAudioPlayer *init_player(char *file);
void deallocate_player(BladeAudioPlayer *player);
void start_playing(BladeAudioPlayer *player);
void player_play(BladeAudioPlayer *player);
void toggle_play(BladeAudioPlayer *player);
void player_pause(BladeAudioPlayer *player);
void seek_position(BladeAudioPlayer *player, int offset_in_secs);
void rewind_audio(BladeAudioPlayer *player);
void *player_get_input(void *player_data); // this function gets the player in a seperate thread and it get input from the user
// and makes any neccessiry adjusments.
void volume(BladeAudioPlayer *player, float new_volume); // apply a new volume if it is provided.
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount); // This function send loaded data from the BladeAudio samples to the device.
// doing any neccessiry computations fft(...) dft(...) volume(...)

void *player_visualize_audio(void *E); // this function is used to visualize the frames using ncurses in a seperate thread. it is like 
// the entry point of visualization.
// synch/mutex
void DistroyPlayerMutex(void);
void LockPlayer(void);
void unLockPlayer(void);
void InitPlayerLock(void);
#endif // MIPLAYER
