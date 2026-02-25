#include "SoundBuffer.h"
#include <inttypes.h>
#include <sndfile.h>
#include <iostream>

#define AL_FORMAT_BFORMAT2D_16 0x20022
#define AL_FORMAT_BFORMAT3D_16 0x20032
enum FormatType { Int16, Float, IMA4, MSADPCM };

//singleton
SoundBuffer *SoundBuffer::get() {
  static SoundBuffer *sndbuf = new SoundBuffer();
  return sndbuf;
}

// taken from the openal-soft examples github repo
// we need to load a sound from file detecting the correct
// format to upload to the sound buffer returning the ALuint
// to that buffer
ALuint SoundBuffer::addSoundEffect(const char *filename) {
  enum FormatType sample_format = Int16;
  ALint byteblockalign = 0;
  ALint splblockalign = 0;
  sf_count_t num_frames;
  ALenum err;
  ALenum format;
  ALsizei num_bytes;
  SNDFILE *sndfile;
  SF_INFO sfinfo;
  ALuint buffer;
  short *membuf;

  // we need to open the audio file nad check that its useable
  sndfile = sf_open(filename, SFM_READ, &sfinfo);
  if (!sndfile) {
    fprintf(stderr, "COuld not open audio in %s: %s\n", filename,
            sf_strerror(sndfile));
    return 0;
  }

  if (sfinfo.frames < 1) {
    fprintf(stderr, "Bad sample count inn %s (%" PRId64 ")\n", filename,
            sfinfo.frames);
    sf_close(sndfile);
    return 0;
  }

  // we need to detect a format to load

  format = AL_NONE;
  if (sfinfo.channels == 1)
    format = AL_FORMAT_MONO16;
  else if (sfinfo.channels == 2)
    format = AL_FORMAT_STEREO16;
  else if (sfinfo.channels == 3) {
    if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) ==
        SF_AMBISONIC_B_FORMAT) {
      format = AL_FORMAT_BFORMAT2D_16;
    }
  } else if (sfinfo.channels == 4) {
    if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) ==
        SF_AMBISONIC_B_FORMAT)
      format = AL_FORMAT_BFORMAT3D_16;
  }
  if (!format) {
    fprintf(stderr, "Unsuported channel count: %d\n", sfinfo.channels);
    sf_close(sndfile);
    return 0;
  }

  membuf = static_cast<short *>(
      malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

  num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
  if (num_frames < 1) {
    free(membuf);
    sf_close(sndfile);
    fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename,
            num_frames);
    return 0;
  }
  num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

  // we need to buffer the audio into a buffer object and close the file as well
  // as free the data
  buffer = 0;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

  free(membuf);
  sf_close(sndfile);

  err = alGetError();
  if (err != AL_NO_ERROR) {
    fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
    if (buffer && alIsBuffer(buffer))
      alDeleteBuffers(1, &buffer);
    return 0;
  }

	//we need to push the buffer passed to our vector 
	//of buffers
  p_SoundEffectBuffers.push_back(buffer);
  return buffer;
}

//we need this function to remove a buffer from the current
//vector of sound buffers as well as delete it from the openal
//buffer
bool SoundBuffer::removeSoundEffect(const ALuint &buffer) {
  auto it = p_SoundEffectBuffers.begin();
  while (it != p_SoundEffectBuffers.end()) {
    if (*it == buffer) {
      alDeleteBuffers(1, &*it);

      it = p_SoundEffectBuffers.erase(it);

      return true;
    } else {
      ++it;
    }
  }
  return false;
}

SoundBuffer::SoundBuffer() { p_SoundEffectBuffers.clear(); }

SoundBuffer::~SoundBuffer() {
  alDeleteBuffers(p_SoundEffectBuffers.size(), p_SoundEffectBuffers.data());
  p_SoundEffectBuffers.clear();
}
