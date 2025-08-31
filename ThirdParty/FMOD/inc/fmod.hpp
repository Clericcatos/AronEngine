// Temporary FMOD header for compilation
// Replace this with actual FMOD SDK headers
#pragma once

#ifndef _FMOD_HPP
#define _FMOD_HPP

#include "fmod_common.h"

namespace FMOD
{
    class System;
    class Sound;
    class Channel;
    class ChannelGroup;
    class DSP;
    class DSPConnection;
    class Geometry;
    class Reverb3D;
    class CommandReplay;

    // FMOD System creation
    FMOD_RESULT System_Create(System **system, unsigned int headerversion = FMOD_VERSION);
}

// FMOD Constants
#define FMOD_INIT_NORMAL 0x00000000
#define FMOD_DEFAULT 0x00000000
#define FMOD_LOOP_OFF 0x00000001
#define FMOD_LOOP_NORMAL 0x00000002
#define FMOD_2D 0x00000008
#define FMOD_3D 0x00000010
#define FMOD_CREATESTREAM 0x00000080
#define FMOD_CREATESAMPLE 0x00000100
#define FMOD_OPENUSER 0x00000400
#define FMOD_OPENMEMORY 0x00000800
#define FMOD_OPENRAW 0x00001000
#define FMOD_OPENONLY 0x00002000
#define FMOD_ACCURATETIME 0x00004000
#define FMOD_MPEGSEARCH 0x00008000
#define FMOD_NONBLOCKING 0x00010000
#define FMOD_UNIQUE 0x00020000

// Time units
enum FMOD_TIMEUNIT
{
    FMOD_TIMEUNIT_MS = 0x00000001,
    FMOD_TIMEUNIT_PCM = 0x00000002,
    FMOD_TIMEUNIT_PCMBYTES = 0x00000004,
    FMOD_TIMEUNIT_RAWBYTES = 0x00000008,
    FMOD_TIMEUNIT_PCMFRACTION = 0x00000010,
    FMOD_TIMEUNIT_MODORDER = 0x00000100,
    FMOD_TIMEUNIT_MODROW = 0x00000200,
    FMOD_TIMEUNIT_MODPATTERN = 0x00000400
};

// Sound format
enum FMOD_SOUND_FORMAT
{
    FMOD_SOUND_FORMAT_NONE,
    FMOD_SOUND_FORMAT_PCM8,
    FMOD_SOUND_FORMAT_PCM16,
    FMOD_SOUND_FORMAT_PCM24,
    FMOD_SOUND_FORMAT_PCM32,
    FMOD_SOUND_FORMAT_PCMFLOAT,
    FMOD_SOUND_FORMAT_MAX
};

// Sound type
enum FMOD_SOUND_TYPE
{
    FMOD_SOUND_TYPE_UNKNOWN,
    FMOD_SOUND_TYPE_AIFF,
    FMOD_SOUND_TYPE_ASF,
    FMOD_SOUND_TYPE_DLS,
    FMOD_SOUND_TYPE_FLAC,
    FMOD_SOUND_TYPE_FSB,
    FMOD_SOUND_TYPE_IT,
    FMOD_SOUND_TYPE_MIDI,
    FMOD_SOUND_TYPE_MOD,
    FMOD_SOUND_TYPE_MPEG,
    FMOD_SOUND_TYPE_OGGVORBIS,
    FMOD_SOUND_TYPE_PLAYLIST,
    FMOD_SOUND_TYPE_RAW,
    FMOD_SOUND_TYPE_S3M,
    FMOD_SOUND_TYPE_USER,
    FMOD_SOUND_TYPE_WAV,
    FMOD_SOUND_TYPE_XM,
    FMOD_SOUND_TYPE_XMA,
    FMOD_SOUND_TYPE_AUDIOQUEUE,
    FMOD_SOUND_TYPE_AT9,
    FMOD_SOUND_TYPE_VORBIS,
    FMOD_SOUND_TYPE_MEDIA_FOUNDATION,
    FMOD_SOUND_TYPE_MEDIACODEC,
    FMOD_SOUND_TYPE_FADPCM,
    FMOD_SOUND_TYPE_OPUS,
    FMOD_SOUND_TYPE_MAX
};

// FMOD System class (declarations only)
namespace FMOD
{
    class System
    {
    public:
        FMOD_RESULT release();
        FMOD_RESULT init(int maxchannels, unsigned int flags, void *extradriverdata);
        FMOD_RESULT update();
        FMOD_RESULT set3DSettings(float dopplerscale, float distancefactor, float rolloffscale);
        FMOD_RESULT get3DListenerAttributes(int listener, FMOD_VECTOR *pos, FMOD_VECTOR *vel, FMOD_VECTOR *forward, FMOD_VECTOR *up);
        FMOD_RESULT set3DListenerAttributes(int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
        FMOD_RESULT createSound(const char *name_or_data, int mode, void *exinfo, Sound **sound);
        FMOD_RESULT playSound(Sound *sound, ChannelGroup *channelgroup, bool paused, Channel **channel);
        FMOD_RESULT getMasterChannelGroup(ChannelGroup **channelgroup);
        FMOD_RESULT getVersion(unsigned int *version);
    };

    class Sound
    {
    public:
        FMOD_RESULT release();
        FMOD_RESULT getLength(unsigned int *length, FMOD_TIMEUNIT lengthtype);
        FMOD_RESULT getFormat(FMOD_SOUND_TYPE *type, FMOD_SOUND_FORMAT *format, int *channels, int *bits);
        FMOD_RESULT getDefaults(float *frequency, int *priority);
    };

    class Channel
    {
    public:
        FMOD_RESULT stop();
        FMOD_RESULT setPaused(bool paused);
        FMOD_RESULT setVolume(float volume);
        FMOD_RESULT setPitch(float pitch);
        FMOD_RESULT setMode(unsigned int mode);
        FMOD_RESULT setPriority(int priority);
        FMOD_RESULT isPlaying(bool *isplaying);
        FMOD_RESULT getPosition(unsigned int *position, FMOD_TIMEUNIT postype);
        FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
        FMOD_RESULT set3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
        FMOD_RESULT set3DMinMaxDistance(float mindistance, float maxdistance);
        FMOD_RESULT set3DLevel(float level);
    };

    class ChannelGroup
    {
    public:
        FMOD_RESULT setVolume(float volume);
        FMOD_RESULT getVolume(float *volume);
    };
}

#endif