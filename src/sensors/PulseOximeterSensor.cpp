#include "PulseOximeterSensor.h"

#include "spo2_algorithm.h"
#include "system/I2cBus.h"

namespace {

constexpr byte kLedBrightness = 60;
constexpr byte kSampleAverage = 4;
constexpr byte kLedMode = 2;
constexpr int kSampleRate = 100;
constexpr int kPulseWidth = 411;
constexpr int kAdcRange = 4096;

class I2cBusLockGuard {
 public:
  I2cBusLockGuard() : locked_(lockI2cBus(portMAX_DELAY)) {}

  ~I2cBusLockGuard() {
    if (locked_) {
      unlockI2cBus();
    }
  }

  bool locked() const { return locked_; }

 private:
  bool locked_;
};

}  // namespace

PulseOximeterSensor::PulseOximeterSensor()
    : sensor_(), initialized_(false), initialBufferFilled_(false) {}

bool PulseOximeterSensor::begin() {
  I2cBusLockGuard lock;
  if (!lock.locked()) {
    return false;
  }

  if (!sensor_.begin(Wire, I2C_SPEED_FAST)) {
    return false;
  }

  sensor_.setup(kLedBrightness, kSampleAverage, kLedMode, kSampleRate,
                kPulseWidth, kAdcRange);

  initialized_ = true;
  initialBufferFilled_ = false;
  return true;
}

bool PulseOximeterSensor::waitForNewSample() {
  uint16_t attempts = 0;
  while (!sensor_.available()) {
    I2cBusLockGuard lock;
    if (!lock.locked()) {
      return false;
    }

    sensor_.check();
    ++attempts;
    if (attempts > 4000) {
      return false;
    }
    delay(1);
  }

  return true;
}

bool PulseOximeterSensor::fillBufferRange(int startIndex, int endIndex) {
  for (int i = startIndex; i < endIndex; ++i) {
    if (!waitForNewSample()) {
      return false;
    }

    redBuffer_[i] = sensor_.getFIFORed();
    irBuffer_[i] = sensor_.getFIFOIR();
    sensor_.nextSample();
  }

  return true;
}

PulseOximetryReading PulseOximeterSensor::calculateReading() {
  int32_t spo2 = 0;
  int8_t validSPO2 = 0;
  int32_t heartRate = 0;
  int8_t validHeartRate = 0;

  maxim_heart_rate_and_oxygen_saturation(
      irBuffer_, kBufferLength, redBuffer_, &spo2, &validSPO2, &heartRate,
      &validHeartRate);

  return {static_cast<int>(heartRate), static_cast<int>(spo2),
          validHeartRate != 0, validSPO2 != 0};
}

PulseOximetryReading PulseOximeterSensor::read() {
  if (!initialized_) {
    return {0, 0, false, false};
  }

  if (!initialBufferFilled_) {
    if (!fillBufferRange(0, kBufferLength)) {
      return {0, 0, false, false};
    }
    initialBufferFilled_ = true;
    return calculateReading();
  }

  for (int i = kSlideWindowStep; i < kBufferLength; ++i) {
    redBuffer_[i - kSlideWindowStep] = redBuffer_[i];
    irBuffer_[i - kSlideWindowStep] = irBuffer_[i];
  }

  if (!fillBufferRange(kBufferLength - kSlideWindowStep, kBufferLength)) {
    return {0, 0, false, false};
  }

  return calculateReading();
}
