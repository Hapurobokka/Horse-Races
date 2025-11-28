# Requisitos
- Un compilador de C++ 23.
- CMake

# Compilando
Primero hay que obtener las librerías y crear los archivos de compilación. Primero hay que crear una carpeta en el proyecto llamada `build`. Dentro de la carpeta en la terminal se debe ejecutar:

```bash
cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

Cuando termine todo debe estar descargado. Regresa a la raíz del proyecto y ejecuta:

```bash
cmake --build build
```

Debería dejar un ejecutable en `build/Debug`.

Copia el archivo `utils/table.json` a `assets/tables/table.json` para obtener un mapa (el programa no correrá sin este archivo).

Copia la carpeta `assets` ahí e intenta ejecutar el juego.
