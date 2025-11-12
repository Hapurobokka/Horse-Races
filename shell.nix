{ pkgs ? import <nixpkgs> {} }:

with pkgs;

mkShell {
  buildInputs = [
    black
    ffmpeg
    python3
    python313Packages.raylib-python-cffi
    raygui
    raylib
    rembg
    valgrind
  ];
}
