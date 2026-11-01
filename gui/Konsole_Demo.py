import tkinter as tk
from tkinter import ttk, messagebox
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

# Hilfsfunktion: Nachricht simulieren (ohne serielle Verbindung)
def send_message_demo(message):
    print(f"[DEMO] Would send: {message}")

# Button-Aktionen
def send_enable():
    print("[GUI] Send: Enable modulator")
    threading.Thread(target=send_message_demo, args=("<SET_EN_MOD = 1>",), daemon=True).start()

def send_disable():
    print("[GUI] Send: Disable modulator")
    threading.Thread(target=send_message_demo, args=("<SET_EN_MOD = 0>",), daemon=True).start()

def send_ack_error():
    print("[GUI] Send: Acknowledge error")
    threading.Thread(target=send_message_demo, args=("<SET_ACKN_ERR>",), daemon=True).start()

def toggle_feature1():
    global feature1_state
    feature1_state = not feature1_state
    value = 1 if feature1_state else 0
    print(f"[GUI] Toggle Feature 1: {value}")
    threading.Thread(target=send_message_demo, args=(f"<feature1={value}>",), daemon=True).start()
    # Lokale Anzeige aktualisieren
    status_text = "High" if value == 1 else "Low"
    feature1_label.config(text=status_text)

def toggle_feature2():
    global feature2_state
    feature2_state = not feature2_state
    value = 1 if feature2_state else 0
    print(f"[GUI] Toggle Feature 2: {value}")
    threading.Thread(target=send_message_demo, args=(f"<feature2={value}>",), daemon=True).start()
    # Lokale Anzeige aktualisieren
    status_text = "High" if value == 1 else "Low"
    feature2_label.config(text=status_text)

def increment_variable():
    global variable_value
    variable_value += 1
    variable_label.config(text=str(variable_value))
    print(f"[GUI] Increment Variable: {variable_value}")
    threading.Thread(target=send_message_demo, args=(f"<variable={variable_value}>",), daemon=True).start()

def decrement_variable():
    global variable_value
    variable_value -= 1
    variable_label.config(text=str(variable_value))
    print(f"[GUI] Decrement Variable: {variable_value}")
    threading.Thread(target=send_message_demo, args=(f"<variable={variable_value}>",), daemon=True).start()

# Haupt-GUI
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

# === Hauptfenster ===
root = tk.Tk()
root.title("UART modulator control")
root.geometry("600x700")
root.configure(bg="white")

# Direkt Hauptinterface starten (ohne Verbindungsauswahl)
build_main_interface()
print("[INFO] Demo mode started - No UART connection required")

root.mainloop()
