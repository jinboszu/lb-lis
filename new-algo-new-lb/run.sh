#!/bin/bash

# screen -dmS new-algo-new-lb-small ./exp.sh small
# screen -dmS new-algo-new-lb-medium ./exp.sh medium
# screen -dmS new-algo-new-lb-large ./exp.sh large

screen -dmS new-algo-new-lb bash -c "./exp.sh small ; ./exp.sh medium ; ./exp.sh large"
