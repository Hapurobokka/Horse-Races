{ pkgs ? import <nixpkgs> {} }:

with pkgs;

mkShell {
  buildInputs = [
    python3
    raylib
    python313Packages.raylib-python-cffi
    black
  ];
}
