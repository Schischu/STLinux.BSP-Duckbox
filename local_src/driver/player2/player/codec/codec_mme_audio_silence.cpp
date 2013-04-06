/************************************************************************
Copyright (C) 2007 STMicroelectronics. All Rights Reserved.

This file is part of the Player2 Library.

Player2 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by the
Free Software Foundation.

Player2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with player2; see the file COPYING.  If not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

The Player2 Library may alternatively be licensed under a proprietary
license from ST.

Source file name : codec_mme_audio_silence.cpp
Author :           Daniel

Implementation of the mpeg2 audio codec class for player 2.


Date        Modification                                    Name
----        ------------                                    --------
19-Mar-08   Created (from codec_mme_audio_mpeg.cpp)         Daniel

************************************************************************/

////////////////////////////////////////////////////////////////////////////
/// \class Codec_MmeAudioSilence_c
///
/// A silence generating audio 'codec' to replace the main codec when licensing is insufficient.
///

// /////////////////////////////////////////////////////////////////////
//
//      Include any component headers

#include "codec_mme_audio_silence.h"
#include "codec_mme_audio_dtshd.h"
#include "player_generic.h"

// /////////////////////////////////////////////////////////////////////////
//
// Locally defined constants
//
// /////////////////////////////////////////////////////////////////////////
//
// Locally defined structures
//

// /////////////////////////////////////////////////////////////////////////
//
// Locally defined structures
//

typedef struct SilentAudioCodecStreamParameterContext_s
{
    CodecBaseStreamParameterContext_t   BaseContext;
} SilentAudioCodecStreamParameterContext_t;

//#if __KERNEL__
#if 0
#define BUFFER_SILENT_AUDIO_CODEC_STREAM_PARAMETER_CONTEXT                "SilentAudioCodecStreamParameterContext"
#define BUFFER_SILENT_AUDIO_CODEC_STREAM_PARAMETER_CONTEXT_TYPE   {BUFFER_SILENT_AUDIO_CODEC_STREAM_PARAMETER_CONTEXT, BufferDataTypeBase, AllocateFromDeviceMemory, 32, 0, true, true, sizeof(SilentAudioCodecStreamParameterContext_t)}
#else
#define BUFFER_SILENT_AUDIO_CODEC_STREAM_PARAMETER_CONTEXT                "SilentAudioCodecStreamParameterContext"
#define BUFFER_SILENT_AUDIO_CODEC_STREAM_PARAMETER_CONTEXT_TYPE   {BUFFER_SILENT_AUDIO_CODEC_STREAM_PARAMETER_CONTEXT, BufferDataTypeBase, AllocateFromOSMemory, 32, 0, true, true, sizeof(SilentAudioCodecStreamParameterContext_t)}
#endif

static BufferDataDescriptor_t            SilentAudioCodecStreamParameterContextDescriptor = BUFFER_SILENT_AUDIO_CODEC_STREAM_PARAMETER_CONTEXT_TYPE;

// --------

typedef union SilentAudioFrameParameters_s
{
    void*                       OtherAudioFrameParameters;
    DtshdAudioFrameParameters_t DtshdAudioFrameParameters;
} SilentAudioFrameParameters_t;

typedef struct SilentAudioCodecDecodeContext_s
{
    CodecBaseDecodeContext_t            BaseContext;
    unsigned int                        TranscodeBufferIndex;
    SilentAudioFrameParameters_t        ContextFrameParameters;
} SilentAudioCodecDecodeContext_t;

//#if __KERNEL__
#if 0
#define BUFFER_SILENT_AUDIO_CODEC_DECODE_CONTEXT  "SilentAudioCodecDecodeContext"
#define BUFFER_SILENT_AUDIO_CODEC_DECODE_CONTEXT_TYPE     {BUFFER_SILENT_AUDIO_CODEC_DECODE_CONTEXT, BufferDataTypeBase, AllocateFromDeviceMemory, 32, 0, true, true, sizeof(SilentAudioCodecDecodeContext_t)}
#else
#define BUFFER_SILENT_AUDIO_CODEC_DECODE_CONTEXT  "SilentAudioCodecDecodeContext"
#define BUFFER_SILENT_AUDIO_CODEC_DECODE_CONTEXT_TYPE     {BUFFER_SILENT_AUDIO_CODEC_DECODE_CONTEXT, BufferDataTypeBase, AllocateFromOSMemory, 32, 0, true, true, sizeof(SilentAudioCodecDecodeContext_t)}
#endif

