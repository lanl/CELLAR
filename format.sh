#!/bin/bash

git ls-files | grep -E "\.(cpp|hpp|h)$" | xargs clang-format -i