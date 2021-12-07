#!/bin/bash
set -eo pipefail
gradle -q packageLibs
mv build/distributions/IoTLambdaHandler.zip build/IoTLambdaHandler-lib.zip