static BufferDataDescriptor_t            SilentAudioCodecDecodeContextDescriptor = BUFFER_SILENT_AUDIO_CODEC_DECODE_CONTEXT_TYPE;

////////////////////////////////////////////////////////////////////////////
///
/// Fill in the configuration parameters used by the super-class and reset everything.
///
/// \todo This uses default values for SizeOfTransformCapabilityStructure and
///       TransformCapabilityStructurePointer. This is wrong (but harmless).
///
Codec_MmeAudioSilence_c::Codec_MmeAudioSilence_c( void )
{
    CodecStatus_t Status;
    int i;

    Configuration.CodecName                             = "Silence generator";

    Configuration.StreamParameterContextCount           = 1;
    Configuration.StreamParameterContextDescriptor      = &SilentAudioCodecStreamParameterContextDescriptor;

    Configuration.DecodeContextCount                    = 4;
    Configuration.DecodeContextDescriptor               = &SilentAudioCodecDecodeContextDescriptor;

    for (i =0; i< CODEC_MAX_TRANSFORMERS; i++)
	Configuration.TransformName[i]                  = "SILENCE_GENERATOR";
    Configuration.AvailableTransformers                 = CODEC_MAX_TRANSFORMERS;

    Configuration.AddressingMode                        = CachedAddress;

    // silencegen only support 'transcoding' of DTS-HD (where transcode actually means trivial core extraction)
    Configuration.TranscodedFrameMaxSize                = DTSHD_FRAME_MAX_SIZE;

//
    
    Reset();
    
    ProtectTransformName				= true;
    
    Status = GloballyVerifyMMECapabilities();
    if( CodecNoError != Status )
    {
	CODEC_ERROR( "Silence generator not found (module not installed?)\n" );
	InitializationStatus = PlayerNotSupported;
	return;
    }
}


////////////////////////////////////////////////////////////////////////////
///
///     Destructor function, ensures a full halt and reset 
///     are executed for all levels of the class.
///
Codec_MmeAudioSilence_c::~Codec_MmeAudioSilence_c( void )
{
    Halt();
    Reset();
}


////////////////////////////////////////////////////////////////////////////
///
/// Examine the capability structure returned by the firmware.
///
/// Unconditionally return success; the silence generator does not report
/// anything other than a version number. 
///
CodecStatus_t   Codec_MmeAudioSilence_c::HandleCapabilities( void )
{
    return CodecNoError;
}


////////////////////////////////////////////////////////////////////////////
///
/// Populate the AUDIO_DECODER's initialization parameters for MPEG audio.
///
/// When this method completes Codec_MmeAudio_c::AudioDecoderInitializationParameters
/// will have been filled out with valid values sufficient to initialize an
/// MPEG audio decoder (defaults to MPEG Layer II but can be updated by new
/// stream parameters).
///
CodecStatus_t   Codec_MmeAudioSilence_c::FillOutTransformerInitializationParameters( void )
{
    MMEInitializationParameters.TransformerInitParamsSize = 0;
    MMEInitializationParameters.TransformerInitParams_p = NULL;
    
    return CodecNoError;
}


