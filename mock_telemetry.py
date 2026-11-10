"""
Terraquipu IoT - Mock Telemetry Simulator
Generates calibrated soil telemetry data and streams it to the Terraquipu API or stdout.
Useful for live hackathon demos when hardware connection is unreliable.
"""
import time
import json
import random
import argparse
from datetime import datetime, timezone
import requests

def generate_payload(device_id: str = "terraquipu-node-cusco-01", base_vwc: float = 18.5) -> dict:
    vwc_jitter = round(base_vwc + random.uniform(-0.8, 0.8), 2)
    soil_temp = round(12.5 + random.uniform(-0.5, 0.5), 2)
    air_temp = round(16.5 + random.uniform(-1.0, 1.2), 2)
    air_hum = round(42.0 + random.uniform(-2.0, 2.0), 1)
    
    return {
        "device_id": device_id,
        "firmware_version": "v1.2.0-esp32",
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "location": {
            "latitude": -13.5321,
            "longitude": -71.9675,
            "elevation_masl": 3410.5
        },
        "readings": {
            "soil_moisture_vwc_percent": vwc_jitter,
            "soil_temperature_celsius": soil_temp,
            "air_temperature_celsius": air_temp,
            "air_humidity_percent": air_hum,
            "battery_level_percent": 92
        },
        "calibration_factors": {
            "smap_bias_offset": -3.2,
            "soil_texture_profile": "franco-arcilloso",
            "status": "calibrated"
        }
    }

def main():
    parser = argparse.ArgumentParser(description="Terraquipu IoT Mock Telemetry Streamer")
    parser.add_argument("--endpoint", type=str, default="", help="HTTP POST URL (e.g. http://localhost:8000/api/v1/telemetry)")
    parser.add_argument("--interval", type=int, default=5, help="Seconds between telemetry pulses")
    parser.add_argument("--count", type=int, default=10, help="Number of telemetry cycles to run (0 for infinite)")
    args = parser.parse_args()

    print(f"[*] Iniciando simulador de nodo IoT Terraquipu (intervalo: {args.interval}s)")
    sent = 0
    while True:
        payload = generate_payload()
        if args.endpoint:
            try:
                res = requests.post(args.endpoint, json=payload, timeout=3)
                print(f"[{payload['timestamp']}] Enviado a {args.endpoint} -> Status: {res.status_code}")
            except Exception as e:
                print(f"[!] Error al enviar a {args.endpoint}: {e}")
        else:
            print("\n[TELEMETRÍA SIMULADA - CONTRATO IOT]:")
            print(json.dumps(payload, indent=2))

        sent += 1
        if args.count > 0 and sent >= args.count:
            break
        time.sleep(args.interval)

if __name__ == "__main__":
    main()
