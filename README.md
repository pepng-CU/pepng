# COMP371

## Build (NixOS)

```bash
nix-shell --command fish
mkdir build
cd build
cmake .. -DNIXOS=True
make
```

Should make output as `/bin/COMP371_*.cpp`.

## Build (Others)

You'll need to install OpenGL. Then cmake should work...