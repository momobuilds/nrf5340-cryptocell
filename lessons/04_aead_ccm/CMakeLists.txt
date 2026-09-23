cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(wings_crypto_phase4)
target_sources(app PRIVATE src/main.c src/demo_crypto.c)
