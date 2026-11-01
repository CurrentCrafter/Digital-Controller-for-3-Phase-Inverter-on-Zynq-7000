# Digital-Controller-for-3-Phase-Inverter-on-Zynq-7000

Bare-metal C firmware (Xilinx **Vitis**) implementing the real-time control path of a
**grid-connected three-phase inverter** on a **Xilinx Zynq-7000 SoC** (Bachelor's thesis project).

The FPGA fabric (Vivado side, not part of this repository) handles delta-sigma current
measurement with Sinc³ filtering, a **SOGI-PLL** for grid synchronization, sine-PWM
modulation, and exposes its data to the processor through **AXI BRAM / AXI GPIO**.
This repository contains the application running on the two ARM Cortex-A9 cores
that closes the control loop on top of that hardware.

---

## Features

- **96 kHz real-time control loop** — timer-triggered ISR (`Ts = 10.4167 µs`,
  half of the 48 kHz PWM switching frequency), see `Praktikum.c`
- **abc → αβ → dq coordinate transformations** (`DQ.c`) — converts AC quantities
  into a rotating reference frame so DC-shaped PI control can be used
- **Grid synchronization** — reads grid angle θ, estimated grid frequency ω and
  PLL lock status from the SOGI-PLL implemented in the FPGA (`HARDWARE.c`, `MEAS.c`)
- **Measurement acquisition** — samples all delta-sigma channels from BRAM and
  scales them into physical float values (`MEAS.c`)
- **Error handling & protection** — DC/grid/converter voltage-current limits,
  temperature supervision, modulator disable path (`ERROR.c`)
- **Fan control** — temperature-dependent PWM fan modulation (`HARDWARE.c`)
- **UART host interface** — 115200 8N1, interrupt-driven ring buffers,
  command processing for a PC GUI (parameter tuning, live values, debug capture) (`UART.c`, `GUI.c`)
- **DAC debug output** — writes up to 4 internal control signals to an SPI DAC
  so they can be observed on an oscilloscope with optional trigger (`SPI.c`)
- **PC control console included** — Python GUI in [`gui/`](gui/) for
  connecting, monitoring and commanding the inverter from a laptop
  (pre-built Windows binary included)

---

## Architecture

```
        ┌───────────────────────────  FPGA fabric (Vivado)  ───────────────────────────┐
        │  ΔΣ ADC → Sinc³ filter   SOGI-PLL (θ, ω, locked)   3-phase Sine-PWM modulator|
        │  Error latches / limits               Fan + aggressor logic                  │
        └───────────────┬─────────────────────────────┬────────────────────────────────┘
                        │ AXI BRAM (measurement data) │ AXI GPIO (control / status)
        ┌───────────────▼─────────────────────────────▼────────────────────────────────┐
        │                     Zynq PS — bare-metal C application                       │
        │                                                                              │
        │   ISR0 @ 96 kHz (Praktikum.c) ── the real-time path:                         │
        │      1. Read_all_Measurments()   BRAM → physically scaled floats             │
        │      2. if (Modulator_Enable && SOGI_locked)  else disable modulator         │
        │      3. dq transforms + control loops            (user control code)         │
        │      4. write_dutycycle(duty_1, duty_2, duty_3)  PWM values → registers      │
        │      5. oszi_write_all()          debug signals → DAC → oscilloscope         │
        │                                                                              │
        │   main() while(1) ── the non-real-time path:                                 │
        │      slow measurements, fan control, UART command processing, GUI protocol   │
        └──────────────────────────────────────────────────────────────────────────────┘
```

---

## System purpose & core components

The C code implements the control software for a **grid-connected three-phase
converter** on a Xilinx Zynq SoC. The system regulates currents and voltages
in **real time** and synchronizes itself with the power grid using a
**SOGI-PLL** (Second Order Generalized Integrator).

### 1. Control loop (real-time)

The central control takes place in the interrupt service routine `isr0` in
`Praktikum.c`:

- **Frequency:** the sampling time `Ts` is approximately 10.4 µs
  (equivalent to 96 kHz — twice the 48 kHz switching frequency).
- **Sequence:** Measure → Transform → Regulate → Write PWM values.

### 2. Signal processing & transformations

To simplify the control of AC quantities, they are transformed into a rotating
coordinate system (dq frame):

- **Transformations:** implemented in `DQ.c` — currents and voltages are
  converted from abc coordinates to dq coordinates
  (`dq_Transformation_I_Conv`, `dq_Transformation_U_Conv`).
- **SOGI-PLL:** estimates the grid frequency, the phase angle (`theta`) and
  the amplitude. The raw data is read from the BRAM and processed in
  `Read_all_Measurments` *(sic — kept to match the code)*, orchestrated by
  `HARDWARE.c`.

### 3. Measurement acquisition

The hardware (FPGA logic) writes the ADC values into Block RAM (BRAM):

- The CPU reads these values in `MEAS.c` and scales them with factors such as
  `PROP_I_AC` or `PROP_UGRID_L1` into physical floating-point values.

### 4. Debugging & interfaces

- **DAC / oscilloscope:** internal control variables are output via SPI to a
  DAC, so they can be visualized on an external oscilloscope (`SPI.c`).
- **UART / GUI:** a serial interface enables communication with the PC GUI
  ([`gui/`](gui/)) for parameter changes and status monitoring.

---

## Repository structure

```
├── src/               Firmware for the Zynq PS (built with Xilinx Vitis)
│   ├── Praktikum.c        isr0() — the 96 kHz control interrupt (user control loop)
│   ├── MASTER.c / .h      main(), global state, shared structs, GPIO channel defines
│   ├── HARDWARE.c / .h    Init, GPIO, interrupts, modulator, relays, fan, SOGI readout,
│   │                      write_dutycycle()
│   ├── MEAS.c / .h        BRAM access, raw-data → float scaling, temperature reading
│   ├── DQ.c / .h          Clarke/Park transformations (converter & grid side)
│   ├── DEFINE.h           Global constants: sampling time, scaling factors, delays
│   ├── ERROR.c / .h       Fault detection, error codes, modulator enable/disable
│   ├── UART.c / .h        Interrupt-driven UART driver with TX/RX ring buffers
│   ├── GUI.c / .h         ASCII command protocol: state, features, variables, debug dumps
│   ├── SPI.c / .h         DAC output for oscilloscope debugging (4 channels + trigger)
│   ├── DEBUG.c / .h       Debug helpers (value logging, SOGI debug capture)
│   └── platform.c/.h      Vitis platform init (cache, UART stdout)
│     
└── gui/               PC control console (connects to the firmware via UART)
    ├── README.md          GUI documentation + UART protocol + error-code table
    ├── Konsole.py         Main application (Python 3, tkinter + pyserial)
    ├── Konsole_Demo.py    Demo build, runs without hardware
    ├── requirements.txt   pip dependencies
    ├── release/           Pre-built Windows binary
    └── docs/              Original German documentation (`README_original_de.txt`)
```

---

## Key data structures

| Struct (declared in `MASTER.h`) | Purpose |
|---|---|
| `Measurments` | All physically scaled measurements: filter/grid currents, filter/grid voltages, DC-link, temperature |
| `dq_values` | Result of the rotating-frame transformations (d/q currents & voltages) |
| `sogi_data` | PLL state: normalized sin/cos, frequency estimate, grid angle θ, lock flag |
| `oszi` | 4 debug channels (+ per-channel voltage range, trigger) forwarded to the DAC |
| `Debug_values` / `Debug_sogi` | 2000-sample ring captures for UART-based waveform debugging |

---

## PC control console (`gui/`)

The companion GUI lets you operate the lab bench from a laptop: pick a COM port,
connect, enable/disable the modulator, acknowledge errors, toggle test features,
and watch the live status (error code in plain text, SOGI lock, feature states).

```bash
pip install -r gui/requirements.txt
python gui/Konsole.py        # real hardware
python gui/Konsole_Demo.py   # demo — try it without any hardware connected
```

Windows users can simply run the pre-built binary: `gui/release/UART_Modulator_Control.exe`.
Full protocol documentation (commands, status frames, error-code table) is in
[`gui/README.md`](gui/README.md).

---

## Repository scope

This repository contains the **PC-side and PS-side application sources** of the
thesis project. It intentionally does not include:

- the Vivado/Vitis project and BSP artifacts (`.xsa`, `xparameters.h`, generated drivers)
- the KiCad PCB designs, calculation spreadsheets and documentation PDFs
  that exist in the private project workspace
---

> ⚠️ **Safety notice** — This firmware drives a **grid-connected power converter**.
> Working with mains-level voltages is dangerous and requires qualified personnel,
> proper insulation, test-bench equipment
> and independent hardware protection. This code is published for educational
> purposes and **without any warranty** (see section below).

---

## Acknowledgments

Developed as part of a Bachelor's thesis project on an Zynq-7000
inverter test bench (delta-sigma current sensing, SOGI-PLL grid
synchronization, lab GUI interface). The FPGA-side design is documented in the
accompanying VHDL sources.

---

## License

Distributed under the MIT Licence.

> ⚠️**Important:**
> The bundled Xilinx platform files (`src/platform.c/h`) remain under their
> original BSD license by Xilinx, Inc.
