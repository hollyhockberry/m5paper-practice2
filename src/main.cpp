// Copyright (c) 2021 Inaba
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <vector>
#include <M5EPD.h>
#include <M5GFX.h>

namespace {

void listFiles(std::vector<String>& files, File& file) {
  while (true) {
    File entry = file.openNextFile();
    if (!entry) {
      return;
    }
    if (entry.isDirectory()) {
      // ignore dot directories
      if (entry.name()[1] != '.') {
        listFiles(files, entry);
      }
    } else {
      files.push_back(String(entry.name()));
    }
    entry.close();
  }
}


// Draw a file list on Canvas.
// INFO: Cannot display too many files :-)
void drawFilelist(std::vector<String>& files, int index, M5GFX& display) {
  display.setEpdMode(epd_mode_t::epd_fastest);
  M5Canvas canvas(&display);
  canvas.setPsram(true);
  canvas.createSprite(540, 300);
  canvas.setTextSize(3);
  canvas.clear(TFT_WHITE);

  int i = 0;
  for (const auto& file : files) {
    if (index == i) {
      canvas.fillRect(0, 8 + i * 32, 540, 32, TFT_BLACK);
      canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    } else {
      canvas.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    canvas.drawString(file.c_str(), 10, 10 + i * 32);
    ++i;
  }
  canvas.pushSprite(0, 0);
}


void drawImage(const String& path, M5GFX& display) {
  if (!path.endsWith("bmp") && !path.endsWith("BMP")) {
    return;
  }
  display.setEpdMode(epd_mode_t::epd_quality);
  M5Canvas canvas(&display);
  canvas.setPsram(true);
  canvas.createFromBmpFile(SD, path.c_str());
  const float dx = 350.f / static_cast<float>(canvas.width());
  const float dy = 350.f / static_cast<float>(canvas.height());
  canvas.pushRotateZoomWithAA(270, 630, 0.f, dx, dy);
}

M5GFX display;

std::vector<String> files;
int fileIndex = 0;

}  // namespace

void setup() {
  M5.begin();

  display.begin();
  display.invertDisplay(false);
  display.clear(TFT_WHITE);
  display.setRotation(0);

  File root = SD.open("/");
  listFiles(files, root);

  drawFilelist(files, fileIndex, display);
}

void loop() {
  M5.update();

  if (M5.BtnL.wasPressed()) {
    fileIndex--;
  } else if (M5.BtnR.wasPressed()) {
    fileIndex++;
  } else if (M5.BtnP.wasPressed()) {
    drawImage(files[fileIndex], display);
    return;
  } else {
    return;
  }
  fileIndex = constrain(fileIndex, 0, files.size() - 1);
  drawFilelist(files, fileIndex, display);
}
