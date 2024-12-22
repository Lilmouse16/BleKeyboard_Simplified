# Automatización de Escritura Tipo Humano con ESP32 y Control AHT

## Descripción General
Este proyecto implementa un sistema avanzado de automatización de escritura que simula patrones de escritura humana utilizando un microcontrolador ESP32. Simula patrones naturales de escritura con control dinámico de velocidad basado en objetivos de Tiempo Medio de Manejo (AHT) derivados de la duración del video. El sistema incluye características como pausas naturales, errores de escritura con correcciones, simulación de fatiga y seguimiento de progreso en tiempo real.

## Características

### Funcionalidad Principal
- Emulación de teclado Bluetooth
- Simulación de patrones naturales de escritura
- Ajuste dinámico de velocidad basado en objetivos AHT
- Seguimiento de progreso en tiempo real
- Sistema de 7 niveles de dificultad

### Comportamientos Tipo Humano
- Errores de escritura aleatorios con correcciones automáticas
- Variaciones naturales de velocidad de escritura
- Simulación de fatiga
- Pausas para pensar
- Variaciones de doble espacio

### Retroalimentación por Hardware
- Indicadores LED de estado
- Interfaz de control por botón
- Notificaciones por buzzer
- Indicación visual de progreso

## Estructura del Proyecto

```plaintext
esp32-typing-automation/
├── include/
│   ├── aht_calculator.h     # Sistema de cálculo AHT
│   ├── constants.h          # Constantes de configuración
│   ├── hardware.h          # Gestión de interfaz hardware
│   ├── human_simulator.h   # Simulación de comportamiento humano
│   ├── keyboard.h         # Interfaz de teclado Bluetooth
│   ├── time_utils.h       # Utilidades de tiempo
│   └── timing_manager.h   # Sistema de control de tiempo
├── src/
│   ├── aht_calculator.cpp  # Implementación AHT
│   ├── hardware.cpp        # Implementación control hardware
│   ├── human_simulator.cpp # Implementación simulador humano
│   ├── keyboard.cpp        # Implementación teclado
│   ├── main.cpp           # Flujo principal del programa
│   ├── time_utils.cpp     # Implementación utilidades tiempo
│   └── timing_manager.cpp # Implementación gestor de tiempo
├── data/
│   ├── text.txt           # Descripciones de tareas
│   └── task_format.txt    # Guía de formato de tareas
└── platformio.ini         # Configuración del proyecto
```

## Detalles de Componentes

### Sistema Calculador AHT
El calculador AHT (Average Handling Time - Tiempo Medio de Manejo) determina las velocidades objetivo de escritura basadas en la duración del video y configuraciones de dificultad.

```cpp
class AHTCalculator {
    // Calcula la velocidad requerida basada en:
    // - Duración del video
    // - Multiplicador de dificultad
    // - Puntos de datos del gráfico AHT
}
```

### Gestor de Tiempo
Gestiona la velocidad de escritura y los retrasos para cumplir con los objetivos AHT mientras mantiene variaciones naturales.

```cpp
class TimingManager {
    // Controla:
    // - Retrasos entre caracteres
    // - Pausas entre palabras
    // - Retrasos de pensamiento
    // - Seguimiento de progreso
}
```

### Simulador Humano
Implementa comportamientos de escritura tipo humano y gestiona el proceso general de escritura.

```cpp
class HumanSimulator {
    // Maneja:
    // - Simulación de escritura
    // - Generación de errores
    // - Efectos de fatiga
    // - Variaciones naturales
}
```

## Sistema de Dificultad
El sistema cuenta con 7 niveles de dificultad que afectan la duración de la escritura:
- 1.00x: Muy Fácil (Tiempo base)
- 2.00x: Fácil (2x Tiempo base)
- 3.00x: Medio (3x Tiempo base)
- 4.00x: Difícil (4x Tiempo base)
- 5.00x: Muy Difícil (5x Tiempo base)
- 6.00x: Extremo (6x Tiempo base)
- 7.00x: Imposible (7x Tiempo base)

## Especificación de Formato de Tarea

