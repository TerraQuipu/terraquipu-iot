# Terraquipu IoT 📡🌱

> **Emulador de Telemetría de Sensores Arduino/ESP32 y Ground-Truthing en los Andes**  
> *(Enfoque 100% Software / Digital Twin de Sensores)*  
> *Parte del ecosistema [Terraquipu](https://github.com/TerraQuipu)*

---

## 📌 Propósito del Repositorio

`terraquipu-iot` resuelve la **"Trampa de Resolución Satelital"** en Terraquipu desde una arquitectura **100% de software**. Mientras que los satélites de la NASA como SMAP (9 km) o GPM (10 km) promedian áreas extensas que mezclan valles y punas, este módulo:
1. **Emula y genera telemetría de campo** idéntica a la producida por sensores de suelo conectados a microcontroladores **Arduino** o **ESP32** (humedad volumétrica $VWC\%$ y temperatura edáfica).
2. Provee el factor de corrección de sesgo (*bias offset*) para calibrar matemáticamente las estimaciones satelitales en microcuencas andinas.
3. Transmite telemetría en tiempo real hacia [`terraquipu-api`](https://github.com/TerraQuipu/terraquipu-api) mediante endpoints HTTP REST.
4. Incluye la referencia de firmware (`firmware/`) para conectar hardware físico real cuando esté disponible en campo.

---

## 👥 Equipo y Roles Asignados

| Integrante | Rol | Responsabilidad en el Sprint de 48h |
| :--- | :--- | :--- |
| **Ing. Software IoT / Telemetría** | **Digital Twin & Telemetry Streamer** | Programación del simulador de sensores Arduino en Python (`mock_telemetry.py`), generación de datasets sintéticos de calibración edáfica, pruebas de estrés y pipeline de streaming hacia el backend. |

---

## 💻 Arquitectura 100% Software (Emulador de Sensores)

El emulador reproduce fielmente el comportamiento físico de una estación agroclimática andina con variación diurna, ruido estocástico y correlación de humedad tras precipitaciones:

```
[ Generador / Emulador Arduino ]
        │
        ▼ (Genera lecturas con fluctuación térmica y de VWC)
[ Contrato iot_telemetry.json ]
        │
        ▼ (HTTP POST /api/v1/telemetry)
[ Terraquipu API (FastAPI) ]
        │
        ▼
[ Calibración del Píxel Satelital NASA ]
```

---

## 📡 Contrato de Telemetría (`iot_telemetry.json`)

El emulador transmite un payload HTTP POST cada ciclo de lectura hacia el backend:

```json
{
  "device_id": "terraquipu-node-cusco-01",
  "firmware_version": "v1.2.0-esp32",
  "timestamp": "2026-11-10T10:30:00Z",
  "location": {
    "latitude": -13.5321,
    "longitude": -71.9675,
    "elevation_masl": 3410.5
  },
  "readings": {
    "soil_moisture_vwc_percent": 18.4,
    "soil_temperature_celsius": 12.3,
    "air_temperature_celsius": 16.8,
    "air_humidity_percent": 42.1,
    "battery_level_percent": 88
  },
  "calibration_factors": {
    "smap_bias_offset": -3.2,
    "soil_texture_profile": "franco-arcilloso",
    "status": "calibrated"
  }
}
```

---

## 🚀 Uso del Emulador de Telemetría

No se requiere ningún circuito físico para correr y probar el sistema:

```bash
# 1. Clonar el repositorio
git clone https://github.com/TerraQuipu/terraquipu-iot.git
cd terraquipu-iot

# 2. Transmitir telemetría simulada hacia la API local
python mock_telemetry.py --endpoint http://localhost:8000/api/v1/telemetry --interval 5

# 3. O simplemente visualizar datos de sensores por consola
python mock_telemetry.py --count 3
```

---

## 📁 Estructura del Repositorio

* **`mock_telemetry.py`**: Emulador central de lecturas de sensores Arduino (100% Python).
* **`firmware/`**: Código de referencia para Arduino/ESP32 en caso de integración futura con hardware físico.
