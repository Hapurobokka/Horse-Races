run: build
    ./game

remove IMAGE:
    echo "Eliminando fondo..."
    rembg i {{IMAGE}} output.png
    echo "Borrando original"
    rip {{IMAGE}}
    echo "Escalando imagen..."
    ffmpeg -i output.png -vf scale=500:500 {{IMAGE}}
    echo "Limpiando..."
    rip output.png

build:
     clang++ main.cpp -o game -lraylib --std=c++23