### Estructura Básica
```
Video [video_id]

Clip #[número] <tiempo_inicio> - <tiempo_fin>
[Descripción principal de la escena]

<tiempo_inicio> - <tiempo_fin>
[Descripción de la acción]

<tiempo_inicio> - <tiempo_fin> [CM]
[Descripción del movimiento de cámara]

<tiempo_inicio> - <tiempo_fin> [CT]
[Descripción de la transición]
```

### Formato de Tiempo
- Formato: `<MM:SS.mmm>`
  - MM: minutos (00-99)
  - SS: segundos (00-59)
  - mmm: milisegundos (000-999)

### Reglas Importantes
1. Cada clip debe comenzar con "Clip #"
2. Todos los tiempos deben incluir ceros iniciales
3. La descripción principal sigue al encabezado del clip
4. [CM] denota Movimiento de Cámara
5. [CT] denota Transición de Cámara
6. Duración total = Último tiempo final - Primer tiempo inicial

### Requisitos de Validación
1. El formato de tiempo debe ser exacto
2. Los números de clip deben ser secuenciales
3. No se permiten huecos ni superposiciones de tiempo
4. Las únicas etiquetas válidas son [CM] y [CT]
5. Todos los marcos de tiempo requieren descripciones

## Sistema de Cálculo AHT

El sistema utiliza un gráfico AHT para determinar las velocidades objetivo de escritura:

### Puntos de Datos del Gráfico AHT
```plaintext
Duración Video (seg) | Límite Inferior | Objetivo | Límite Superior
       5            |       30        |   47.5   |       65
      10            |       60        |    95    |      130
      15            |       90        |   142.5  |      195
      20            |      120        |   190    |      260
      25            |      150        |   237.5  |      325
      30            |      180        |   285    |      390
      35            |      210        |   332.5  |      455
      40            |      240        |   380    |      520
      45            |      270        |   427.5  |      585
```

## Configuración de Hardware

### Componentes Necesarios
- ESP32-S3 DevKit (o ESP32-WROOM)
- Indicadores LED (Azul y Rojo)
- Buzzer (pasivo)
- Cable USB (capaz de transmitir datos)
- Protoboard y cables puente

### Conexiones de Pines
```plaintext
Botón: GPIO0 (Botón BOOT incorporado)
Buzzer: GPIO19
LED Azul: GPIO2 (Externo)
LED Rojo: GPIO13 (Externo)
```

## Uso

### Configuración Inicial
1. Clonar repositorio
2. Instalar PlatformIO en VS Code
3. Configurar `platformio.ini`
4. Conectar componentes de hardware
5. Subir `text.txt` a SPIFFS

### Comandos Serie
- `d X.XX` - Establecer multiplicador de dificultad (1.00 a 7.00)
- `s` - Mostrar estado actual
- `r` - Reiniciar clip actual (cuando está pausado)
- `h` - Mostrar guía de dificultad

### Patrones LED
- Ambos APAGADOS: En espera
- Alternando: Escritura activa
- Ambos ENCENDIDOS: Conectado, listo
- Rojo ENCENDIDO: Pausado
- Azul ENCENDIDO: Esperando conexión

### Controles por Botón
- Una pulsación: Iniciar/Pausar/Reanudar
- Pulsar cuando está pausado: Iniciar siguiente sección

## Monitoreo de Rendimiento

### Información de Estado
El sistema proporciona actualizaciones de estado en tiempo real:
- Progreso del clip actual
- Velocidad de escritura y nivel de dificultad
- Cumplimiento de AHT
- Tiempo estimado de finalización

### Salida de Depuración
El monitor serie proporciona información detallada:
- Estado de conexión
- Eventos de escritura
- Correcciones de errores
- Actualizaciones de progreso

## Licencia
Licencia MIT - Ver archivo LICENSE para detalles

## Contribuir
1. Hacer fork del repositorio
2. Crear rama de características
3. Enviar pull request

## Solución de Problemas
- Verificar conexión USB
- Verificar configuración del puerto COM
- Confirmar conexiones LED
- Verificar carga de archivo SPIFFS
- Asegurar que el monitor serie esté en el modo correcto para entrada de comandos