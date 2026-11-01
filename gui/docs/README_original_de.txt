================================================================================
         UART MODULATOR CONTROL - README
================================================================================

BESCHREIBUNG
------------
Diese Konsole ermöglicht die Steuerung eines Modulators über eine UART-
Verbindung (Serielle Schnittstelle) mit 115200 Baud.


VERWENDUNG
----------
1. Starten Sie die Anwendung (Konsole.exe aus dem dist/ Ordner oder Konsole.py)
2. Wählen Sie den entsprechenden COM-Port aus
3. Klicken Sie auf "Connect"
4. Nutzen Sie die Schaltflächen zur Steuerung:
   - Enable/Disable Modulator
   - Acknowledge Error (Ackn_Error)
   - Toggle Feature 1 / Toggle Feature 2
   - Variable +1 / -1 zur Steuerung der Variablen
5. Die GUI zeigt verschiedene Status-Informationen an:
   - Current Error (Fehlercode als Klartext)
   - SOGI_LOCKED Wert
   - Feature 1 Status (High/Low)
   - Feature 2 Status (High/Low)
   - Variable Wert


ÜBERGABEPARAMETER (UART-Protokoll)
-----------------------------------

GESENDETE BEFEHLE:
------------------
Die Konsole sendet folgende Befehle an das angeschlossene Gerät:

1. Enable Modulator:    <SET_EN_MOD = 1>
   Aktiviert den Modulator

2. Disable Modulator:   <SET_EN_MOD = 0>
   Deaktiviert den Modulator

3. Ackn_Error:          <SET_ACKN_ERR>
   Quittiert aufgetretene Fehler

4. Toggle Feature 1:    <feature1=1> oder <feature1=0>
   Schaltet Feature 1 ein (1) oder aus (0)

5. Toggle Feature 2:    <feature2=1> oder <feature2=0>
   Schaltet Feature 2 ein (1) oder aus (0)

6. Variable Wert:       <variable=X>
   Sendet einen Variablenwert X (kann durch +1/-1 Buttons geändert werden)


EMPFANGENE DATEN:
-----------------
Die Konsole empfängt und zeigt folgende Werte an:

1. Error Code:          <VALUE_ERROR_CODE=X>
   X = Fehlercode (0-22), siehe Fehlercode-Tabelle unten
   Die GUI zeigt die Klartextbezeichnung des Fehlers an

2. SOGI Locked Status:  <SOGI_LOCKED=X>
   X = 0 oder 1 (einziffrige Zahl)

3. Feature 1 Status:    <feature1=X>
   X = 0 (Low) oder 1 (High)

4. Feature 2 Status:    <feature2=X>
   X = 0 (Low) oder 1 (High)

5. Variable Wert:       <variable=X>
   X = Ganzzahlwert der Variable


FEHLERCODE-TABELLE
------------------
 0  = reset state
 1  = idle
 2  = on state
 3  = notEnabledOnState
 4  = OV_UDC_Error
 5  = OC_IDC_Error
 6  = OV_U_Grid_Error
 7  = OC_I_Grid_Error
 8  = OV_U_Conv_Error
 9  = OC_I_Conv_Error
10  = DS_U_Grid_Error
12  = DS_U_Conv_Error
13  = DS_I_Ph1_Error
14  = DS_I_Ph2_Error
15  = DS_I_Ph3_Error
16  = DS_DC_Error
17  = nFLT_LS_Error
18  = nFLT_HS_Error
19  = RDY_HS_Error
20  = RDY_LS_Error
21  = nPG_Supp_HS_Error
22  = nPG_Supp_LS_Error


TECHNISCHE DETAILS
------------------
- Baudrate: 115200
- Datenformat: 8N1 (8 Datenbits, keine Parität, 1 Stoppbit)
- Jeder Befehl wird zweimal mit 50ms Pause gesendet
- Alle Nachrichten werden mit <...> umschlossen
- Zeilenumbruch: \n (LF)


DATEIEN
-------
- Konsole.exe                 : Ausführbare Datei (im dist/ Verzeichnis)
- Konsole.py                  : Python-Quellcode mit UART-Verbindung
- README.txt                  : Diese Datei


SYSTEMANFORDERUNGEN
-------------------
- Windows 10 oder höher
- Für .exe: Keine weiteren Abhängigkeiten (alle Dateien im dist/ Ordner benötigt)
- Für .py: Python 3.x, tkinter, pyserial


================================================================================
Stand: 01.12.2025
================================================================================
