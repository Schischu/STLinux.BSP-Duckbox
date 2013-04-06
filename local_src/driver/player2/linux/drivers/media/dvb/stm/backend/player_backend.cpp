/************************************************************************
COPYRIGHT (C) STMicroelectronics 2003

Source file name : player_backend.cpp - backend engine for driving player2
Author :           Julian


Date        Modification                                    Name
----        ------------                                    --------
09-Feb-07   Created                                         Julian

************************************************************************/

#include "backend_ops.h"
#include "alsa_backend_ops.h"
#include "player_interface_ops.h"
#include "fixed.h"
#include "player_module.h"
#include "player_backend.h"
#include "havana_player.h"
#include "player_factory.h"
#include "havana_playback.h"
#include "havana_demux.h"
#include "havana_stream.h"
#include "mixer_mme.h"

static class HavanaPlayer_c*    HavanaPlayer;

#ifdef __cplusplus
extern "C" {
#endif
//{{{  BackendInit
player_status_t BackendInit (void)
{
    HavanaStatus_t              Status;

    PLAYER_DEBUG("\n");
    HavanaPlayer        = new HavanaPlayer_c;
    if (HavanaPlayer == NULL)
    {
        PLAYER_ERROR("Unable to create player\n");
        return PLAYER_NO_MEMORY;
    }
    Status      = HavanaPlayer->Init ();
    if (Status != HavanaNoError)
    {
        delete HavanaPlayer;
        HavanaPlayer    = NULL;
    }
    else
        Status  = RegisterBuiltInFactories (HavanaPlayer);

    return (player_status_t)Status;
}
//}}}
//{{{  BackendDelete
player_status_t BackendDelete (void)
{
    if (HavanaPlayer != NULL)
        delete HavanaPlayer;
    HavanaPlayer        = NULL;

    return PLAYER_NO_ERROR;
}
//}}}

//{{{  PlaybackCreate
player_status_t PlaybackCreate   (playback_handle_t*      Playback)
{
    HavanaStatus_t              Status;
    class HavanaPlayback_c*     HavanaPlayback = NULL;

    PLAYER_DEBUG("\n");
    Status      = HavanaPlayer->CreatePlayback (&HavanaPlayback);
    if (Status == HavanaNoError)
        *Playback       = (playback_handle_t)HavanaPlayback;

    return (player_status_t)Status;
}
//}}}
//{{{  PlaybackDelete
player_status_t PlaybackDelete   (playback_handle_t       Playback)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;

    PLAYER_DEBUG("\n");

    return (player_status_t)HavanaPlayer->DeletePlayback (HavanaPlayback);
}
//}}}
//{{{  PlaybackAddDemux
player_status_t PlaybackAddDemux (playback_handle_t       Playback,
                                  int                     DemuxId,
                                  demux_handle_t*         Demux)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;
    class HavanaDemux_c*        HavanaDemux     = NULL;
    HavanaStatus_t              Status;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    Status      = HavanaPlayback->AddDemux (DemuxId, &HavanaDemux);
    if (Status == HavanaNoError)
        *Demux  = (demux_handle_t)HavanaDemux;

    return (player_status_t)Status;
}
//}}}
//{{{  PlaybackRemoveDemux
player_status_t PlaybackRemoveDemux    (playback_handle_t       Playback,
                                        demux_handle_t          Demux)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;
    class HavanaDemux_c*        HavanaDemux     = (class HavanaDemux_c*)Demux;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    return (player_status_t) HavanaPlayback->RemoveDemux (HavanaDemux);
}
//}}}
//{{{  PlaybackAddStream
player_status_t PlaybackAddStream       (playback_handle_t      Playback,
                                         char*                  Media,
                                         char*                  Format,
                                         char*                  Encoding,
                                         unsigned int           SurfaceId,
                                         stream_handle_t*       Stream)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;
    class HavanaStream_c*       HavanaStream    = NULL;
    HavanaStatus_t              Status          = HavanaNoError;

    PLAYER_DEBUG("%s\n", Media);
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    Status      = HavanaPlayback->AddStream (Media, Format, Encoding, SurfaceId, &HavanaStream);
    if (Status == HavanaNoError)
        *Stream = (stream_handle_t)HavanaStream;

    return (player_status_t)Status;
}
//}}}
//{{{  PlaybackRemoveStream
player_status_t PlaybackRemoveStream   (playback_handle_t       Playback,
                                        stream_handle_t         Stream)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    return (player_status_t)HavanaPlayback->RemoveStream (HavanaStream);
}
//}}}
//{{{  PlaybackSetSpeed
player_status_t PlaybackSetSpeed        (playback_handle_t       Playback,
                                         int                     Speed)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    return (player_status_t)HavanaPlayback->SetSpeed (Speed);
}
//}}}
//{{{  PlaybackGetSpeed
player_status_t PlaybackGetSpeed        (playback_handle_t       Playback,
                                         int*                    PlaySpeed)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    return (player_status_t)HavanaPlayback->GetSpeed (PlaySpeed);
}
//}}}
//{{{  PlaybackSetOption
player_status_t PlaybackSetOption      (playback_handle_t       Playback,
                                        play_option_t           Option,
                                        unsigned int            Value)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    return (player_status_t)HavanaPlayback->SetOption (Option, Value);
}
//}}}
//{{{  PlaybackSetNativePlaybackTime
player_status_t PlaybackSetNativePlaybackTime  (playback_handle_t    Playback,
                                                unsigned long long   NativeTime,
                                                unsigned long long   SystemTime)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    return (player_status_t)HavanaPlayback->SetNativePlaybackTime (NativeTime, SystemTime);
}
//}}}
//{{{  PlaybackSetClockdataPoint
player_status_t PlaybackSetClockDataPoint      (playback_handle_t       Playback,
                                                time_format_t           TimeFormat,
                                                unsigned long long      SourceTime,
                                                unsigned long long      SystemTime)
{
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    return (player_status_t)HavanaPlayback->SetClockDataPoint (TimeFormat, SourceTime, SystemTime);
}
//}}}
//{{{  PlaybackGetPlayerEnvironment
player_status_t PlaybackGetPlayerEnvironment (playback_handle_t               Playback,
                                  playback_handle_t*              playerplayback)
{
    PlayerPlayback_t            player_playback = NULL;
    class HavanaPlayback_c*     HavanaPlayback  = (class HavanaPlayback_c*)Playback;
    HavanaStatus_t              Status          = HavanaNoError;

    PLAYER_DEBUG("\n");
    if (HavanaPlayback == NULL)
        return PLAYER_PLAYBACK_INVALID;

    Status              = HavanaPlayback->GetPlayerEnvironment (&player_playback);
    if (Status == HavanaNoError)
        *playerplayback = (void *)player_playback;

    return (player_status_t)Status;
}
//}}}

