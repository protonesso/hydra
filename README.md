# Hydra

## Introduction
Hydra is a next-generation package manager for **stateless** and stateful Linux-based systems. It aims to be simple and easy. It uses cpio and Brotli for package archives and BLAKE2B for checksums.

## Dependencies
Hydra was written in **BASH**, make sure if your `bash` version is 4.4 or higher. Here's the other dependencies for Hydra:
```
POSIX coreutils (GNU coretils, busybox, toybox, lobase)
libarchive (bsdtar and bsdcpio)
brotli
curl
libselinux (optional, for selinuxenabled)
```
