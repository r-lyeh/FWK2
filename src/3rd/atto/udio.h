#ifndef ATTO_UDIO_H__DECLARED
#define ATTO_UDIO_H__DECLARED

#ifndef ATTO_UDIO_SAMPLERATE
	#define ATTO_UDIO_SAMPLERATE 48000
#endif

#ifndef ATTO_UDIO_CHANNELS
	#define ATTO_UDIO_CHANNELS 2
#endif

typedef void (*a_udio_callback_f)(void *opaque, unsigned int sample, float *samples, unsigned int nsamples);

int aUdioStart(a_udio_callback_f callback, void *opaque);
unsigned int aUdioPosition();
void aUdioStop();

#endif /* ifndef ATTO_UDIO_H__DECLARED */

#ifdef ATTO_UDIO_H_IMPLEMENT

#ifndef ATTO_PLATFORM
	#error ATTO_PLATFORM must be already defined
#endif

#ifdef ATTO_UDIO_H__IMPLEMENTED
	#error atto_udio.h can be implemented only once
#endif /* ifdef ATTO_UDIO_H__IMPLEMENTED */
#define ATTO_UDIO_H__IMPLEMENTED

#ifndef ATTO_UDIO_BUFFER_SAMPLES
	#define ATTO_UDIO_BUFFER_SAMPLES 512
#endif

#ifdef ATTO_PLATFORM_LINUX
	/* Use pulseaudio simple api */
	#include <pthread.h>
	/* pulseaudio, y u no c89?! */
	#define inline
	#include <pulse/simple.h>
	#undef inline

static pthread_mutex_t a__udio_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t a__udio_thread;
static void *a__udio_opaque = 0;
static a_udio_callback_f a__udio_callback = 0;
static unsigned int a__udio_sample = 0;

static void *a__udio_thread_main(void *arg) {
	pa_simple *s;
	pa_sample_spec ss;

	pthread_mutex_lock(&a__udio_mutex);
	ss.format = PA_SAMPLE_FLOAT32NE;
	ss.channels = ATTO_UDIO_CHANNELS;
	ss.rate = ATTO_UDIO_SAMPLERATE;
	s = pa_simple_new(NULL, ATTO_APP_NAME, PA_STREAM_PLAYBACK, NULL, "m", &ss, NULL, NULL, NULL);

	for (;;) {
		float buffer[ATTO_UDIO_BUFFER_SAMPLES * ATTO_UDIO_CHANNELS];

		if (a__udio_callback == 0) {
			pthread_mutex_unlock(&a__udio_mutex);
			break;
		}
		a__udio_callback(a__udio_opaque, a__udio_sample, buffer, ATTO_UDIO_BUFFER_SAMPLES);
		pthread_mutex_unlock(&a__udio_mutex);

		pa_simple_write(s, buffer, sizeof(buffer), NULL);

		pthread_mutex_lock(&a__udio_mutex);
		a__udio_sample += ATTO_UDIO_BUFFER_SAMPLES;
	}

	pa_simple_flush(s, NULL);
	pa_simple_free(s);

	return 0;
}

int aUdioStart(a_udio_callback_f callback, void *opaque) {
	int result;

	pthread_mutex_lock(&a__udio_mutex);
	if (a__udio_callback != 0)
		return -1;

	a__udio_sample = 0;
	result = pthread_create(&a__udio_thread, NULL, a__udio_thread_main, 0);
	if (result != 0)
		goto exit;

	a__udio_callback = callback;
	a__udio_opaque = opaque;

exit:
	pthread_mutex_unlock(&a__udio_mutex);
	return result;
}

unsigned int aUdioPosition() {
	unsigned int ret;
	pthread_mutex_lock(&a__udio_mutex);
	ret = a__udio_sample;
	pthread_mutex_unlock(&a__udio_mutex);
	return ret;
}

void aUdioStop() {
	pthread_mutex_lock(&a__udio_mutex);

	if (a__udio_callback == 0) {
		pthread_mutex_unlock(&a__udio_mutex);
		return;
	}

	a__udio_callback = 0;
	pthread_mutex_unlock(&a__udio_mutex);

	pthread_join(a__udio_thread, NULL);
}

#endif