//{{{  DemuxInjectData
int DemuxInjectData    (demux_handle_t          Demux,
                        const unsigned char*    Data,
                        unsigned int            DataLength)
{
    class HavanaDemux_c*        HavanaDemux     = (class HavanaDemux_c*)Demux;

    if (HavanaDemux == NULL)
        return -1;

    HavanaDemux->InjectData (Data, DataLength);

    return DataLength;
}
//}}}

//{{{  StreamInjectData
int StreamInjectData            (stream_handle_t         Stream,
                                 const unsigned char*    Data,
                                 unsigned int            DataLength)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return -1;

    HavanaStream->InjectData (Data, DataLength);

    return DataLength;
}
//}}}
//{{{  StreamInjectDataPacket
int StreamInjectDataPacket      (stream_handle_t         Stream,
                                 const unsigned char*    Data,
                                 unsigned int            DataLength,
                                 bool                    PresentationTimeValid,
                                 unsigned long long      PresentationTime )
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return -1;

    HavanaStream->InjectDataPacket (Data, DataLength, PresentationTimeValid, PresentationTime);

    return DataLength;
}
//}}}
//{{{  StreamSetId
player_status_t StreamSetId    (stream_handle_t         Stream,
                                unsigned int            DemuxId,
                                unsigned int            Id)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->SetId (DemuxId, Id);
}
//}}}
//{{{  StreamSetOption
player_status_t StreamSetOption        (stream_handle_t         Stream,
                                        play_option_t           Option,
                                        unsigned int            Value)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->SetOption (Option, Value);
}
//}}}
//{{{  StreamGetOption
player_status_t StreamGetOption        (stream_handle_t         Stream,
                                        play_option_t           Option,
                                        unsigned int*           Value)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->GetOption (Option, Value);
}
//}}}
//{{{  StreamEnable
player_status_t StreamEnable   (stream_handle_t         Stream,
                                unsigned int            Enable)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->Enable (Enable);
}
//}}}
//{{{  StreamDrain
player_status_t StreamDrain    (stream_handle_t         Stream,
                                unsigned int            Discard)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->Drain (Discard);
}
//}}}
//{{{  StreamGetPlayInfo
player_status_t StreamGetPlayInfo(stream_handle_t         Stream,
                                struct play_info_s*     PlayInfo)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->GetPlayInfo (PlayInfo);
}
//}}}
//{{{  StreamSetOutputWindow
player_status_t StreamSetOutputWindow  (stream_handle_t         Stream,
                                        unsigned int            X,
                                        unsigned int            Y,
                                        unsigned int            Width,
                                        unsigned int            Height)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->SetOutputWindow (X, Y, Width, Height);
}
//}}}
//{{{  StreamGetOutputWindow
player_status_t StreamGetOutputWindow  (stream_handle_t         Stream,
                                        unsigned int*           X,
                                        unsigned int*           Y,
                                        unsigned int*           Width,
                                        unsigned int*           Height)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->GetOutputWindow (X, Y, Width, Height);
}
//}}}
//{{{  StreamSetInputWindow
player_status_t StreamSetInputWindow   (stream_handle_t         Stream,
                                        unsigned int            X,
                                        unsigned int            Y,
                                        unsigned int            Width,
                                        unsigned int            Height)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->SetInputWindow (X, Y, Width, Height);
}
//}}}
//{{{  StreamSetPlayInterval
player_status_t StreamSetPlayInterval  (stream_handle_t         Stream,
                                        unsigned long long      Start,
                                        unsigned long long      End)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->SetPlayInterval (Start, End);
}
//}}}
//{{{  StreamStep
player_status_t StreamStep                (stream_handle_t         Stream)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->Step ();
}
//}}}
//{{{  StreamDiscontinuity
player_status_t StreamDiscontinuity     (stream_handle_t         Stream,
                                         unsigned int            ContinuousReverse,
                                         unsigned int            SurplusData)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    //PLAYER_DEBUG("%x %x %x\n", Discontinuity, Discontinuity & DISCONTINUITY_CONTINUOUS_REVERSE, Discontinuity & DISCONTINUITY_SURPLUS_DATA);
    return (player_status_t)HavanaStream->Discontinuity (ContinuousReverse, SurplusData);
}
//}}}
//{{{  StreamSwitch
player_status_t StreamSwitch    (stream_handle_t         Stream,
                                 char*                   Format,
                                 char*                   Encoding)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->Switch (Format, Encoding);
}
//}}}
//{{{  StreamChannelSelect
player_status_t StreamChannelSelect     (stream_handle_t         Stream,
                                         channel_select_t        Channel)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->ChannelSelect (Channel);
}
//}}}
//{{{  StreamSetAlarm
player_status_t StreamSetAlarm (stream_handle_t         Stream,
                                unsigned long long      Pts)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  StreamGetDecodeBuffer
