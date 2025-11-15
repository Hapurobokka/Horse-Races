{ pkgs ? import <nixpkgs> {} }:

with pkgs;

mkShell {
  buildInputs = [
    ffmpeg
    raygui
    raylib
    # rembg
    valgrind
    clang-tools
    clang
  ];
}
