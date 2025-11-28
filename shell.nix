{ pkgs ? import <nixpkgs> {} }:
with pkgs;
mkShell rec {
  nativeBuildInputs = [
    clang-tools 
    clang
    cmake
    ffmpeg
    gnumake
    pkg-config
    valgrind
    ninja
    yt-dlp
  ];
  buildInputs = [
    alsa-lib
    libGL
    libffi
    libx11
    mesa
    xorg.libXcursor
    xorg.libXi
    xorg.libXinerama
    xorg.libXrandr
    xorg.xinput
  ] ++ raylib.buildInputs;
  LD_LIBRARY_PATH = lib.makeLibraryPath buildInputs;
  shellHook = ''
    export CPATH="${stdenv.cc.cc}/include/c++/${stdenv.cc.cc.version}:${stdenv.cc.cc}/include/c++/${stdenv.cc.cc.version}/x86_64-unknown-linux-gnu:${glibc.dev}/include"
    export CPLUS_INCLUDE_PATH="$CPATH"
    echo "¡Entorno de desarrollo listo, nya~!"
  '';
}
