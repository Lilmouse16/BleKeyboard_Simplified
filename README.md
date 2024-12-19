# Teclado BLE Simplificado con ESP32

Este proyecto utiliza un **microcontrolador ESP32** para emular un teclado Bluetooth, permitiendo enviar pulsaciones de teclas a dispositivos emparejados como PC, smartphones o tabletas.

---

## **Características**
- Simula un teclado Bluetooth usando ESP32.
- Envía pulsaciones de teclas personalizables a través de BLE (Bluetooth Low Energy).
- Navega entre secciones de una página web utilizando la tecla **Tab**.
- Escribe un número específico de textos dependiendo del número de apariciones de **clip X** en un archivo de texto.
- Simula características humanas en la navegación y escritura.
- Ligero y fácil de modificar para combinaciones de teclas específicas.
- Funciona sin problemas con dispositivos que admiten entrada Bluetooth.

---

## **Resumen del Proyecto**

### **Configuración Inicial:**
- Se conecta a un dispositivo (computadora/teléfono) vía Bluetooth como un teclado.
- Carga un archivo de texto (`text.txt`) desde el almacenamiento interno (SPIFFS).
- Calcula el tiempo estimado de finalización en función del contenido del texto.
- Muestra progreso en los LEDs integrados:
  - **Azul**: Conexión Bluetooth establecida.
  - **Rojo**: Escritura activa.

### **Proceso de Navegación:**
- Utiliza la tecla **TAB** para navegar entre secciones (clips) de una página web:
  - El primer clip requiere **16 pulsaciones de Tab**.
  - Los clips siguientes requieren **5 pulsaciones de Tab** cada uno.
- La navegación incluye características humanas como:
  - Velocidades variables de pulsación.
  - Errores ocasionales con correcciones simuladas.
  - Pausas naturales entre grupos de pulsaciones.

### **Proceso de Escritura:**
- Escribe texto de manera similar a como lo haría un humano:
  - Velocidad variable de escritura (base de **56 palabras por minuto**).
  - Simulación de errores tipográficos con correcciones.
  - Pausas naturales entre palabras y oraciones.
  - Simulación de fatiga a lo largo del tiempo.
  - Pausas aleatorias simulando "pensamiento".
  - Agrupación de palabras en ráfagas con descansos entre ellas.

### **Controles y Seguimiento:**
- **Botón BOOT**:
  - Inicia el proceso de escritura.
  - Pausa y reanuda la escritura.
- **Botón RST**:
  - Reinicia el proceso completamente.
- **Monitor Serial**:
  - Muestra el estado de la conexión.
  - Indica porcentaje de progreso.
  - Tiempo transcurrido.
  - Mensajes de estado generales.

### **Manejo de Errores:**
- Gestiona desconexiones Bluetooth.
- Mantiene el estado del progreso durante pausas.
- Se recupera de interrupciones sin perder el progreso.
- Muestra un mensaje de finalización al terminar el proceso.

Este proyecto esencialmente **simula a un humano escribiendo texto de forma natural**, haciendo que parezca que alguien realmente está tecleando en lugar de simplemente pegar texto en el dispositivo.

---

## **Requisitos**

### **Hardware**
- Placa de desarrollo ESP32
- Cable USB (para programar el ESP32)

### **Software**
- Plataforma de desarrollo **PlatformIO** en **Visual Studio Code**
- Librerías necesarias:
    - [ESP32 BLE Keyboard Library](https://github.com/T-vK/ESP32-BLE-Keyboard)

Para instalar las librerías:
1. Abre Visual Studio Code con PlatformIO instalado.
2. Accede al archivo `platformio.ini` y agrega las librerías necesarias usando la configuración adecuada.

---

## **Estructura de Carpetas**
```
BleKeyboard_Simplified/
│
├── README.md           # Documentación del proyecto (este archivo)
├── src/                # Carpeta principal del código fuente
│   └── main.cpp        # Archivo principal del código
├── text.txt            # Archivo de texto usado como entrada (clips)
├── platformio.ini      # Configuración del entorno PlatformIO
└── libraries/          # Librerías externas requeridas (opcional)
```

---

## **Configuración y Uso**
1. **Instala las Dependencias**: Asegúrate de tener instalada la extensión PlatformIO en Visual Studio Code.
2. **Configura el Proyecto**:
    - Clona este repositorio.
    - Abre el proyecto en VSCode.
    - Configura tu archivo `platformio.ini` con la placa ESP32 y las librerías necesarias.
3. **Conecta el ESP32 a tu Computadora**: Utiliza un cable USB para conectar tu ESP32.
4. **Carga el Código**:
    - Compila y carga el código usando PlatformIO.
5. **Empareja el ESP32 con un Dispositivo**:
    - El ESP32 transmitirá como un teclado Bluetooth.
    - Empareja el ESP32 con una computadora, teléfono o tableta.
6. **Inicia el Proceso**:
    - Utiliza el botón **BOOT** para comenzar la navegación y escritura.
    - Observa el progreso en el monitor serial.

---

## **Personalización**
Para enviar pulsaciones de teclas personalizadas, edita el archivo `main.cpp`:
```cpp
bleKeyboard.print("Hello, world!"); // Envía 'Hello, world!' al dispositivo emparejado
bleKeyboard.write(KEY_TAB);         // Envía la tecla 'Tab' para navegar entre secciones
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
