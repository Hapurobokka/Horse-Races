# Requisitos
- Un compilador de C++ 23 (g++, clang++, mingw).
- CMake

# Compilando
Copia el archivo `utils/table.json` a `assets/tables/table.json` para obtener un mapa (el programa no correrá sin este archivo).

Luego hay que obtener las librerías y crear los archivos de compilación. Hay que crear una carpeta en el proyecto llamada `build`. Dentro de la carpeta en la terminal se debe ejecutar:

```bash
cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

Cuando termine todo debe estar descargado. Regresa a la raíz del proyecto y ejecuta:

```bash
cmake --build build
```

Debería dejar un ejecutable en `build/Debug`.

Copia la carpeta `assets` ahí e intenta ejecutar el juego.

# Características
- Simulación de físicas.
- Edición de mapas.
- Guardado y cargado de mapas.
- Selección de imágenes de caballo.
- Música y sonido.

# Contribuciones
- Alan: Desarrollador principal
- Leo: Desarrollador ayudante y documentación
- Dante y Víctor: Documentación
