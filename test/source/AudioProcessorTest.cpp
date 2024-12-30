#include <gtest/gtest.h>
#include <PhysicalModellingFan/PluginProcessor.h>

namespace audio_plugin_test {
    TEST(AudioPluginAudioProcessor, should_create) {
        AudioPluginAudioProcessor();
        ASSERT_TRUE(true);
    }
}