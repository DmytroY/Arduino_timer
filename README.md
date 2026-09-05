# Arduino Timer with RTC, 16x2 "Large" Display and Relay

A simple, self-contained Arduino-based weekly timer that uses a DS3231 RTC, a 16x2 LCD with custom large digits, and a relay to switch external loads on/off on scheduled days and times. The project includes a custom LargeLCD helper to draw big digits on a standard 16x2 LCD, button-driven menu for time and schedule configuration, and EEPROM-backed schedule storage.

Features
- Displays the current time using large digits on a 16x2 LCD.
- Uses DS3231 RTC for accurate timekeeping (via Adafruit RTClib).
- Schedules weekly ON and OFF times (select days, hour, minute).
- Stores schedules in EEPROM to survive power cycles.
- Manual relay toggle with button.
- Simple 3-button user interface (Back, Time, Relay).
- Wokwi diagram and a schematic image included.

Repository layout
- src/
  - main.cpp — main application (UI, scheduling, RTC, EEPROM, relay control)
  - LargeLCD.cpp — implements large-digit drawing on a 16x2 LCD
- include/ — (empty)
- lib/ — (empty)
- Schematic.JPG — wiring schematic image
- diagram.json / wokwi.toml — Wokwi simulation diagram and metadata
- platformio.ini — PlatformIO configuration (env: nanoatmega328)

Hardware
- Arduino Nano (ATmega328)
- 16x2 LCD (HD44780-compatible) wired in 4-bit mode
- DS3231 RTC module (I2C)
- Relay module (5V)
- 3 x pushbuttons (active LOW, use INPUT_PULLUP)
- Optional LED driven through relay for demo

Wiring (as used in the project)
- LCD
  - RS -> D12
  - E  -> D11
  - D4 -> D10
  - D5 -> D9
  - D6 -> D8
  - D7 -> D7
  - VDD -> 5V
  - VSS -> GND
  - A/K -> backlight pins (as appropriate)
- RTC (DS3231)
  - SDA -> A4
  - SCL -> A5
  - VCC -> 5V
  - GND -> GND
- Buttons (active LOW using INPUT_PULLUP)
  - BACK -> A0
  - TIME -> A1
  - RELAY -> A2
- Relay module
  - IN -> D5
  - VCC -> 5V
  - GND -> GND
- LED (optional)
  - LED-A -> Relay VCC / NO as in schematic
  - LED-C -> Relay NO / COM per schematic

See Schematic.JPG and diagram.json (Wokwi) for the exact wiring and visual layout.

Core behavior and controls
- Main screen: shows large HH:MM.
  - LEFT button (A0): open Menu (labelled BACK on display).
  - MIDDLE button (A1): used for entering Time adjustment from Menu (labelled TIME).
  - RIGHT button (A2): toggles relay manually from main screen (labelled RELAY). In menu it enters relay scheduling.
- Menu (shows ON/OFF schedules alternating):
  - BACK (LEFT / A0): Exit menu.
  - TIME (MIDDLE / A1): Enter time-setting UI.
  - RELAY (RIGHT / A2): Enter schedule-setting UI (ON then OFF).
- Time set:
  - Walk through parameters (day, hour, minute, second) using BACK/Next.
  - +/- modify the selected field.
- Schedule set:
  - Configure ON schedule: choose days (Mon..Sun), hour, minute.
  - Then configure OFF schedule in similar way.
  - Schedules are stored in EEPROM at addresses defined in code.

Software & building
- PlatformIO
  - The included platformio.ini contains an environment for `nanoatmega328` (framework = arduino).
  - Build: pio run
  - Upload: pio run -t upload
- Arduino IDE
  - Open the project in Arduino IDE (or copy src files into a sketch).
  - Ensure libraries are installed:
    - Wire, SPI, EEPROM — built-in
    - LiquidCrystal — built-in or available via Library Manager
    - Adafruit RTClib — install via Library Manager (required)
- Dependencies (listed in platformio.ini)
  - LiquidCrystal
  - adafruit/RTClib (tested with ^2.1.3)

Configuration constants (in src/main.cpp)
- RELAY_PIN = D5
- BUT0_PIN = A0 (BACK)
- BUT1_PIN = A1 (TIME)
- BUT2_PIN = A2 (RELAY / Toggle)
- EEPROM addresses:
  - RELAY_ON_EEPROM_ADDR = 0
  - RELAY_OFF_EEPROM_ADDR = 4

Data structures
- ScheduleBitmask (in main.cpp)
  - uint8_t daysMask — bitmask for days (bits 1..7 -> Mon..Sun)
  - uint8_t hour, minute
- The code reads/writes ScheduleBitmask to EEPROM using EEPROM.get()/put().

Wokwi simulation
- The repo contains diagram.json and wokwi.toml for Wokwi — you can import/run the simulation on Wokwi (https://wokwi.com) using the included files.

EEPROM behavior & defaults
- On first run or if EEPROM has garbage, the code initializes default schedules:
  - ON time: 07:00, no days selected by default (daysMask = 0)
  - OFF time: 08:00, no days selected by default (daysMask = 0)
- EEPROM.put is used (writes only changed bytes to reduce wear).

Troubleshooting
- RTC not detected: The boot code calls rtc.begin(); if it fails the LCD shows "RTC Error!" and the program halts. Check wiring (SDA/A4, SCL/A5) and module power.
- Buttons not responsive: Verify buttons are wired to use INPUT_PULLUP and wired to ground on press (active LOW). Debouncing is basic (software delay) — ensure wiring is stable.
- LCD display odd characters: Ensure contrast potentiometer and wiring are correct; LargeLCD uses custom characters — LCD must be in 16x2 mode.

Contributing
- Improvements welcome: better debouncing, richer UI, multi-schedule support, relay state persistence, web or serial configuration UI, or I2C LCD support.
- If you open issues or PRs, include hardware used and steps to reproduce.

License
- No license file included in the repository. If you want others to use or contribute, consider adding an open-source license (e.g., MIT).

Acknowledgements
- Uses Adafruit RTClib for DS3231 support.
- LCD logic inspired by custom large-digit approaches for HD44780 displays.

Contact
- Repository: https://github.com/DmytroY/Arduino_timer
