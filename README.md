Phần cứng sử dụng:
- Mạch RFID NFC 13.56Mhz PN532
- ESP32
Hướng dẫn sử dụng:
- Đầu tiên tải các thư viện: ModbusIP_ESP8266, PN532_I2C, NfcAdapter
  ModbusIP_ESP8266: https://github.com/emelianov/modbus-esp8266
  PN532_I2C, NfcAdapter: [https://github.com/elechouse/PN532](https://github.com/Seeed-Studio/Seeed_Arduino_NFC)
- Sử dụng giao thức I2C để giao tiếp giữa ESP32 và RFID
- ESP32 đóng vai trò là Client còn PLC là Server
