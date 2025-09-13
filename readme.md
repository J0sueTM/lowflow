# LowFlow

LowFlow is a functional, pure, data-oriented programming language designed for deterministic, side-effect-controlled computation. It emphasizes bottom-up evaluation, immutable intermediate values, and runtime-managed arenas for performance.

## Key Features

- **Pure functions**: Computation threads are free of side effects; all state changes happen only at the beginning or end.
- **First-class functions and values**: Everything can be passed around as data, including function definitions.
- **Traits & interfaces**: Support for protocol-like traits for type-safe abstraction.
- **Arena memory management**: Efficient ephemeral value allocation; no garbage collection required for intermediate computation.
- **Bottom-up evaluation**: Runtime builds a computation tree and evaluates it efficiently, with tail-call optimization.
- **Entry points & capsules**: Explicit starting points for execution; runtime commands are returned and executed in a controlled environment.

Read the [code walkthrough here](./docs/walkthrough.lf)!

## Goals

- Provide Python/Lua-level expressiveness with performance closer to C.
- Make side effects explicit and controllable.
- Enable developers to build higher-level libraries (DB clients, networking) without exposing unsafe operations.

## Developing

Use [premake5](https://premake.github.io/) to build a project file. For example:

```bash
premake5 gmake && make
```
