#include <WiFi.h>
#include <ModbusIP_ESP8266.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <NfcAdapter.h>

const char* ssid = "SFSV-WIFI-DEMO";
const char* password = "SFSV@2025#$%";

IPAddress plcIP(192,168,79,223);
const int plcPort = 502;
ModbusIP mb;

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc(pn532_i2c);

void setup() {
  Serial.begin(115200);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  // 👉 Log thông tin mạng
  Serial.print("📡 SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("📡 ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("📡 Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("📡 Subnet Mask: ");
  Serial.println(WiFi.subnetMask());

  // Khởi tạo Modbus client
  mb.client();
  Serial.println("✅ Modbus client initialized");

  // Khởi tạo NFC
  Serial.println("🔄 Initializing NFC...");
  nfc.begin();
  Serial.println("✅ NFC initialized");
}

void loop() {
  mb.task();

  // Đảm bảo kết nối PLC (giống code gốc của bạn)
  if(!mb.connect(plcIP, plcPort)) {
    mb.connect(plcIP, plcPort);
  }

  // Kiểm tra và đọc NFC tag (giống code gốc)
  if (nfc.tagPresent()) {
    Serial.println("📱 NFC tag detected!");

    NfcTag tag = nfc.read();
    if (tag.hasNdefMessage()) {
      NdefMessage msg = tag.getNdefMessage();
      NdefRecord record = msg.getRecord(0);

      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);

      // Byte 0 = độ dài chuỗi language code
      int langLength = payload[0];

      // Bỏ qua langLength ký tự đầu (ví dụ "en")
      String text = "";
      for (int i = langLength + 1; i < payloadLength; i++) {
        text += (char)payload[i];
      }

      Serial.print("NFC Payload: ");
      Serial.println(text);   // 👉 sẽ in ra "12345678"

      // ===== Gửi từng ký tự sang PLC =====
      int len = text.length();
      for (int i = 0; i < len; i++) {
        uint16_t value = text[i] - '0';  // '1' -> 1, '2' -> 2, ...

        // Kiểm tra kết quả ghi (cải tiến nhỏ)
        if (mb.writeHreg(plcIP, i, value)) {
          Serial.printf("📤 Ghi ký tự '%c' thành số %u vào Hreg[%d] - ✅\n",
                        text[i], value, i);
        } else {
          Serial.printf("📤 Ghi ký tự '%c' thành số %u vào Hreg[%d] - ❌\n",
                        text[i], value, i);
        }

        delay(100);  // tránh gửi quá nhanh
      }

      // 👉 Thêm delay để tránh đọc liên tục cùng 1 thẻ
      delay(2000);
    }
  }

  delay(200);
}