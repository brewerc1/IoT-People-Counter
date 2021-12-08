#!/bin/bash
set -eo pipefail
gradle build #-q packageLibs
#mv build/distributions/IoTLambdaHandler.zip build/IoTLambdaHandler-lib.zip