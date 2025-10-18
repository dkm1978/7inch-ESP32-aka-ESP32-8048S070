#include <7inch_board.h>
#include "WiFi.h"

#define SCAN_INTERVAL 3000

int16_t w, h, text_size, banner_height, graph_baseline, graph_height, channel_width, signal_width;

// RSSI RANGE
#define RSSI_CEILING -40
#define RSSI_FLOOR -100

// Channel color mapping from channel 1 to 14
uint16_t channel_color[] = {
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA};

uint8_t scan_count = 0;

void setup()
{
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);


  // init LCD
  BoardInit(true,255);
  w = Screen->width();
  h = Screen->height();
  text_size = (h < 200) ? 1 : 2;
  banner_height = text_size * 3 * 4;
  graph_baseline = h - 20;                            // minus 2 text lines
  graph_height = graph_baseline - banner_height - 30; // minus 3 text lines
  channel_width = w / 17;
  signal_width = channel_width * 2;

  // init banner
  Screen->setTextSize(text_size);
  Screen->fillScreen(BLACK);
  Screen->setTextColor(RED);
  Screen->setCursor(0, 0);
  Screen->print("ESP");
  Screen->setTextColor(WHITE);
  Screen->print(" WiFi Analyzer");
}

bool matchBssidPrefix(uint8_t *a, uint8_t *b)
{
  for (uint8_t i = 0; i < 5; i++)
  { // only compare first 5 bytes
    if (a[i] != b[i])
    {
      return false;
    }
  }
  return true;
}

