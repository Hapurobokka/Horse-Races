# Requisitos
- Un compilador de C++ 23.
- raylib instalado.

# Compilar
El comando para compilar el programa se encuentra en `justfile`.

```bash
clang++ src/main.cpp src/horse.cpp -o target/game -lraylib --std=c++23

# Tambien funciona con g++
g++ src/main.cpp src/horse.cpp -o target/game -lraylib --std=c++23
```

Probablemente sea necesario indicar al compilador donde se encuentra tu instalación de raylib.
