#include <DmtHeader.h>
#include <JuceHeader.h>

struct dmt::Settings::Audio
{
  static inline auto& useOutputHighpass =
    container.add<bool>("Audio.UseOutputLowpass", true);

  constexpr static float MIN_LOW_PASS_FREQ = 20.0f;
  constexpr static float MAX_LOW_PASS_FREQ = 20000.0f;
  static inline auto& outputHighpassFrequency =
    container.add<float>("Audio.OutputLowpassFrequency",
                         20.0f,
                         &MIN_LOW_PASS_FREQ,
                         &MAX_LOW_PASS_FREQ);

  constexpr static float MIN_FRQUENCY_SMOOTHNESS = 0.0f;
  constexpr static float MAX_FRQUENCY_SMOOTHNESS = 1.0f;
  static inline auto& frequencySmoothness =
    container.add<float>("Audio.FrequencySmoothness",
                         0.2f,
                         &MIN_FRQUENCY_SMOOTHNESS,
                         &MAX_FRQUENCY_SMOOTHNESS);

  constexpr static float MIN_SPREAD_SMOOTHNESS = 0.0f;
  constexpr static float MAX_SPREAD_SMOOTHNESS = 1.0f;
  static inline auto& spreadSmoothness =
    container.add<float>("Audio.SpreadSmoothness", 0.02f);

  constexpr static float MIN_PINCH_SMOOTHNESS = 0.0f;
  constexpr static float MAX_PINCH_SMOOTHNESS = 1.0f;
  static inline auto& pinchSmoothness =
    container.add<float>("Audio.PinchSmoothness", 0.02f);

  constexpr static float MIN_MIX_SMOOTHNESS = 0.0f;
  constexpr static float MAX_MIX_SMOOTHNESS = 1.0f;
  static inline auto& mixSmoothness =
    container.add<float>("Audio.MixSmoothness", 0.02f);

  constexpr static int MIN_SMOOTHING_INTERVAL = 1;
  constexpr static int MAX_SMOOTHING_INTERVAL = 64;
  static inline auto& smoothingInterval =
    container.add<int>("Audio.SmoothingInterval", 32);
};
