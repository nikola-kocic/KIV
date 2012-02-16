#!/bin/bash

export QT_PLUGIN_PATH=.
export LD_LIBRARY_PATH=".:$LD_LIBRARY_PATH"

cd "$(dirname "${0}")"

./kiv $@

