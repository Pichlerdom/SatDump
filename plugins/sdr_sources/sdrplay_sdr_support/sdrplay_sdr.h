#pragma once

#include "common/dsp_source_sink/dsp_sample_source.h"
#include <sdrplay_api.h>
#include "logger.h"
#include "common/rimgui.h"
#include "common/widgets/double_list.h"

// define HW id for RSP1B if it's not available in older version of SDRPlay API
#ifndef SDRPLAY_RSP1B_ID
#define SDRPLAY_RSP1B_ID (6)
#endif

// define HW id for RSPDxR2 if it's not available in older version of SDRPlay API
#ifndef SDRPLAY_RSPdxR2_ID
#define SDRPLAY_RSPdxR2_ID (7) // TBD
#endif

class SDRPlaySource : public dsp::DSPSampleSource
{
protected:

    struct ifMode{
      std::string name;
      sdrplay_api_If_kHzT if_type;
      sdrplay_api_Bw_MHzT if_bw;
      uint64_t samplerate;
    };

    ifMode available_if_modes[10] = {
        {"ZeroIF\0", sdrplay_api_IF_Zero, sdrplay_api_BW_Undefined, 2000000},
        {"LowIF 450kHz, IFBW 200kHz\0", sdrplay_api_IF_0_450,
         sdrplay_api_BW_0_200, 2000000},
        {"LowIF 450kHz, IFBW 300kHz\0", sdrplay_api_IF_0_450,
         sdrplay_api_BW_0_300, 2000000},
        {"LowIF 450kHz, IFBW 600kHz\0", sdrplay_api_IF_0_450,
         sdrplay_api_BW_0_600, 2000000},
        {"LowIF 1620kHz, IFBW 200kHz\0", sdrplay_api_IF_1_620,
         sdrplay_api_BW_0_200, 6000000},
        {"LowIF 1620kHz, IFBW 300kHz\0", sdrplay_api_IF_1_620,
         sdrplay_api_BW_0_300, 6000000},
        {"LowIF 1620kHz, IFBW 600kHz\0", sdrplay_api_IF_1_620,
         sdrplay_api_BW_0_600, 6000000},
        {"LowIF 1620kHz, IFBW 1536kHz\0", sdrplay_api_IF_1_620,
         sdrplay_api_BW_1_536, 6000000},
        {"LowIF 2048kHz, IFBW 1536kHz\0", sdrplay_api_IF_2_048,
         sdrplay_api_BW_1_536, 8000000},
        {"LowIF 2048kHz, IFBW 5000kHz\0", sdrplay_api_IF_2_048,
         sdrplay_api_BW_5_000, 8000000},
    };

    bool is_open = false, is_started = false;
    sdrplay_api_DeviceT sdrplay_dev;
    sdrplay_api_DeviceParamsT *dev_params = nullptr;
    sdrplay_api_RxChannelParamsT *channel_params = nullptr;
    sdrplay_api_CallbackFnsT callback_funcs;
    static sdrplay_api_DeviceT devices_addresses[128];
    static void event_callback(sdrplay_api_EventT id, sdrplay_api_TunerSelectT tuner, sdrplay_api_EventParamsT *params, void *ctx);
    static void stream_callback(short *real, short *imag, sdrplay_api_StreamCbParamsT *params, unsigned int cnt, unsigned int reset, void *ctx);

    int max_gain;

    widgets::DoubleList samplerate_widget;

    std::string if_mode_option_str;

    int lna_gain = 0;
    int if_gain = 20;

    int if_mode_index = 0;
    bool bias = false;
    bool fm_notch = false;
    bool dab_notch = false;
    bool am_notch = false;
    int antenna_input = 0;
    int agc_mode = 0;

    void set_gains();
    void set_bias();
    void set_agcs();
    void set_others();

    void set_duo_tuner();
    void set_duo_channel();

public:
    SDRPlaySource(dsp::SourceDescriptor source) : DSPSampleSource(source), samplerate_widget("Samplerate")
    {
    }

    ~SDRPlaySource()
    {
        stop();
        close();
    }

    void set_settings(nlohmann::json settings);
    nlohmann::json get_settings();

    void open();
    void start();
    void stop();
    void close();

    void set_frequency(uint64_t frequency);

    void drawControlUI();

    void set_samplerate(uint64_t samplerate);
    uint64_t get_samplerate();

    static std::string getID() { return "sdrplay"; }
    static std::shared_ptr<dsp::DSPSampleSource> getInstance(dsp::SourceDescriptor source) { return std::make_shared<SDRPlaySource>(source); }
    static std::vector<dsp::SourceDescriptor> getAvailableSources();
};
