import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import threading
import time
import re

# Fehlercode-Mapping
error_codes = {
    0: "reset state",
    1: "idle",
    2: "on state",
    3: "notEnabledOnState", 
    4: "OV_UDC_Error",
    5: "OC_IDC_Error",
    6: "OV_U_Grid_Error",
    7: "OC_I_Grid_Error", 
    8: "OV_U_Conv_Error",
    9: "OC_I_Conv_Error",
    10: "DS_U_Grid_Error",
    12: "DS_U_Conv_Error",
    13: "DS_I_Ph1_Error",
    14: "DS_I_Ph2_Error",
    15: "DS_I_Ph3_Error",
    16: "DS_DC_Error",
    17: "nFLT_LS_Error",
    18: "nFLT_HS_Error",
    19: "RDY_HS_Error",
    20: "RDY_LS_Error",
    21: "nPG_Supp_HS_Error",
    22: "nPG_Supp_LS_Error"
}

# Globale Variable für serielle Verbindung
ser = None

# GUI-Komponenten
error_label = None
sogi_locked_label = None
feature1_label = None
feature2_label = None
variable_label = None

# Feature-Status
feature1_state = False
feature2_state = False
variable_value = 0

# Serielle Ports auflisten
def get_serial_ports():
    return [port.device for port in serial.tools.list_ports.comports()]

# Serielle Schnittstelle lesen (nur Fehlercode)
def read_serial():
    buffer = ""
    while ser and ser.is_open:
        try:
            data = ser.read(ser.in_waiting or 1).decode("utf-8", errors="ignore")
            if not data:
                time.sleep(0.01)
                continue
            buffer += data
            while "<" in buffer and ">" in buffer:
                start = buffer.find("<")
                end = buffer.find(">", start)
                if end == -1:
                    break
                message = buffer[start:end+1]
                buffer = buffer[end+1:]

                # Nur Fehlercode verarbeiten
                if err_match := re.match(r"<VALUE_ERROR_CODE=(\d+)>", message):
                    error_code = int(err_match.group(1))
                    # Fehlername ermitteln
                    error_name = error_codes.get(error_code, f"Unknown error ({error_code})")
                    # In GUI anzeigen
                    error_label.config(text=error_name)
                    # In Konsole ausgeben
                    print(f"[UART] Recieved Errorcode: {error_code} ({error_name})")
                
                # SOGI_LOCKED verarbeiten
                if sogi_match := re.match(r"<SOGI_LOCKED=(\d)>", message):
                    sogi_value = sogi_match.group(1)
                    # In GUI anzeigen
                    sogi_locked_label.config(text=sogi_value)
                    # In Konsole ausgeben
                    print(f"[UART] Recieved SOGI_LOCKED value: {sogi_value}")
                
                # Feature1 verarbeiten
                if feature1_match := re.match(r"<feature1=(\d)>", message):
                    feature1_value = int(feature1_match.group(1))
                    status_text = "High" if feature1_value == 1 else "Low"
                    feature1_label.config(text=status_text)
                    print(f"[UART] Recieved Feature1: {status_text}")
                
                # Feature2 verarbeiten
                if feature2_match := re.match(r"<feature2=(\d)>", message):
                    feature2_value = int(feature2_match.group(1))
                    status_text = "High" if feature2_value == 1 else "Low"
                    feature2_label.config(text=status_text)
                    print(f"[UART] Recieved Feature2: {status_text}")
                
                # Variable verarbeiten
                if variable_match := re.match(r"<variable=(-?\d+)>", message):
                    global variable_value
                    variable_value = int(variable_match.group(1))
                    variable_label.config(text=str(variable_value))
                    print(f"[UART] Recieved Variable: {variable_value}")
        except Exception as e:
            print(f"Serial error: {e}")
        time.sleep(0.01)

# Hilfsfunktion: Nachricht doppelt senden 
def send_message_twice(message):
    if ser and ser.is_open:
        for _ in range(2):
            ser.write((message + "\n").encode("utf-8"))
            time.sleep(0.05)

# Hilfsfunktion: Nachricht einmal senden
def send_message_once(message):
    if ser and ser.is_open:
        ser.write((message + "\n").encode("utf-8"))

# Button-Aktionen
def send_enable():
    print("[GUI] Send: Enable modulator")
    threading.Thread(target=send_message_twice, args=("<SET_EN_MOD = 1>",), daemon=True).start()

def send_disable():
    print("[GUI] Send: Disable modulator")
    threading.Thread(target=send_message_twice, args=("<SET_EN_MOD = 0>",), daemon=True).start()

def send_ack_error():
    print("[GUI] Send: Acknowledge error")
    threading.Thread(target=send_message_twice, args=("<SET_ACKN_ERR>",), daemon=True).start()

def toggle_feature1():
    global feature1_state
    feature1_state = not feature1_state
    value = 1 if feature1_state else 0
    print(f"[GUI] Toggle Feature 1: {value}")
    threading.Thread(target=send_message_twice, args=(f"<feature1={value}>",), daemon=True).start()