////////////////////////////////////////////////////////////////////////////
///
/// Populate the (non-existant) MME_SET_GLOBAL_TRANSFORMER_PARAMS parameters.
///
CodecStatus_t   Codec_MmeAudioSilence_c::FillOutSetStreamParametersCommand( void )
{
    TranscodeEnable = Codec_MmeAudioDtshd_c::CapableOfTranscodeDtshdToDts( ParsedAudioParameters );

    //
    // Fillout the actual command
    //

    StreamParameterContext->MMECommand.CmdStatus.AdditionalInfoSize        = 0;
    StreamParameterContext->MMECommand.CmdStatus.AdditionalInfo_p          = NULL;
    StreamParameterContext->MMECommand.ParamSize                           = 0;
    StreamParameterContext->MMECommand.Param_p                             = NULL;

//

    return CodecNoError;
}

////////////////////////////////////////////////////////////////////////////
///
/// Populate the (non-existant) MME_TRANSFORM parameters.
///
CodecStatus_t   Codec_MmeAudioSilence_c::FillOutDecodeCommand(       void )
{

    SilentAudioCodecDecodeContext_t  *Context        = (SilentAudioCodecDecodeContext_t *)DecodeContext;

    // export the frame parameters structure to the decode context (so that we can access them from the MME callback)
    memcpy(&Context->ContextFrameParameters, ParsedFrameParameters->FrameParameterStructure, sizeof(DtshdAudioFrameParameters_t));

    //
    // Fillout the actual command
    //

    DecodeContext->MMECommand.CmdStatus.AdditionalInfoSize        = 0;
    DecodeContext->MMECommand.CmdStatus.AdditionalInfo_p          = NULL;
    DecodeContext->MMECommand.ParamSize                           = 0;
    DecodeContext->MMECommand.Param_p                             = NULL;

    return CodecNoError;
}

////////////////////////////////////////////////////////////////////////////
///
/// Validate the (non-existant) reply from the transformer.
/// 
/// Unconditionally return success. Given the whole point of the transformer is
/// to mute the output it cannot 'successfully' fail in the way the audio firmware
/// does.
///
/// \return CodecNoError
///
CodecStatus_t   Codec_MmeAudioSilence_c::ValidateDecodeContext( CodecBaseDecodeContext_t *Context )
{

	SilentAudioCodecDecodeContext_t *LocalDecodeContext = (SilentAudioCodecDecodeContext_t *) Context;

    memset( &AudioDecoderStatus, 0, sizeof(AudioDecoderStatus));    // SYSFS
    
    if (TranscodeEnable)
    {
        Codec_MmeAudioDtshd_c::TranscodeDtshdToDts(
	        &LocalDecodeContext->BaseContext,
		&LocalDecodeContext->ContextFrameParameters.DtshdAudioFrameParameters,
		TranscodedBuffers + LocalDecodeContext->TranscodeBufferIndex );
    }

    return CodecNoError;
}

// /////////////////////////////////////////////////////////////////////////
//
//      Function to dump out the set stream 
//      parameters from an mme command.
//

CodecStatus_t   Codec_MmeAudioSilence_c::DumpSetStreamParameters(          void    *Parameters )
{
    CODEC_ERROR("Not implemented\n");  
    return CodecNoError;
}

// /////////////////////////////////////////////////////////////////////////
//
//      Function to dump out the decode
//      parameters from an mme command.
//

CodecStatus_t   Codec_MmeAudioSilence_c::DumpDecodeParameters(             void    *Parameters )
{
    CODEC_ERROR("Not implemented\n");  
    return CodecNoError;
}

////////////////////////////////////////////////////////////////////////////
///
///  Set Default FrameBase style TRANSFORM command for AudioDecoder MT
///  with 1 Input Buffer and 1 Output Buffer.

void Codec_MmeAudioSilence_c::SetCommandIO( void )
{	
    if (TranscodeEnable)
    {
        CodecStatus_t Status = GetTranscodeBuffer();
	
        if (Status != CodecNoError)
        {            
            CODEC_ERROR("Error while requesting Transcoded buffer: %d. Disabling transcoding...\n", Status);
            TranscodeEnable = false;
        }
        ((SilentAudioCodecDecodeContext_t *)DecodeContext)->TranscodeBufferIndex = CurrentTranscodeBufferIndex;
    }

    Codec_MmeAudio_c::SetCommandIO();
}
