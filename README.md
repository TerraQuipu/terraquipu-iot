# Terraquipu IoT 📡🌱

> **Nodo IoT In-Situ, Calibración Edáfica de Suelo y Ground-Truthing en los Andes**  
> *Parte del ecosistema [Terraquipu](https://github.com/TerraQuipu)*

---

## 📌 Propósito del Repositorio

`terraquipu-iot` resuelve la **"Trampa de Resolución Satelital"** en el proyecto Terraquipu. Mientras que los satélites de la NASA como SMAP (9 km) o GPM (10 km) promedian áreas extensas que mezclan valles y punas, este nodo IoT físico:
1. Mide la **humedad volumétrica real del suelo (VWC %)** y la temperatura edáfica en parcelas concretas.
2. Provee el factor de corrección de sesgo (*bias offset*) para ajustar las estimaciones satelitales a nivel de microcuenca.
3. Transmite telemetría en tiempo real hacia [`terraquipu-api`](https://github.com/TerraQuipu/terraquipu-api) mediante WiFi o LoRa.

---

## 👥 Equipo y Roles Asignados

| Integrante | Rol | Responsabilidad en el Sprint de 48h |
| :--- | :--- | :--- |
| **Ing. Electrónico** | **Hardware & IoT Telemetry Lead** | Diseño del circuito con microcontrolador ESP32, acondicionamiento de señal del sensor capacitivo v1.2, calibración en seco/saturado, firmware de transmisión y script de respaldo para la demo en vivo. |

---

## 🔌 Especificaciones de Hardware

* **Microcontrolador:** ESP32 NodeMCU (30 pines) / ESP32-C3 con soporte WiFi 802.11 b/g/n y Bluetooth LE.
* **Sensor de Humedad Edáfica:** Sensor Capacitivo de Humedad de Suelo v1.2 (resistente a corrosión por electrólisis). Conexión en pin analógico `ADC1_CH6` (GPIO 34).
* **Sensor de Temperatura:** Sonda sumergible Dallas DS18B20 (protocolo 1-Wire) en GPIO 4.
* **Alimentación:** Batería LiPo 18650 (3.7V / 2500mAh) con módulo cargador TP4056 y modo Deep Sleep activado cada 15 minutos para maximizar autonomía en campo.

---

## 📡 Contrato de Telemetría (`iot_telemetry.json`)

El nodo transmite un payload HTTP POST cada ciclo de lectura hacia el endpoint `POST /api/v1/telemetry`:

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

## 🛡️ Plan de Contingencia para el Demo (Hackathon Safeguard)

Las redes WiFi saturadas en los recintos de hackathons suelen desconectar el hardware físico durante la presentación del pitch.  
Para evitar bloqueos, este repositorio incluye **`mock_telemetry.py`**, un simulador que inyecta datos dinámicos y realistas a la API con solo presionar un botón.

### Ejecución del Simulador:
```bash
# Simular telemetría periódica hacia la API local
python mock_telemetry.py --endpoint http://localhost:8000/api/v1/telemetry --interval 5
```

---

## 📁 Estructura del Repositorio

* **`firmware/`**: Código fuente en C++/Arduino para ESP32 (`esp32_sensor_node.ino`).
* **`mock_telemetry.py`**: Script de simulación para pruebas de integración con el backend.
* **`schematics/`**: Diagrama de conexiones Fritzing / Wokwi.