player_status_t StreamGetDecodeBuffer  (stream_handle_t         Stream,
                                        buffer_handle_t*        Buffer,
                                        unsigned char**         Data,
                                        surface_format_t        Format,
                                        unsigned int            DimensionCount,
                                        unsigned int            Dimensions[],
                                        unsigned int*           Index,
                                        unsigned int*           Stride)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->GetDecodeBuffer (Buffer, Data, Format, DimensionCount, Dimensions, Index, Stride);
}
//}}}
//{{{  StreamReturnDecodeBuffer
player_status_t StreamReturnDecodeBuffer (stream_handle_t         Stream,
                                        buffer_handle_t*        Buffer)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->ReturnDecodeBuffer (Buffer);
}
//}}}
//{{{  StreamGetDecodeBufferPoolStatus
player_status_t StreamGetDecodeBufferPoolStatus  (stream_handle_t         Stream,
                                                  unsigned int*           BuffersInPool,
                                                  unsigned int*           BuffersWithNonZeroReferenceCount)

{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    return (player_status_t)HavanaStream->GetDecodeBufferPoolStatus (BuffersInPool,BuffersWithNonZeroReferenceCount);
}
//}}}
//{{{  StreamRegisterEventSignalCallback
stream_event_signal_callback StreamRegisterEventSignalCallback (stream_handle_t                 Stream,
                                                                context_handle_t                Context,
                                                                stream_event_signal_callback    Callback)
{
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return NULL;

    return HavanaStream->RegisterEventSignalCallback (Context, Callback);
}
//}}}
//{{{  StreamGetPlayerEnvironment
player_status_t StreamGetPlayerEnvironment (stream_handle_t                 Stream,
                                playback_handle_t*              playerplayback,
                                stream_handle_t*                playerstream)
{
    PlayerPlayback_t            player_playback = NULL;
    PlayerStream_t              player_stream   = NULL;
    class HavanaStream_c*       HavanaStream    = (class HavanaStream_c*)Stream;
    HavanaStatus_t              Status          = HavanaNoError;

    PLAYER_DEBUG("\n");
    if (HavanaStream == NULL)
        return PLAYER_STREAM_INVALID;

    Status              = HavanaStream->GetPlayerEnvironment (&player_playback, &player_stream);
    if (Status == HavanaNoError)
    {
        *playerplayback = (void *)player_playback;
        *playerstream   = (void *)player_stream;
    }

    return (player_status_t)Status;
}
//}}}

