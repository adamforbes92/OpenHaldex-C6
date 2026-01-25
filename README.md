# OpenHaldex - ESP32 C6 (OEM PCB / beta)

Personal experimental branch for the OpenHaldex-C6 OEM PCB.

## What's different here

### Analyzer mode (inline CAN bridge + SavvyCAN)
Adds a lightweight analyzer mode that turns the controller into a passive CAN bridge and exposes a TCP analyzer interface.

- Analyzer mode disables OpenHaldex control logic (no injected control frames).
- The bridge remains active, so the Haldex ECU still sits on the chassis bus.
- Works with SavvyCAN using GVRET over TCP (port 23).
- Optional Lawicel/SLCAN is supported for tools like CANHacker (limited to a single bus).

#### Enable analyzer mode
1. Open the OpenHaldex web UI.
2. Go to **Setup** and enable **Analyzer Mode**.
   - This also forces **Disable Controller** for clarity.

#### Connect SavvyCAN (GVRET)
1. Keep the OpenHaldex Wi-Fi AP connected.
2. In SavvyCAN: **Add New Device Connection** -> **Network Connection (GVRET)**.
3. IP address: `192.168.1.1` (port 23 is implicit in SavvyCAN's GVRET UI).
4. Set CAN speed to `500000`.

### Notes
- Analyzer mode is intended for inline sniffing; it does not generate any control frames.
- When analyzer mode is disabled, the controller works normally.
