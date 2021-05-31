#!/bin/bash

# screen -dmS old-algo-old-lb-small ./exp.sh small
# screen -dmS old-algo-old-lb-medium ./exp.sh medium
# screen -dmS old-algo-old-lb-large ./exp.sh large

screen -dmS old-algo-old-lb bash -c "./exp.sh small ; ./exp.sh medium ; ./exp.sh large"