void loop()
{
  uint8_t ap_count_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int32_t noise_list[] = {RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR};
  int32_t peak_list[] = {RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR};
  int16_t peak_id_list[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  int32_t channel;
  int16_t idx;
  int32_t rssi;
  uint8_t *bssid;
  String ssid;
  uint16_t color;
  int16_t height, offset, text_width;
  int n = WiFi.scanNetworks(false /* async */, true /* show_hidden */, true /* passive */, 500 /* max_ms_per_chan */);

  // clear old graph
  Screen->fillRect(0, banner_height, w, h - banner_height, BLACK);
  Screen->setTextSize(1);

  if (n == 0)
  {
    Screen->setTextColor(WHITE);
    Screen->setCursor(0, banner_height);
    Screen->println("no networks found");
  }
  else
  {
    for (int i = 0; i < n; i++)
    {
      channel = WiFi.channel(i);
      idx = channel - 1;
      rssi = WiFi.RSSI(i);
      bssid = WiFi.BSSID(i);

      // channel peak stat
      if (peak_list[idx] < rssi)
      {
        peak_list[idx] = rssi;
        peak_id_list[idx] = i;
      }

      // check signal come from same AP
      bool duplicate_SSID = false;
      for (int j = 0; j < i; j++)
      {
        if ((WiFi.channel(j) == channel) && matchBssidPrefix(WiFi.BSSID(j), bssid))
        {
          duplicate_SSID = true;
          break;
        }
      }

      if (!duplicate_SSID)
      {
        ap_count_list[idx]++;

        // noise stat
        int32_t noise = rssi - RSSI_FLOOR;
        noise *= noise;
        if (channel > 4)
        {
          noise_list[idx - 4] += noise;
        }
        if (channel > 3)
        {
          noise_list[idx - 3] += noise;
        }
        if (channel > 2)
        {
          noise_list[idx - 2] += noise;
        }
        if (channel > 1)
        {
          noise_list[idx - 1] += noise;
        }
        noise_list[idx] += noise;
        if (channel < 14)
        {
          noise_list[idx + 1] += noise;
        }
        if (channel < 13)
        {
          noise_list[idx + 2] += noise;
        }
        if (channel < 12)
        {
          noise_list[idx + 3] += noise;
        }
        if (channel < 11)
        {
          noise_list[idx + 4] += noise;
        }
      }
    }

    // plot found WiFi info
    for (int i = 0; i < n; i++)
    {
      channel = WiFi.channel(i);
      idx = channel - 1;
      rssi = WiFi.RSSI(i);
      color = channel_color[idx];
      height = constrain(map(rssi, RSSI_FLOOR, RSSI_CEILING, 1, graph_height), 1, graph_height);
      offset = (channel + 1) * channel_width;

      // trim rssi with RSSI_FLOOR
      if (rssi < RSSI_FLOOR)
      {
        rssi = RSSI_FLOOR;
      }

      // plot chart
      // Screen->drawLine(offset, graph_baseline - height, offset - signal_width, graph_baseline + 1, color);
      // Screen->drawLine(offset, graph_baseline - height, offset + signal_width, graph_baseline + 1, color);
      Screen->startWrite();
      Screen->drawEllipseHelper(offset, graph_baseline + 1, signal_width, height, 0b0011, color);
      Screen->endWrite();

      if (i == peak_id_list[idx])
      {
        // Print SSID, signal strengh and if not encrypted
        String ssid = WiFi.SSID(i);
        if (ssid.length() == 0)
        {
          ssid = WiFi.BSSIDstr(i);
        }
        text_width = (ssid.length() + 6) * 6;
        if (text_width > w)
        {
          offset = 0;
        }
        else
        {
          offset -= signal_width;
          if ((offset + text_width) > w)
          {
            offset = w - text_width;
          }
        }
        Screen->setTextColor(color);
        Screen->setCursor(offset, graph_baseline - 10 - height);
        Screen->print(ssid);
        Screen->print('(');
        Screen->print(rssi);
        Screen->print(')');
#if defined(ESP32)
        if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)
#else
        if (WiFi.encryptionType(i) == ENC_TYPE_NONE)
#endif
        {
          Screen->print('*');
        }
      }
    }
  }

  // print WiFi stat
  Screen->setTextColor(WHITE);
  Screen->setCursor(0, banner_height);
  Screen->print(n);
  Screen->print(" networks found, lesser noise channels: ");
  bool listed_first_channel = false;
  int32_t min_noise = noise_list[0];          // init with channel 1 value
  for (channel = 2; channel <= 11; channel++) // channels 12-14 may not available
  {
    idx = channel - 1;
    log_i("min_noise: %d, noise_list[%d]: %d", min_noise, idx, noise_list[idx]);
    if (noise_list[idx] < min_noise)
    {
      min_noise = noise_list[idx];
    }
  }

  for (channel = 1; channel <= 11; channel++) // channels 12-14 may not available
  {
    idx = channel - 1;
    // check channel with min noise
    if (noise_list[idx] == min_noise)
    {
      if (!listed_first_channel)
      {
        listed_first_channel = true;
      }
      else
      {
        Screen->print(", ");
      }
      Screen->print(channel);
    }
  }

  // draw graph base axle
  Screen->drawFastHLine(0, graph_baseline, 320, WHITE);
  for (channel = 1; channel <= 14; channel++)
  {
    idx = channel - 1;
    offset = (channel + 1) * channel_width;
    Screen->setTextColor(channel_color[idx]);
    Screen->setCursor(offset - ((channel < 10) ? 3 : 6), graph_baseline + 2);
    Screen->print(channel);
    if (ap_count_list[idx] > 0)
    {
      Screen->setCursor(offset - ((ap_count_list[idx] < 10) ? 9 : 12), graph_baseline + 8 + 2);
      Screen->print('{');
      Screen->print(ap_count_list[idx]);
      Screen->print('}');
    }
  }

  // Wait a bit before scanning again
  delay(SCAN_INTERVAL);

#if defined(SCAN_COUNT_SLEEP)
  // POWER SAVING
  if (++scan_count >= SCAN_COUNT_SLEEP)
  {
#if defined(LCD_PWR_PIN)
    pinMode(LCD_PWR_PIN, INPUT); // disable pin
#endif

#if defined(GFX_BL)
    pinMode(GFX_BL, INPUT); // disable pin
#endif

#if defined(ESP32)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, LOW);
    esp_deep_sleep_start();
#else
    ESP.deepSleep(0);
#endif
  }
#endif // defined(SCAN_COUNT_SLEEP)
}