def toggle_feature2():
    global feature2_state
    feature2_state = not feature2_state
    value = 1 if feature2_state else 0
    print(f"[GUI] Toggle Feature 2: {value}")
    threading.Thread(target=send_message_twice, args=(f"<feature2={value}>",), daemon=True).start()

def increment_variable():
    print("[GUI] Send: Increment Variable (+1)")
    threading.Thread(target=send_message_once, args=("<variable=+1>",), daemon=True).start()

def decrement_variable():
    print("[GUI] Send: Decrement Variable (-1)")
    threading.Thread(target=send_message_once, args=("<variable=-1>",), daemon=True).start()
# Haupt-GUI nach Verbindung
def build_main_interface():
    global error_label, sogi_locked_label, feature1_label, feature2_label, variable_label

    frame = tk.Frame(root, bg="white")
    frame.place(relx=0.5, rely=0.5, anchor="center")

    # Überschrift
    tk.Label(frame, text="Modulator Control", font=("Arial", 14, "bold"), bg="white").pack(pady=10)

    # Buttons
    btn_frame = tk.Frame(frame, bg="white")
    btn_frame.pack(pady=10)

    tk.Button(btn_frame, text="Enable Modulator", command=send_enable, width=20, bg="#4CAF50", fg="white").pack(pady=5)
    tk.Button(btn_frame, text="Disable Modulator", command=send_disable, width=20, bg="#f44336", fg="white").pack(pady=5)
    tk.Button(btn_frame, text="Ackn_Error", command=send_ack_error, width=20, bg="#FF9800", fg="white").pack(pady=5)
    tk.Button(btn_frame, text="Toggle Feature 1", command=toggle_feature1, width=20, bg="#673AB7", fg="white").pack(pady=5)
    tk.Button(btn_frame, text="Toggle Feature 2", command=toggle_feature2, width=20, bg="#009688", fg="white").pack(pady=5)

    # Fehlercode-Anzeige
    tk.Label(frame, text="Current Error:", bg="white", font=("Arial", 10)).pack(pady=(20, 5))
    error_label = tk.Label(frame, text="---", relief="sunken", width=25, font=("Consolas", 14), bg="white")
    error_label.pack()

    # SOGI_LOCKED Anzeige
    tk.Label(frame, text="SOGI_LOCKED:", bg="white", font=("Arial", 10)).pack(pady=(20, 5))
    sogi_locked_label = tk.Label(frame, text="-", relief="sunken", width=5, font=("Consolas", 14), bg="white")
    sogi_locked_label.pack()

    # Feature1 Anzeige
    tk.Label(frame, text="Feature 1:", bg="white", font=("Arial", 10)).pack(pady=(20, 5))
    feature1_label = tk.Label(frame, text="Low", relief="sunken", width=10, font=("Consolas", 12), bg="white")
    feature1_label.pack()

    # Feature2 Anzeige
    tk.Label(frame, text="Feature 2:", bg="white", font=("Arial", 10)).pack(pady=(10, 5))
    feature2_label = tk.Label(frame, text="Low", relief="sunken", width=10, font=("Consolas", 12), bg="white")
    feature2_label.pack()

    # Variable Anzeige
    tk.Label(frame, text="Feature: Variable", bg="white", font=("Arial", 10)).pack(pady=(20, 5))
    variable_label = tk.Label(frame, text="0", relief="sunken", width=10, font=("Consolas", 12), bg="white")
    variable_label.pack()

    # Variable Control Buttons
    var_btn_frame = tk.Frame(frame, bg="white")
    var_btn_frame.pack(pady=10)
    tk.Button(var_btn_frame, text="+1", command=increment_variable, width=10, bg="#2196F3", fg="white").pack(side="left", padx=5)
    tk.Button(var_btn_frame, text="-1", command=decrement_variable, width=10, bg="#2196F3", fg="white").pack(side="left", padx=5)

# Verbindung herstellen
def connect():
    global ser
    port = port_var.get()
    if not port:
        messagebox.showwarning("No Port", "Please select a serial port.")
        return
    try:
        ser = serial.Serial(port, 115200, timeout=1)
        # Alte Widgets entfernen
        for w in root.winfo_children():
            w.destroy()
        build_main_interface()
        # Lese-Thread starten
        threading.Thread(target=read_serial, daemon=True).start()
        print(f"[INFO] Connected with {port} at 115200 baud.")
    except serial.SerialException as e:
        messagebox.showerror("Connection Error", str(e))
        print(f"[ERROR] Connection failed: {e}")

# === Hauptfenster ===
root = tk.Tk()
root.title("UART modulator control")
root.geometry("600x700")
root.configure(bg="white")

# Verbindungsseite
tk.Label(root, text="Select serial port:", font=("Arial", 12), bg="white").pack(pady=20)
port_var = tk.StringVar()
ports = get_serial_ports()
port_dropdown = ttk.Combobox(root, textvariable=port_var, values=ports, state="readonly", width=30)
port_dropdown.pack()

if ports:
    port_dropdown.current(0)
else:
    tk.Label(root, text="⚠️ No ports found", fg="red", bg="white").pack(pady=5)

tk.Button(root, text="Connect", command=connect, bg="#2196F3", fg="white", font=("Arial", 10), width=20).pack(pady=20)

root.mainloop()