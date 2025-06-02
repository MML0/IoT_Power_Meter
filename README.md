# IoT Power Meter

A Wi-Fi-enabled IoT device and backend for real-time monitoring and logging of AC electrical parameters such as voltage, current, real power, apparent power (VA), power factor, frequency, and energy usage. Ideal for DIY smart home energy monitoring projects using ESP8266 or ESP32.

---

## 📦 Features

- ✅ Wi-Fi (ESP8266/ESP32) based data sender
- ✅ REST API for submitting and retrieving data
- ✅ MySQL backend for persistent logging
- ✅ Rebuildable database schema via `migrate.php`
- ✅ JSON interface for easy integration
- ✅ Tracks:
  - `voltage`
  - `current`
  - `power`
  - `energy`
  - `frequency`
  - `power factor`
  - `name` (device/location identifier)
  - `created_at` timestamp

---

# Feel free to fork the repo and submit pull requests. 
# This project is open source under the MIT License.