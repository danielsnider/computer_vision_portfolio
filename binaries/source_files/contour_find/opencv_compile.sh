#!/bin/sh

g++ -g $1 `pkg-config --cflags opencv` `pkg-config --libs opencv`
