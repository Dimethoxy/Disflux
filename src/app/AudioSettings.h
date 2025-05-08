#include <DmtHeader.h>
#include <JuceHeader.h>

struct dmt::Settings::Audio
{
  static inline auto& useOutputHighpass =
    container.add<bool>("Audio.UseOutputLowpass", true);

  static inline auto& outputHighpassFrequency =
    container.add<float>("Audio.OutputLowpassFrequency", 20.0f);

  static inline auto& frequencySmoothness =
    container.add<float>("Audio.FrequencySmoothness", 0.2f);

  static inline auto& spreadSmoothness =
    container.add<float>("Audio.SpreadSmoothness", 0.02f);

  static inline auto& pinchSmoothness =
    container.add<float>("Audio.PinchSmoothness", 0.02f);

  static inline auto& mixSmoothness =
    container.add<float>("Audio.MixSmoothness", 0.02f);

  static inline auto& smoothingInterval =
    container.add<int>("Audio.SmoothingInterval", 32);
};
