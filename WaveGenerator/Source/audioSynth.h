//#include "geometry_generator.h"
//#include "simulator.h"
//#include "engine.h"
//#include "simulation_object.h"
//#include "ui_manager.h"
//#include "dynamometer.h"
//#include "oscilloscope.h"
#include "audio_buffer.h"
#include "convolution_filter.h"
//#include "shaders.h"
//#include "engine_view.h"
//#include "right_gauge_cluster.h"
//#include "cylinder_temperature_gauge.h"
#include "synthesizer.h"
//#include "oscilloscope_cluster.h"
//#include "performance_cluster.h"
//#include "load_simulation_cluster.h"
//#include "mixer_cluster.h"
//#include "info_cluster.h"

#include "delta.h"
//#include "dtv.h"

#include <vector>

class AudioSynth {
public:
    AudioSynth();
    virtual ~AudioSynth();

    void initialize(void* instance);
    void run();
    void destroy();

protected:
    void renderScene();

protected:
    virtual void initialize();
    virtual void process(float dt);
    virtual void render();

    float m_displayHeight;
    int m_gameWindowHeight;
    int m_screenWidth;
    int m_screenHeight;

    std::string m_assetPath;

    bool m_paused;

protected:
    //ysAudioBuffer* m_outputAudioBuffer;
    AudioBuffer m_audioBuffer;
    //ysAudioSource* m_audioSource;

    int m_oscillatorSampleOffset;
    int m_screen;
};
