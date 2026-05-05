#!/usr/bin/env bash

git remote add dmt-upstream https://github.com/Dimethoxy/DMT.git

git fetch dmt-upstream

git subtree add --prefix=src/dmt dmt-upstream main --squash