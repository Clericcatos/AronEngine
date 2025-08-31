// Temporary FMOD errors header for compilation
#pragma once

#ifndef _FMOD_ERRORS_H
#define _FMOD_ERRORS_H

#include "fmod_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Error string function
static const char *FMOD_ErrorString(FMOD_RESULT errcode)
{
    switch (errcode)
    {
        case FMOD_OK:                            return "No errors.";
        case FMOD_ERR_BADCOMMAND:                return "Tried to call a function on a data type that does not allow this type of functionality (ie calling Sound::lock on a streaming sound).";
        case FMOD_ERR_CHANNEL_ALLOC:             return "Error trying to allocate a channel.";
        case FMOD_ERR_CHANNEL_STOLEN:            return "The specified channel has been reused to play another sound.";
        case FMOD_ERR_DMA:                       return "DMA Failure.";
        case FMOD_ERR_DSP_CONNECTION:            return "DSP connection error.";
        case FMOD_ERR_DSP_DONTPROCESS:           return "DSP return code from a DSP process query callback.";
        case FMOD_ERR_DSP_FORMAT:                return "DSP Format error.";
        case FMOD_ERR_DSP_INUSE:                 return "DSP is already in the mixer's DSP chain.";
        case FMOD_ERR_DSP_NOTFOUND:              return "DSP connection error.";
        case FMOD_ERR_DSP_RESERVED:              return "DSP reserved.";
        case FMOD_ERR_DSP_SILENCE:               return "DSP did not return any data.";
        case FMOD_ERR_DSP_TYPE:                  return "DSP tried to connect to this DSP but the types did not match.";
        case FMOD_ERR_FILE_BAD:                  return "Error loading file.";
        case FMOD_ERR_FILE_COULDNOTSEEK:         return "Couldn't perform seek operation.";
        case FMOD_ERR_FILE_DISKEJECTED:          return "Media was ejected while reading.";
        case FMOD_ERR_FILE_EOF:                  return "End of file unexpectedly reached while trying to read essential data (truncated?).";
        case FMOD_ERR_FILE_ENDOFDATA:            return "End of current chunk reached while trying to read data.";
        case FMOD_ERR_FILE_NOTFOUND:             return "File not found.";
        case FMOD_ERR_FORMAT:                    return "Unsupported file or audio format.";
        case FMOD_ERR_HEADER_MISMATCH:           return "There is a version mismatch between the FMOD header and either the FMOD Studio library or the FMOD Low Level library.";
        case FMOD_ERR_HTTP:                      return "A HTTP error occurred.";
        case FMOD_ERR_HTTP_ACCESS:               return "The HTTP request failed because it was not allowed by the server.";
        case FMOD_ERR_HTTP_PROXY_AUTH:           return "Proxy authentication is required to access the internet resource.";
        case FMOD_ERR_HTTP_SERVER_ERROR:         return "A HTTP server error occurred.";
        case FMOD_ERR_HTTP_TIMEOUT:              return "The HTTP request timed out.";
        case FMOD_ERR_INITIALIZATION:            return "FMOD was not initialized correctly.";
        case FMOD_ERR_INITIALIZED:               return "Cannot call this command after System::init.";
        case FMOD_ERR_INTERNAL:                  return "An error occurred that wasn't supposed to.";
        case FMOD_ERR_INVALID_FLOAT:             return "Value passed in was a NaN, Inf or denormalized float.";
        case FMOD_ERR_INVALID_HANDLE:            return "An invalid object handle was used.";
        case FMOD_ERR_INVALID_PARAM:             return "An invalid parameter was passed to this function.";
        case FMOD_ERR_INVALID_POSITION:          return "An invalid seek position was passed to this function.";
        case FMOD_ERR_INVALID_SPEAKER:           return "An invalid speaker was passed to this function.";
        case FMOD_ERR_INVALID_SYNCPOINT:         return "The syncpoint did not come from this sound handle.";
        case FMOD_ERR_INVALID_THREAD:            return "Tried to call a function on a thread that is not supported.";
        case FMOD_ERR_INVALID_VECTOR:            return "The vectors passed in are not unit length, or perpendicular.";
        case FMOD_ERR_MAXAUDIBLE:                return "Reached maximum audible playback count for this sound's soundgroup.";
        case FMOD_ERR_MEMORY:                    return "Not enough memory or resources.";
        case FMOD_ERR_MEMORY_CANTPOINT:          return "Can't use FMOD_OPENMEMORY_POINT on non PCM source data.";
        case FMOD_ERR_NEEDS3D:                   return "Tried to call a command on a 2d sound when the command was meant for 3d sound.";
        case FMOD_ERR_NEEDSHARDWARE:             return "Tried to use a feature that requires hardware support.";
        case FMOD_ERR_NET_CONNECT:               return "Couldn't connect to the specified host.";
        case FMOD_ERR_NET_SOCKET_ERROR:          return "A socket error occurred.";
        case FMOD_ERR_NET_URL:                   return "The specified URL couldn't be resolved.";
        case FMOD_ERR_NET_WOULD_BLOCK:           return "Operation on a non-blocking socket could not complete immediately.";
        case FMOD_ERR_NOTREADY:                  return "Operation could not be performed because specified sound/DSP connection is not ready.";
        case FMOD_ERR_OUTPUT_ALLOCATED:          return "Error initializing output device.";
        case FMOD_ERR_OUTPUT_CREATEBUFFER:       return "Error creating hardware sound buffer.";
        case FMOD_ERR_OUTPUT_DRIVERCALL:         return "A call to a standard soundcard driver failed.";
        case FMOD_ERR_OUTPUT_FORMAT:             return "Soundcard does not support the specified format.";
        case FMOD_ERR_OUTPUT_INIT:               return "Error initializing output device.";
        case FMOD_ERR_OUTPUT_NODRIVERS:          return "The output device has no drivers installed.";
        case FMOD_ERR_PLUGIN:                    return "An unspecified error has been returned from a plugin.";
        case FMOD_ERR_PLUGIN_MISSING:            return "A requested output, dsp unit type or codec was not available.";
        case FMOD_ERR_PLUGIN_RESOURCE:           return "A resource that the plugin requires cannot be found.";
        case FMOD_ERR_PLUGIN_VERSION:            return "A plugin was built with an unsupported SDK version.";
        case FMOD_ERR_RECORD:                    return "An error occurred trying to initialize the recording device.";
        case FMOD_ERR_REVERB_CHANNELGROUP:       return "Reverb properties cannot be set on this channel because a parent channelgroup owns the reverb connection.";
        case FMOD_ERR_REVERB_INSTANCE:           return "Specified reverb instance has not been created.";
        case FMOD_ERR_SUBSOUNDS:                 return "The error occurred because the sound referenced contains subsounds.";
        case FMOD_ERR_SUBSOUND_ALLOCATED:        return "This subsound is already being used by another sound.";
        case FMOD_ERR_SUBSOUND_CANTMOVE:         return "Shared subsounds cannot be replaced or moved from their parent stream.";
        case FMOD_ERR_TAGNOTFOUND:               return "The specified tag could not be found or there are no tags.";
        case FMOD_ERR_TOOMANYCHANNELS:           return "The sound created exceeds the allowable input channel count.";
        case FMOD_ERR_TRUNCATED:                 return "The retrieved string is too long to fit in the supplied buffer and has been truncated.";
        case FMOD_ERR_UNIMPLEMENTED:             return "Something in FMOD hasn't been implemented when it should be!";
        case FMOD_ERR_UNINITIALIZED:             return "This command failed because System::init or System::setDriver was not called.";
        case FMOD_ERR_UNSUPPORTED:               return "A command issued was not supported by this object.";
        case FMOD_ERR_VERSION:                   return "The version number of this file format is not supported.";
        case FMOD_ERR_EVENT_ALREADY_LOADED:      return "The specified bank has already been loaded.";
        case FMOD_ERR_EVENT_LIVEUPDATE_BUSY:     return "The live update connection failed due to the game already being connected.";
        case FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH: return "The live update connection failed due to the game data being out of sync with the tool.";
        case FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT:  return "The live update connection timed out.";
        case FMOD_ERR_EVENT_NOTFOUND:            return "The requested event, parameter, bus or vca could not be found.";
        case FMOD_ERR_STUDIO_UNINITIALIZED:      return "The Studio::System object is not yet initialized.";
        case FMOD_ERR_STUDIO_NOT_LOADED:         return "The specified resource is not loaded, so it can't be unloaded.";
        case FMOD_ERR_INVALID_STRING:            return "An invalid string was passed to this function.";
        case FMOD_ERR_ALREADY_LOCKED:            return "The specified resource is already locked.";
        case FMOD_ERR_NOT_LOCKED:                return "The specified resource is not locked.";
        case FMOD_ERR_RECORD_DISCONNECTED:       return "The specified recording driver has been disconnected.";
        case FMOD_ERR_TOOMANYSAMPLES:            return "The length provided exceeds the allowable limit.";
        default:                                 return "Unknown error.";
    }
}

#ifdef __cplusplus
}
#endif

#endif