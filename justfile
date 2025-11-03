remove IMAGE:
    echo "Eliminando fondo..."
    rembg i {{IMAGE}} output.png
    echo "Borrando original"
    rip {{IMAGE}}
    echo "Escalando imagen..."
    ffmpeg -i output.png -vf scale=500:500 {{IMAGE}}
    echo "Limpiando..."
    rip output.png

