# Teclado BLE Simplificado con ESP32

Este proyecto utiliza un **microcontrolador ESP32** para emular un teclado Bluetooth, permitiendo enviar pulsaciones de teclas a dispositivos emparejados como PC, smartphones o tabletas.

---

## **Características**
- Simula un teclado Bluetooth usando ESP32.
- Envía pulsaciones de teclas personalizables a través de BLE (Bluetooth Low Energy).
- Ligero y fácil de modificar para combinaciones de teclas específicas.
- Funciona sin problemas con dispositivos que admiten entrada Bluetooth.

---

### **Hardware**
- Placa de desarrollo ESP32
- Cable USB (para programar el ESP32)

### **Software**
- Arduino IDE
- Paquete de tarjetas ESP32 instalado en Arduino IDE
- Librerías necesarias:
    - [ESP32 BLE Keyboard Library](https://github.com/T-vK/ESP32-BLE-Keyboard)

Para instalar la librería:
1. Abre Arduino IDE.
2. Ve a **Sketch > Include Library > Manage Libraries**.
3. Busca "ESP32 BLE Keyboard" e instálala.

---

## **Estructura de Carpetas**
```
BleKeyboard_Simplified/
│
├── README.md           # Documentación del proyecto (este archivo)
├── BleKeyboard.ino     # Archivo principal del código Arduino
└── libraries/          # Librerías externas requeridas (opcional)
```

---

## **Configuración y Uso**
1. **Instala las Dependencias**: Asegúrate de tener instalado el paquete de tarjetas ESP32 y la librería BLE Keyboard en Arduino IDE.
2. **Conecta el ESP32 a tu Computadora**: Utiliza un cable USB para conectar tu ESP32.
3. **Carga el Código**:
    - Abre el archivo `BleKeyboard.ino` en Arduino IDE.
    - Selecciona tu tarjeta ESP32 en **Tools > Board**.
    - Elige el puerto correcto.
    - Haz clic en **Upload** para cargar el código.
4. **Empareja el ESP32 con un Dispositivo**:
    - El ESP32 transmitirá como un teclado Bluetooth.
    - Empareja el ESP32 con una computadora, teléfono o tableta.
5. **Envía Pulsaciones de Teclas**:
    - Una vez emparejado, el ESP32 enviará las pulsaciones predefinidas.
    - Modifica el código para personalizar las secuencias de teclas.

---

## **Personalización**
Para enviar pulsaciones de teclas personalizadas, edita el archivo `BleKeyboard.ino`:
```cpp
bleKeyboard.print("Hello, world!"); // Envía 'Hello, world!' al dispositivo emparejado
bleKeyboard.write(KEY_RETURN);      // Envía la tecla 'Enter'
```
Consulta la documentación de la librería para obtener una lista completa de las teclas compatibles.

---

## **Contribuciones**
Siéntete libre de enviar solicitudes de extracción (pull requests) para mejoras o características adicionales.

---

## **Licencia**
Este proyecto es de código abierto y está licenciado bajo la licencia MIT.

---

## **Créditos**
- Construido utilizando la [ESP32 BLE Keyboard Library](https://github.com/T-vK/ESP32-BLE-Keyboard).
- Soporte de ESP32 proporcionado por Espressif.

---
