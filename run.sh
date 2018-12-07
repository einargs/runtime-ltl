#!/bin/bash

PLUGIN_SO=./build/plugin/out.so
echo $PLUGIN_SO

echo $@

clang++ -fplugin=$PLUGIN_SO -Wall -stdlib=libc++ $@
