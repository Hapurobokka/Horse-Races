run: build
    build/horses

init:
    mkdir build
    cd build
    cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    cmake --build .
    cd ..

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
    cmake --build build

clean:
    rm -rf build
