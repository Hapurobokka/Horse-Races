{ pkgs ? import <nixpkgs> {} }:

with pkgs;

mkShell {
  inputsFrom = [
    raygui
    raylib
  ];

  buildInputs = [
    clang
    cmake
    ffmpeg
    gnumake
    kdePackages.wayland
    kdePackages.wayland-protocols
    libGL
    libffi
    libx11
    libxkbcommon
    mesa
    pkg-config
    # rembg
    valgrind
    wayland-scanner
    wayland-utils
    xorg.libXcursor
    xorg.libXi
    xorg.libXinerama
    xorg.libXrandr
    xorg.xinput
  ];
  shellHook = ''
    echo "¡Entorno de desarrollo listo, nya~!"
  '';
}
