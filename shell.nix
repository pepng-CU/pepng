{ pkgs ? import <nixpkgs> { } }:
with pkgs;

stdenv.mkDerivation {
    name = "env";
    nativeBuildInputs = [ 
        cmake 
        xorg.libXrandr
        xorg.libXinerama
        xorg.libXcursor
        xorg.libXi
        xorg.libXext
    ];
    buildInputs = [ 
        zeromq
        mesa
        libcxx
        libGL
        driversi686Linux.mesa
        freetype
    ];
}