#ifdef ATTO_PLATFORM_WINDOWS
	#include <mmsystem.h>
	#include <mmreg.h>

static struct {
	HWAVEOUT waveout;
	HANDLE thread, event;
	a_udio_callback_f callback;
	void *opaque;
} a__udio = {0};


static DWORD WINAPI a__udio_thread(LPVOID lpParameter) {
	float buffer[ATTO_UDIO_BUFFER_SAMPLES * ATTO_UDIO_CHANNELS];
	unsigned int sample = 0;
	MMRESULT result;
	WAVEHDR whdr[2] = {{0}, {0}};
	whdr[0].lpData = buffer;
	whdr[1].lpData = buffer + ATTO_UDIO_BUFFER_SAMPLES * ATTO_UDIO_CHANNELS / 2;
	whdr[0].dwBufferLength = whdr[1].dwBufferLength = sizeof(buffer) / 2;
	printf("audio thread started\n");
	while (WAIT_OBJECT_0 == WaitForSingleObject(a__udio.event, INFINITE)) {
		for (int i = 0; i < 2; ++i) {
			if (whdr[i].dwFlags & WHDR_DONE) {
				whdr[i].dwFlags = 0;
				result = waveOutUnprepareHeader(a__udio.waveout, whdr + i, sizeof(*whdr));
				if (result != MMSYSERR_NOERROR) {
					fprintf(stderr, "waveOutUnprepareHeader: %d\n", result);
					continue;
				}
			}
			if (!(whdr[i].dwFlags & WHDR_PREPARED)) {
				a__udio.callback(a__udio.opaque, sample, whdr[i].lpData, ATTO_UDIO_BUFFER_SAMPLES / 2);
				sample += ATTO_UDIO_BUFFER_SAMPLES / 2;

				result = waveOutPrepareHeader(a__udio.waveout, whdr + i, sizeof(*whdr));
				if (result != MMSYSERR_NOERROR) {
					fprintf(stderr, "waveOutPrepareHeader: %d\n", result);
					continue;
				}

				result = waveOutWrite(a__udio.waveout, whdr + i, sizeof(*whdr));
				if (result != MMSYSERR_NOERROR) {
					fprintf(stderr, "waveOutWrite: %d\n", result);
					continue;
				}
			}
		}
	}
	printf("audio thread finished\n");
	return 0;
}

int aUdioStart(a_udio_callback_f callback, void *opaque) {
	if (a__udio.waveout)
		return -1;

	WAVEFORMATEX wvfmt;
	wvfmt.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	wvfmt.nChannels = ATTO_UDIO_CHANNELS;
	wvfmt.nSamplesPerSec = ATTO_UDIO_SAMPLERATE;
	wvfmt.wBitsPerSample = 32;
	wvfmt.nBlockAlign = wvfmt.wBitsPerSample * wvfmt.nChannels / 8;
	wvfmt.nAvgBytesPerSec = wvfmt.nSamplesPerSec * wvfmt.nBlockAlign;
	wvfmt.cbSize = 0;

	a__udio.event = CreateEvent(NULL, FALSE, TRUE, "AttoUdio");

	MMRESULT result = waveOutOpen(&a__udio.waveout, WAVE_MAPPER, &wvfmt, a__udio.event, 0, CALLBACK_EVENT);
	if (result != MMSYSERR_NOERROR) {
		fprintf(stderr, "waveOutOpen: %d\n", result);
		return -2;
	}

	a__udio.callback = callback;

	a__udio.thread = CreateThread(NULL, 0, a__udio_thread, 0, 0, NULL);

	SetEvent(a__udio.event);

	return 0;
}

unsigned int aUdioPosition() {
	if (!a__udio.waveout)
		return 0;

	MMTIME mmtime;
	mmtime.wType = TIME_SAMPLES;
	waveOutGetPosition(a__udio.waveout, &mmtime, sizeof(mmtime));
	return (float)(mmtime.u.sample * 1000.f) / (float)ATTO_UDIO_SAMPLERATE;
}

void aUdioStop() {
	/* TODO */
}
#endif /* ifdef ATTO_PLATFORM_WINDOWS */


#endif /* ifdef ATTO_UDIO_H_IMPLEMENT */
