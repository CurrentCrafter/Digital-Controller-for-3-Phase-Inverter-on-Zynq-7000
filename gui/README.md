# LSPW Konsole — PC Control GUI (Serial)

Windows/Linux desktop application used to **configure, monitor and debug the
inverter** over the board's UART interface (**115200 baud, 8N1**).

Written in **Python 3** using *tkinter* (standard library) and *pyserial*.
A pre-built Windows executable is included in [`release/`](release/) for
quick use without a Python installation.

---

## Features

- 🔌 **COM-port auto-detection** and one-click Connect/Disconnect
- 🎛 **Modulator control** — Enable / Disable, Acknowledge Error
- 🔀 **Feature toggles** — switch `feature1` / `feature2` test signals on the firmware side
- 🔢 **Variable adjustment** — increment/decrement a live control variable
- 📊 **Live status display** — decoded error code, SOGI lock state, feature states
- 🧪 `Konsole_Demo.py` — a self-contained demo build that runs **without hardware**

## Screenshot

```
┌─────────────────────────────────────────────┐
│  LSPW Konsole                        [_][X] │
│                                             │
│  COM-Port [▼ COM3  ]   [ Connect ]          │
│                                             │
│  Current Error : idle                       │
│  SOGI Locked   : 1                          │
│  Feature 1     : Low      Feature 2 : Low   │
│  Variable      : 5                          │
│                                             │
│  [Enable Modulator]  [Disable Modulator]    │
│  [Ackn_Error]        [Toggle Feature 1/2]   │
│  [Variable −1]       [Variable +1]          │
└─────────────────────────────────────────────┘
```

## Running it

**From source (Windows / Linux):**

```bash
pip install pyserial
python Konsole.py          # real hardware
python Konsole_Demo.py     # demo, no hardware required
```

**Windows, no Python:** run `release/UART_Modulator_Control.exe`.

---

## UART protocol

All frames are wrapped in `<...>` and terminated with `\n` (LF).
Every command is sent **twice with a 50 ms pause** for robustness.

### Commands sent by the GUI

| Command | Frame |
|---|---|
| Enable Modulator | `<SET_EN_MOD = 1>` |
| Disable Modulator | `<SET_EN_MOD = 0>` |
| Acknowledge error | `<SET_ACKN_ERR>` |
| Feature 1 on/off | `<feature1=1>` / `<feature1=0>` |
| Feature 2 on/off | `<feature2=1>` / `<feature2=0>` |
| Set variable | `<variable=X>` |

### Messages received by the GUI

| Message | Meaning |
|---|---|
| `<VALUE_ERROR_CODE=X>` | Error code 0–22, displayed as text |
| `<SOGI_LOCKED=X>` | Grid PLL lock flag (0/1) |
| `<feature1=X>` / `<feature2=X>` | Feature test-signal states (0/1) |
| `<variable=X>` | Current variable value |

### Error codes

| Code | Meaning | Code | Meaning |
|---|---|---|---|
| 0 | reset state | 12 | DS_U_Conv_Error |
| 1 | idle | 13 | DS_I_Ph1_Error |
| 2 | on state | 14 | DS_I_Ph2_Error |
| 3 | notEnabledOnState | 15 | DS_I_Ph3_Error |
| 4 | OV_UDC_Error | 16 | DS_DC_Error |
| 5 | OC_IDC_Error | 17 | nFLT_LS_Error |
| 6 | OV_U_Grid_Error | 18 | nFLT_HS_Error |
| 7 | OC_I_Grid_Error | 19 | RDY_HS_Error |
| 8 | OV_U_Conv_Error | 20 | RDY_LS_Error |
| 9 | OC_I_Conv_Error | 21 | nPG_Supp_HS_Error |
| 10 | DS_U_Grid_Error | 22 | nPG_Supp_LS_Error |

*(OV = overvoltage, OC = overcurrent, DS = delta-sigma signal error, FLT = gate fault, RDY = not ready, nPG = power-good missing; HS/LS = high-side/low-side.)*

The command protocol is implemented on the firmware side in
[`src/UART.c`](../src/UART.c) and [`src/GUI.c`](../src/GUI.c).

---

## Files

| File | Purpose |
|---|---|
| `Konsole.py` | Main GUI application with real UART connection |
| `Konsole_Demo.py` | Demo application (no hardware required) |
| `release/UART_Modulator_Control.exe` | Pre-built Windows binary |
| `docs/README_original_de.txt` | Original German documentation |

## Requirements

- Python 3.x with `tkinter` (bundled with standard Python) and `pyserial`
  — or the included Windows executable
