/************************************************************************
Copyright (C) 2003 STMicroelectronics. All Rights Reserved.

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

Source file name : player_backend.h - player access points
Author :           Julian

Date        Modification                                    Name
----        ------------                                    --------
31-Jan-07   Created                                         Julian

************************************************************************/

#ifndef H_PLAYER_BACKEND
#define H_PLAYER_BACKEND

#ifdef __cplusplus
extern "C" {
#endif
player_status_t BackendInit                    (void);
player_status_t BackendDelete                  (void);

player_status_t PlaybackCreate                 (playback_handle_t      *playback);
player_status_t PlaybackDelete                 (playback_handle_t       playback);
player_status_t PlaybackAddDemux               (playback_handle_t       playback,
                                                int                     demux_id,
                                                demux_handle_t         *demux);
player_status_t PlaybackRemoveDemux            (playback_handle_t       playback,
                                                demux_handle_t          demux);
player_status_t PlaybackAddStream              (playback_handle_t       playback,
                                                char                   *media,
                                                char                   *format,
                                                char                   *encoding,
                                                unsigned int            surface_id,
                                                stream_handle_t        *stream);
player_status_t PlaybackRemoveStream           (playback_handle_t       playback,
                                                stream_handle_t         stream);
player_status_t PlaybackSetSpeed               (playback_handle_t       playback,
                                                int                     speed);
player_status_t PlaybackGetSpeed               (playback_handle_t       playback,
                                                int                    *speed);
player_status_t PlaybackSetOption              (playback_handle_t       playback,
                                                play_option_t           option,
                                                unsigned int            value);
player_status_t PlaybackSetNativePlaybackTime  (playback_handle_t       playback,
                                                unsigned long long      nativeTime,
                                                unsigned long long      systemTime);
player_status_t PlaybackSetClockDataPoint      (playback_handle_t       Playback,
                                                time_format_t           TimeFormat,
                                                unsigned long long      SourceTime,
                                                unsigned long long      SystemTime);
player_status_t PlaybackGetPlayerEnvironment   (playback_handle_t       playback,
                                                playback_handle_t*      playerplayback);

int DemuxInjectData                            (demux_handle_t          demux,
                                                const unsigned char    *data,
                                                unsigned int            data_length);

int StreamInjectData                           (stream_handle_t         stream,
                                                const unsigned char*    data,
                                                unsigned int            data_length);
int StreamInjectDataPacket                     (stream_handle_t         stream,
                                                const unsigned char*    data,
                                                unsigned int            data_length,
                                                bool                    PresentationTimeValid,
                                                unsigned long long      PresentationTime);
player_status_t StreamDiscontinuity            (stream_handle_t         stream,
                                                unsigned int            ContinuousReverse,
                                                unsigned int            SurplusData);
player_status_t StreamDrain                    (stream_handle_t         stream,
                                                unsigned int            discard);
player_status_t StreamEnable                   (stream_handle_t         stream,
                                                unsigned int            enable);
player_status_t StreamSetId                    (stream_handle_t         stream,
                                                unsigned int            demux_id,
                                                unsigned int            id);
player_status_t StreamChannelSelect            (stream_handle_t         Stream,
                                                channel_select_t        Channel);
player_status_t StreamSetOption                (stream_handle_t         stream,
                                                play_option_t           option,
                                                unsigned int            value);
player_status_t StreamGetOption                (stream_handle_t         stream,
                                                play_option_t           option,
                                                unsigned int*           value);
player_status_t StreamStep                     (stream_handle_t         stream);
player_status_t StreamSwitch                   (stream_handle_t         stream,
                                                char                   *format,
                                                char                   *encoding);
player_status_t StreamSetAlarm                 (stream_handle_t         stream,
                                                unsigned long long      pts);
player_status_t StreamGetPlayInfo              (stream_handle_t         Stream,
                                                struct play_info_s*     PlayInfo);
player_status_t StreamGetDecodeBuffer          (stream_handle_t         stream,
                                                buffer_handle_t        *buffer,
                                                unsigned char         **data,
                                                surface_format_t        Format,
                                                unsigned int            DimensionCount,
                                                unsigned int            Dimensions[],
                                                unsigned int*           Index,
                                                unsigned int*           Stride);
player_status_t StreamReturnDecodeBuffer       (stream_handle_t         Stream,
                                                buffer_handle_t*        Buffer);
player_status_t StreamSetOutputWindow          (stream_handle_t         Stream,
                                                unsigned int            X,
                                                unsigned int            Y,
                                                unsigned int            Width,
                                                unsigned int            Height);
player_status_t StreamGetOutputWindow          (stream_handle_t         Stream,
                                                unsigned int*           X,
                                                unsigned int*           Y,
                                                unsigned int*           Width,
                                                unsigned int*           Height);
player_status_t StreamSetInputWindow           (stream_handle_t         Stream,
                                                unsigned int            X,
                                                unsigned int            Y,
                                                unsigned int            Width,
                                                unsigned int            Height);
player_status_t StreamSetPlayInterval          (stream_handle_t         Stream,
                                                unsigned long long      Start,
                                                unsigned long long      End);
player_status_t StreamGetDecodeBufferPoolStatus  (stream_handle_t         stream,
                                                unsigned int*           BuffersInPool,
                                                unsigned int*           BuffersWithNonZeroReferenceCount);


stream_event_signal_callback    StreamRegisterEventSignalCallback      (stream_handle_t                 stream,
                                                                        context_handle_t                context,
                                                                        stream_event_signal_callback    callback);
player_status_t StreamGetPlayerEnvironment     (stream_handle_t                 Stream,
                                                playback_handle_t*              playerplayback,
                                                stream_handle_t*                playerstream);
player_status_t DisplayCreate                  (char*                           Media,
                                                unsigned int                    SurfaceId);
player_status_t DisplayDelete                  (char*                           Media,
                                                unsigned int                    SurfaceId);
player_status_t DisplaySynchronize             (char*                           Media,
                                                unsigned int                    SurfaceId);
player_status_t ComponentGetAttribute          (component_handle_t              Component,
                                                const char*                     Attribute,
                                                union attribute_descriptor_u*   Value);
player_status_t ComponentSetAttribute          (component_handle_t              Component,
                                                const char*                     Attribute,
                                                union attribute_descriptor_u*   Value);
player_status_t ComponentSetModuleParameters   (component_handle_t              Component,
                                                void*                           Data,
                                                unsigned int                    Size);

player_event_signal_callback                    PlayerRegisterEventSignalCallback      (player_event_signal_callback    Callback);


player_status_t MixerGetInstance               (int                             StreamId,
                                                component_handle_t*             Classoid);

player_status_t MixerAllocSubStream            (component_handle_t              Component,
                                                int                            *SubStreamId);
player_status_t MixerFreeSubStream             (component_handle_t              Component,
                                                int                             SubStreamId);
player_status_t MixerSetupSubStream            (component_handle_t              Component,
                                                int                             SubStreamId,
                                                struct alsa_substream_descriptor *Descriptor);
player_status_t MixerPrepareSubStream          (component_handle_t              Component,
                                                int                             SubStreamId);
player_status_t MixerStartSubStream            (component_handle_t              Component,
                                                int                             SubStreamId);
player_status_t MixerStopSubStream             (component_handle_t              Component,
                                                int                             SubStreamId);


#ifdef __cplusplus
}
#endif

#endif
