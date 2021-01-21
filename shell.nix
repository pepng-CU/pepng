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
        SDL2
        SDL2_image
        glew
        glm
        glfw
        freetype
        freeglut
    ];
}