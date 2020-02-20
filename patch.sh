#!/bin/bash

# This patch fixes a issue that Widget::setVisible(false) doesn't hide widget.
# For more info: https://github.com/DISTRHO/DPF/issues/216
cp patch/NanoVG.cpp lib/DPF/dgl/src/NanoVG.cpp
