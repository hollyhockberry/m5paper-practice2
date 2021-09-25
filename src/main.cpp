#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include <M5GFX.h>

/*

/img のファイルをSDカードのルートにコピーします。
inaba.bmp 540x540 24bit 
inaba2.bmp 270x270 24bit

ボタン G37で /inaba.bmp, G39で /inaba2.bmp で createFromBmpFile()します。

再現方法
* inaba.bmpだけ何度もcreateFromBmpFileしてもabort()はしない
* inaba2.bmpを開いた後だとどのファイルを開いてもabort()する

*/

namespace {

void drawImage(const String& path, M5Canvas& canvas) {
  Serial.printf("open: %s\r\n", path.c_str());
  canvas.deleteSprite();
  canvas.deletePalette();
  if (path.endsWith("bmp") || path.endsWith("BMP")) {
    canvas.createFromBmpFile(SD, path.c_str());
  } else {
    // TODO
  }
  Serial.printf("(%d, %d)\r\n", canvas.width(), canvas.height());
  canvas.pushSprite(0, 300);
}

M5GFX display;
M5Canvas canvas(&display);

}  // namespace

void setup() {
  Serial.begin(115200);
  SPI.begin(14, 13, 12, 4);
  SD.begin(4, SPI, 20000000);

  ::pinMode(37, INPUT);
  ::pinMode(39, INPUT);

  display.begin();
  display.setEpdMode(epd_mode_t::epd_quality);
  display.invertDisplay(false);
  display.clear(TFT_WHITE);
  display.setRotation(0);
}

void loop() {
  const char* files[] = { "/inaba.bmp", "/inaba2.bmp" };

  if (::digitalRead(37) == LOW) {
    drawImage(String(files[0]), canvas);
  } else if (::digitalRead(39) == LOW) {
    drawImage(String(files[1]), canvas);
  } else {
    return;
  }
}
