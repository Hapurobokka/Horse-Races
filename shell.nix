{ pkgs ? import <nixpkgs> {} }:
with pkgs;
mkShell {
  inputsFrom = [
    raygui
    raylib
  ];
  buildInputs = [
    clang-tools 
    clang
    cmake
    ffmpeg
    gnumake
    kdePackages.wayland
    kdePackages.wayland-protocols
    libGL
    libffi
    libx11
    ninja
    libxkbcommon
    mesa
    pkg-config
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
    export CPATH="${stdenv.cc.cc}/include/c++/${stdenv.cc.cc.version}:${stdenv.cc.cc}/include/c++/${stdenv.cc.cc.version}/x86_64-unknown-linux-gnu:${glibc.dev}/include"
    export CPLUS_INCLUDE_PATH="$CPATH"
    echo "¡Entorno de desarrollo listo, nya~!"
  '';
}
