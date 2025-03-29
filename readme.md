# LowFlow

Strongly typed language. WIP!!

## Compilation

You need `premake5` installed in order to build to your build system.
Use `premake5 --help` to see the supported ones.

```bash
# For example:

premake5 gmake  # For gnu make
premake5 vs2022 # For Visual Studio 2022
```

Build with your toolkit of choice. GMake for example:

```
make -config=rel lowflow # The target name isn't necessary.
```
