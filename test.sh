#!/bin/bash

set -e
set -x

pushd examples/executable
cxpm --build .
popd

pushd examples/archive-object
cxpm --build .
popd

pushd examples/shared-object
cxpm --build .
popd

pushd examples/executable-with-gstreamer-1.0
cxpm --build .
popd