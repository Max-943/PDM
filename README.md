# ATmega1284P Motorsport PDM Firmware - Test and DBC Generated Edition

## Important safety rule
`TEST_MODE_ENABLED` is disabled by default. Enable it only on a current-limited bench with motors, pumps, and vehicle loads disconnected.

## Install generator
```powershell
python -m pip install -r requirements.txt
```

## DBC workflow
Edit only `dbc/pdm.dbc`, then build. PlatformIO runs `scripts/generate_dbc.py` and regenerates:
- `include/generated/pdm_dbc.h`
- `src/generated/pdm_dbc.c`

Bit positions, IDs, lengths, byte order, and raw packing follow the DBC automatically as long as the existing message and signal names remain compatible with the small application adapter in `can_messages.cpp`.

## Enable bench test console
Add this line under `build_flags` in `platformio.ini`:
```ini
    -DTEST_MODE_ENABLED=1
```
Open the MCP2221A terminal at 57600 baud and type `help`.

## Disable one feature during testing
Add one or more:
```ini
    -DFEATURE_CAN_ENABLED=0
    -DFEATURE_EFUSE_ENABLED=0
    -DFEATURE_TEMPERATURE_ENABLED=0
    -DFEATURE_SUPPLY_VOLTAGE_ENABLED=0
    -DFEATURE_BUTTONS_ENABLED=0
    -DFEATURE_OUTPUTS_ENABLED=0
    -DFEATURE_LEDS_ENABLED=0
    -DFEATURE_WATCHDOG_ENABLED=0
```

## Before powered-load testing
Confirm NTC R25/Beta, AVCC, IMON calibration, JTAG disabled, current limits, voltage-divider clamping, and safe hardware ILIM.
