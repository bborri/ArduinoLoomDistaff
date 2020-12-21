#pragma once

#define SerialPrintStr(str) { \
  auto pgmStr = PSTR(str); \
  for (uint8_t c, i = 0; (c = pgm_read_byte(pgmStr + i)); ++i) \
    Serial.write(c); \
}

#define SerialPrintLnStr(str) { \
  SerialPrintStr(str) \
  Serial.println(); \
}
