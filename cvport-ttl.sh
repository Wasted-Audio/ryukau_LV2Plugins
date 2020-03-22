#!/bin/bash

BIN="./bin"

for JSON in lv2cvport/*/cvport.json; do
  if [ -f "$JSON" ]; then
    python3 lv2cvport/cvport_ttl.py $BIN "$JSON"
  fi
done
