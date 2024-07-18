#include "XTTSClient.hpp"
#include <cstring>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace AIKIT;

XTTSClient* XTTSClient::instance_ = nullptr;

XTTSClient::XTTSClient() : ttsFinished(false), fin(nullptr) {
    AIKIT_Configurator::builder()
        .app().appID("686f608e").apiSecret("MTM4ZjA2YTUzNDE4MWY1ZmFmZDFhNGVl").apiKey("da680e7deb1cde0e4ac01881b362a940").workDir("./")
        .auth().authType(0)
        .log().logLevel(LOG_LVL_INFO).logPath("./");

    if (AIKIT_Init() != 0) {
        throw runtime_error("AIKIT_Init failed");
    }

    AIKIT_Callbacks cbs = {OnOutput, OnEvent, OnError};
    AIKIT_RegisterAbilityCallback(ABILITY, cbs);
    instance_ = this;
}

XTTSClient::~XTTSClient() {
    AIKIT_UnInit();
}

void XTTSClient::createXTTS(const json& config) {
    std::lock_guard<std::mutex> lock(mutex_);

    cout << "text: " << config["text"] << endl;
    cout << "vcn: " << config["vcn"] << endl;
    cout << "vcnModel: " << config["vcnModel"] << endl;
    cout << "language: " << config["language"] << endl;
    cout << "speed: " << config["speed"] << endl;
    cout << "pitch: " << config["pitch"] << endl;
    cout << "volume: " << config["volume"] << endl;
    cout << "textEncoding: " << config["textEncoding"] << endl;
    cout << "sample_rate: " << config["sample_rate"] << endl;
    cout << "bits_per_sample: " << config["bits_per_sample"] << endl;
    cout << "channels: " << config["channels"] << endl;

    AIKIT_ParamBuilder* paramBuilder = nullptr;
    AIKIT_DataBuilder* dataBuilder = nullptr;
    string cntext = config["text"].get<string>();
    AIKIT_HANDLE* handle = nullptr;

    try {
        paramBuilder = AIKIT_ParamBuilder::create();
        paramBuilder->param("vcn", config["vcn"].get<string>().c_str(), config["vcn"].get<string>().size());
        paramBuilder->param("vcnModel", config["vcnModel"].get<string>().c_str(), config["vcnModel"].get<string>().size());
        paramBuilder->param("language", config["language"].get<int>());
        paramBuilder->param("speed", config["speed"].get<int>());
        paramBuilder->param("pitch", config["pitch"].get<int>());
        paramBuilder->param("volume", config["volume"].get<int>());
        paramBuilder->param("textEncoding", config["textEncoding"].get<string>().c_str(), config["textEncoding"].get<string>().size());

        if (AIKIT_Start(ABILITY, AIKIT_Builder::build(paramBuilder), nullptr, &handle) != 0) {
            throw runtime_error("AIKIT_Start failed");
        }

        dataBuilder = AIKIT_DataBuilder::create();
        dataBuilder->payload(AiText::get("text")->data(cntext.c_str(), cntext.length())->once()->valid());

        fin = fopen("./output.wav", "wb");
        if (fin == nullptr) {
            throw runtime_error("fopen output.wav failed");
        }

        WriteWAVHeader(fin, config["sample_rate"], config["bits_per_sample"], config["channels"]);

        if (AIKIT_Write(handle, AIKIT_Builder::build(dataBuilder)) != 0) {
            throw runtime_error("AIKIT_Write failed");
        }

        {
            unique_lock<mutex> lk(cvMutex);
            cv.wait(lk, [this] { return ttsFinished.load(); });
        }

        AIKIT_End(handle);
    } catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
        if (fin != nullptr) {
            UpdateWAVHeader(fin);
            fclose(fin);
            fin = nullptr;
        }
        if (paramBuilder != nullptr) {
            delete paramBuilder;
            paramBuilder = nullptr;
        }
        if (dataBuilder != nullptr) {
            delete dataBuilder;
            dataBuilder = nullptr;
        }
        if (handle != nullptr) {
            AIKIT_End(handle);
        }
        throw; // rethrow the exception to ensure the caller knows an error occurred
    }

    if (fin != nullptr) {
        UpdateWAVHeader(fin);
        fclose(fin);
        fin = nullptr;
    }
    if (paramBuilder != nullptr) {
        delete paramBuilder;
        paramBuilder = nullptr;
    }
    if (dataBuilder != nullptr) {
        delete dataBuilder;
        dataBuilder = nullptr;
    }
}

void XTTSClient::WriteWAVHeader(FILE *fout, int sampleRate, int bitsPerSample, int channels) {
    WAVHeader hdr{};
    memcpy(hdr.riff_header, "RIFF", 4);
    memcpy(hdr.wave_header, "WAVE", 4);
    memcpy(hdr.fmt_header, "fmt ", 4);
    hdr.fmt_chunk_size = 16;
    hdr.audio_format = 1; // PCM
    hdr.num_channels = channels;
    hdr.sample_rate = sampleRate;
    hdr.bit_depth = bitsPerSample;
    hdr.byte_rate = sampleRate * channels * (bitsPerSample / 8);
    hdr.sample_alignment = channels * (bitsPerSample / 8);
    memcpy(hdr.data_header, "data", 4);
    hdr.data_bytes = 0;
    hdr.wav_size = 36 + hdr.data_bytes;

    fwrite(&hdr, sizeof(hdr), 1, fout);
}

void XTTSClient::UpdateWAVHeader(FILE *fout) {
    long file_size = ftell(fout);
    fseek(fout, 4, SEEK_SET);
    uint32_t riff_size = file_size - 8;
    fwrite(&riff_size, sizeof(riff_size), 1, fout);

    fseek(fout, 40, SEEK_SET);
    uint32_t data_size = file_size - 44;
    fwrite(&data_size, sizeof(data_size), 1, fout);

    fseek(fout, 0, SEEK_END);
}

void XTTSClient::OnOutput(AIKIT_HANDLE* handle, const AIKIT_OutputData* output) {
    if ((output->node->value) && (instance_->fin != nullptr)) {
        fwrite(output->node->value, sizeof(char), output->node->len, instance_->fin);
        UpdateWAVHeader(instance_->fin);
    }
}

void XTTSClient::OnEvent(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue) {
    if (eventType == AIKIT_Event_End) {
        {
            std::lock_guard<std::mutex> lock(instance_->cvMutex);
            instance_->ttsFinished = true;
        }
        instance_->cv.notify_all();
    }
}

void XTTSClient::OnError(AIKIT_HANDLE* handle, int32_t err, const char* desc) {
    printf("OnError:%d\n", err);
}
