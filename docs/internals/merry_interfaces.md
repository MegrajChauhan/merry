# Overview

Merry uses simple interfaces to interact with various platforms. These interfaces are defined in **merry/interface** where each module abstracts some form of system interaction.

These interfaces are:
- memory: Abstracts memory mapping for the VM making it easier to get and delete memory pages.
- protectors: Provied synchronization tools such as mutex and condition variable for thread safety.
- thread: Allows Merry to easily spawn threads as needed on different platforms.