//{{{  MixerGetInstance
player_status_t MixerGetInstance (int                     StreamId,
                                component_handle_t*     Classoid)
{
    const char *BackendId;

    PLAYER_DEBUG("\n");
    switch (StreamId)
    {
        case 0: BackendId = BACKEND_MIXER0_ID; break;
        case 1: BackendId = BACKEND_MIXER1_ID; break;
        default: return PLAYER_ERROR;
    }

    return (player_status_t)HavanaPlayer->CallFactory  (BACKEND_AUDIO_ID,
                                                        BackendId,
                                                        StreamTypeAudio,
                                                        ComponentExternal,
                                                        Classoid);
}
//}}}
//{{{  MixerAllocSubStream
player_status_t MixerAllocSubStream    (component_handle_t      Component,
                                        int                    *SubStreamId)
{
    Mixer_Mme_c *Mixer = (Mixer_Mme_c *) Component;
    PlayerStatus_t Status;

    Status = Mixer->AllocInteractiveInput(SubStreamId);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  MixerFreeSubStream
player_status_t MixerFreeSubStream     (component_handle_t      Component,
                                        int                     SubStreamId)
{
    Mixer_Mme_c *Mixer = (Mixer_Mme_c *) Component;
    PlayerStatus_t Status;

    Status = Mixer->FreeInteractiveInput(SubStreamId);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  MixerSetupSubStream
player_status_t MixerSetupSubStream    (component_handle_t      Component,
                                        int                     SubStreamId,
                                        struct alsa_substream_descriptor *Descriptor)
{
    Mixer_Mme_c *Mixer = (Mixer_Mme_c *) Component;
    PlayerStatus_t Status;

    Status = Mixer->SetupInteractiveInput(SubStreamId, Descriptor);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  MixerPrepareSubStream
player_status_t MixerPrepareSubStream  (component_handle_t      Component,
                                        int                     SubStreamId)

{
    Mixer_Mme_c *Mixer = (Mixer_Mme_c *) Component;
    PlayerStatus_t Status;

    Status = Mixer->PrepareInteractiveInput(SubStreamId);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  MixerStartSubStream
player_status_t MixerStartSubStream    (component_handle_t      Component,
                                        int                     SubStreamId)
{
    Mixer_Mme_c *Mixer = (Mixer_Mme_c *) Component;
    PlayerStatus_t Status;

    Status = Mixer->EnableInteractiveInput(SubStreamId);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  MixerStopSubstream
player_status_t MixerStopSubStream     (component_handle_t      Component,
                                        int                     SubStreamId)
{
    Mixer_Mme_c *Mixer = (Mixer_Mme_c *) Component;
    PlayerStatus_t Status;

    Status = Mixer->DisableInteractiveInput(SubStreamId);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    return PLAYER_NO_ERROR;
}
//}}}

//{{{  DisplayCreate
player_status_t DisplayCreate  (char*           Media,
                                unsigned int    SurfaceId)
{
    class HavanaDisplay_c*      Display;

    PLAYER_DEBUG("SurfaceId  = %d\n", SurfaceId);

    return (player_status_t)HavanaPlayer->CreateDisplay (Media, SurfaceId, &Display);
}
//}}}
//{{{  DisplayDelete
player_status_t DisplayDelete  (char*           Media,
                                unsigned int    SurfaceId)
{
    PLAYER_DEBUG("SurfaceId  = %d\n", SurfaceId);

    return (player_status_t)HavanaPlayer->DeleteDisplay (Media, SurfaceId);
}
//}}}
//{{{  DisplaySynchronize
player_status_t DisplaySynchronize (char*           Media,
                        unsigned int    SurfaceId)
{
    return (player_status_t)HavanaPlayer->SynchronizeDisplay (Media, SurfaceId);
}
//}}}

//{{{  ComponentSetAttribute
player_status_t ComponentSetAttribute  (player_component_handle_t       Component,
                                        const char*                     Attribute,
                                        union attribute_descriptor_u*   Value)
{
    BaseComponentClass_c*       PlayerComponent = (BaseComponentClass_c*)Component;
    PlayerStatus_t              Status;

    PLAYER_DEBUG("\n");
    if (PlayerComponent == NULL)
        return PLAYER_COMPONENT_INVALID;

    Status      = PlayerComponent->SetAttribute (Attribute, (PlayerAttributeDescriptor_t*)Value);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  ComponentGetAttribute
player_status_t ComponentGetAttribute  (player_component_handle_t       Component,
                                        const char*                     Attribute,
                                        union attribute_descriptor_u*   Value)
{
    BaseComponentClass_c*               PlayerComponent = (BaseComponentClass_c*)Component;
    PlayerStatus_t                      Status;
    PlayerAttributeDescriptor_t         AttributeDescriptor;

    if (PlayerComponent == NULL)
        return PLAYER_COMPONENT_INVALID;

    Status      = PlayerComponent->GetAttribute (Attribute, &AttributeDescriptor);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    switch (AttributeDescriptor.Id)
    {
        case SYSFS_ATTRIBUTE_ID_INTEGER:
            Value->Int                      = (int)AttributeDescriptor.u.Int;
            break;
        case SYSFS_ATTRIBUTE_ID_BOOL:
            Value->Bool                     = (int)AttributeDescriptor.u.Bool;
            break;
        case SYSFS_ATTRIBUTE_ID_UNSIGNEDLONGLONGINT:
            Value->UnsignedLongLongInt      = (unsigned long long int)AttributeDescriptor.u.UnsignedLongLongInt;
            break;
        case SYSFS_ATTRIBUTE_ID_CONSTCHARPOINTER:
            Value->ConstCharPointer         = (char*)AttributeDescriptor.u.ConstCharPointer;
            break;
        default:
            PLAYER_ERROR("This attribute does not exist.\n");
            return PLAYER_ERROR;
    }

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  ComponentSetModuleParameters
player_status_t ComponentSetModuleParameters   (component_handle_t      Component,
                                                void*                   Data,
                                                unsigned int            Size)
{
    BaseComponentClass_c*       ActualComponent = (BaseComponentClass_c*)Component;
    PlayerStatus_t              Status;

    PLAYER_DEBUG("\n");
    if (ActualComponent == NULL)
        return PLAYER_COMPONENT_INVALID;

    Status      = ActualComponent->SetModuleParameters (Size, Data);
    if (Status != PlayerNoError)
        return PLAYER_ERROR;

    return PLAYER_NO_ERROR;
}
//}}}
//{{{  PlayerRegisterEventSignalCallback
player_event_signal_callback PlayerRegisterEventSignalCallback (player_event_signal_callback Callback)
{
    PLAYER_DEBUG("\n");

    return HavanaPlayer->RegisterEventSignalCallback (Callback);
}
//}}}

#ifdef __cplusplus
}
#endif
//{{{  C++ operators
#if defined (__KERNEL__)
#include "osinline.h"

////////////////////////////////////////////////////////////////////////////
// operator new
////////////////////////////////////////////////////////////////////////////

void* operator new (unsigned int size)
{
#if 0
    void* Addr = __builtin_new (size);
    HAVANA_DEBUG ("Newing %d (%x) at %p\n", size, size, Addr);
    return Addr;
#else
    return __builtin_new (size);
#endif
}

////////////////////////////////////////////////////////////////////////////
// operator delete
////////////////////////////////////////////////////////////////////////////

void operator delete (void *mem)
{
#if 0
    HAVANA_DEBUG ("Deleting (%p)\n", mem);
#else
    __builtin_delete (mem);
#endif
}

////////////////////////////////////////////////////////////////////////////
// operator new
////////////////////////////////////////////////////////////////////////////

void* operator new[] (unsigned int size)
{
#if 0
    void* Addr = __builtin_vec_new (size);
    HAVANA_DEBUG ("Vec newing %d (%x) at %p\n", size, size, Addr);
    return Addr;
#else
    return __builtin_vec_new (size);
#endif
}

////////////////////////////////////////////////////////////////////////////
//   operator delete
////////////////////////////////////////////////////////////////////////////

void operator delete[] (void *mem)
{
#if 0
    HAVANA_DEBUG ("Vec deleting (%p)\n", mem);
#else
    __builtin_vec_delete (mem);
#endif
}
#endif
//}}}
