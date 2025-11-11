# Merry Nexus

## Overview

**Merry Nexus**, formerly known as the *Merry Virtual Machine*, is a set of conventions that enables various cores—each representing different architecture types—to run alongside one another and cooperate to solve problems. In simpler terms, Merry Nexus allows different **vCPUs** to work together even if they operate in completely different ways.  

An analogy would be a computer with two CPUs: one from AMD and another based on the ARM architecture—distinct systems that somehow work together. Merry enables a similar concept, except its CPUs are virtual and need not emulate real hardware. Merry’s **flexibility and versatility** make it possible to implement almost anything conceivable within the bounds of current hardware capabilities.

You can think of Merry as a **runtime environment** that supports multiple **virtual machines**, each potentially unique. Alternatively, you can think of it as a **set of conventions** that these VMs follow to collaborate and solve problems. Each interpretation offers a valid perspective.

## Purpose

Merry is designed to be an **educational platform**—a place for those who appreciate abstraction but want to explore low-level concepts without breaking things. With a wide variety of vCPUs, users can experiment freely, combining different architectures and behaviors. Imagination is the only limit.

For example, Merry might include three vCPUs:
- One counts the occurrences of a given character in a buffer.
- Another creates a window and draws a flower.
- A third simulates a stack-based virtual machine.

A program could use the stack-based VM to read a file and, if it detects the character `'f'` a thousand times, instruct the second vCPU to display the flower. This example is intentionally whimsical, but it demonstrates Merry’s potential for orchestrating diverse components.

This flexibility supports both **beginners**, who can learn by experimentation, and **experts**, who can prototype complex ideas. Merry still requires refinement to become a robust tool, but its goal extends beyond education—it’s also a **problem-solving platform**. The main challenge in this regard lies in the **creation of a unified programming language**.

Each vCPU has its own architecture, internal behavior, and memory structure, making a universal language that works across all vCPU types extraordinarily difficult—perhaps even impossible. Since a single Merry program may utilize multiple executing architectures at once, the challenge is significant. Overcoming it would make Merry even more valuable to the real world.

## Project Structure

You can find the entire source code in the **merry/** folder. The documentation for Merry can be found in the **docs/** folder whereas tests and the results can be found in **test/** and **test_results/** respectively.

## Requirements

Merry requires:
1. **GCC** compiler.
2. **Make** build system

_Note: Merry currently only works on Linux as it hasn't been tested for Windows and win support is limited._

## Trying Merry Out

1. Clone Merry
```bash
  git clone https://www.github.com/MegrajChauhan/merry.git
```

2. Change into directory
```bash
 cd merry
```

3. Build Merry
```bash
make all flags=-O3 # You may add other flags as necessary(the build is done in debug mode)
```

4. Run Merry
```bash
./build/mvm --help # To list all of the commands
./build/mvm --f test_results/{any of the .mdat file}
```

## Goals

- [] Sub-systems
- [] Stack-based core
- [] Inter-core communication
- [] More interfaces
- [] More Graves Capabilities
- [tick] Robust Error handling
- [] Windows support
- [] Documentation
- [] 5 core types
- [] Rigorous testing
- [] Lots of customizability

## Contributing

If anyone wishes to contribute to the project, please check out **CONTRIBUTING.md** 
If anyone wishes to contribute their own Core Type, please check out **CORE_CONTRIBUTION.md**

## License

This project is licensed under **GNU GENERAL PUBLIC LICENSE**.
