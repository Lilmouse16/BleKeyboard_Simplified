# ESP32 Human-Like Typing Automation

Este proyecto utiliza un **microcontrolador ESP32** para emular un teclado Bluetooth, permitiendo enviar pulsaciones de teclas a dispositivos emparejados como PC, smartphones o tabletas.

---

## **Estructura del Proyecto**
```
proyecto/
├── include/
│   ├── constants.h           # Configuraciones y constantes
│   ├── hardware.h           # Gestión de hardware (LEDs, buzzer, botón)
│   ├── keyboard.h           # Gestión de teclado Bluetooth
│   └── human_simulator.h    # Simulación de comportamiento humano
├── src/
│   ├── main.cpp            # Programa principal
│   ├── hardware.cpp        # Implementación de hardware
│   ├── keyboard.cpp        # Implementación de teclado
│   └── human_simulator.cpp # Implementación de simulador
└── data/
    └── text.txt           # Texto para escritura
```

## **Características**
- Simula un teclado Bluetooth usando ESP32
- Envía pulsaciones de teclas personalizables a través de BLE
- LED indicadores de estado y actividad
- Buzzer para notificación de finalización de secciones
- Control mediante botón externo para inicio/pausa/continuación
- Navega entre secciones de página web usando Tab
- Simula características humanas en navegación y escritura
- Auto-pausa después de cada sección

---

## **Indicadores Visuales y Controles**

### **Patrones de LED**
- **Estado Inicial (Conectando)**:
  - LED Azul: ON
  - LED Rojo: OFF

- **Listo para Escribir**:
  - LED Azul: ON
  - LED Rojo: ON

- **Durante la Escritura**:
  - LEDs alternan rápidamente (simula transmisión)
  - Cuando uno está ON, el otro está OFF

- **En Pausa**:
  - LED Azul: OFF
  - LED Rojo: ON

- **Sección Completada**:
  - Ambos LEDs parpadean sincronizadamente
  - Patrón continúa hasta presionar el botón

### **Control por Botón**
- Primera presión: Inicia la escritura
- Durante escritura: Pausa/continúa inmediatamente
- Después de sección completa: Continúa a siguiente sección

### **Buzzer**
- Suena brevemente al completar cada sección
- Indica que el sistema está listo para continuar

---

## **Variables y Constantes**
```cpp
// Hardware
const int BUTTON_PIN = 0;      // Botón BOOT o G22 para botón externo
const int BUZZER_PIN = 19;     // Buzzer 
const int BLUE_LED = 2;        // LED Azul
const int RED_LED = 13;        // LED Rojo

// Navegación
const int FIRST_CLIP_TAB_COUNT = 16;  // Tabs para primer clip
const int NEXT_CLIP_TAB_COUNT = 5;    // Tabs para clips siguientes
const int CLIP_DELAY = 1000;          // Delay entre clips

// Comportamiento Humano
const int BASE_WPM = 65;                // Palabras por minuto base
const float TYPO_CHANCE = 0.15f;        // 15% de error por palabra
const float FATIGUE_FACTOR = 0.05f;     // Factor de fatiga
const float RECOVERY_RATE = 0.05f;      // Tasa de recuperación
```

---

## **Hardware Requerido**
- Placa de desarrollo ESP32
- Buzzer pasivo
- Botón táctil de 4 pines (opcional, puede usar BOOT)
- LEDs (opcional, puede usar LED integrado)
- Resistencia pull-up de 10kΩ (si usa botón externo)
- Cable USB (para programación)

### **Conexiones**
- **Botón** (si es externo):
  - Un pin diagonal a G22
  - Otro pin diagonal a GND
  - Resistencia pull-up de 10kΩ a 3.3V

- **Buzzer**:
  - Pin largo (+) a G19
  - Pin corto (-) a GND

- **LEDs** (si son externos):
  - LED Azul: G2 (con resistencia a GND)
  - LED Rojo: G13 (con resistencia a GND)

---

## **Software Requerido**
- Visual Studio Code con PlatformIO
- Librerías necesarias:
  - ESP32 BLE Keyboard
  - SPIFFS

---

## **Configuración y Uso**

1. **Preparación**:
   - Instala Visual Studio Code y PlatformIO
   - Clona este repositorio
   - Conecta el hardware según el esquema

2. **Carga del Código**:
   - Abre el proyecto en VSCode
   - Compila y carga el código al ESP32

3. **Operación**:
   - Empareja el ESP32 con tu dispositivo
   - Observa los LEDs para confirmar conexión
   - Presiona el botón para iniciar
   - El sistema pausará automátamente después de cada sección
   - Presiona el botón para continuar a la siguiente sección

4. **Monitoreo**:
   - Usa el monitor serial para ver el progreso
   - Observa los LEDs para estado del sistema
   - El buzzer indicará completación de secciones

---

## **Personalización**
- Ajusta las constantes en `constants.h`
- Modifica velocidades de escritura y patrones de LED
- Configura duración del buzzer y pausas
- Personaliza comportamientos de escritura

---

## **Solución de Problemas**
- Verifica conexiones de hardware
- Confirma polaridad correcta del buzzer
- Asegura conexión Bluetooth activa
- Observa indicadores LED para estado
- Revisa mensajes de debug en monitor serial

---

## **Contribuciones**
Se aceptan contribuciones mediante pull requests.

---

## **Licencia**
Este proyecto está bajo la licencia MIT.

---

## **Créditos**
- Construido sobre [ESP32 BLE Keyboard Library](https://github.com/T-vK/ESP32-BLE-Keyboard)
- Soporte de ESP32 por Espressif