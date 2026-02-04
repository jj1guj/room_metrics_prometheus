#include <Adafruit_AHTX0.h>

class Sensor {
  public:
  Sensor() {}
  void begin() { aht.begin(); };
  void fetch() {
    aht.getEvent(&humidity_sensor, &temp_sensor);
    hum = humidity_sensor.relative_humidity;
    temp = temp_sensor.temperature;
  };

  float temp = 0, hum = 0;
  private:
  Adafruit_AHTX0 aht;
  sensors_event_t humidity_sensor, temp_sensor;
};
