#!/bin/bash

# screen -dmS old-algo-new-lb-small ./exp.sh small
# screen -dmS old-algo-new-lb-medium ./exp.sh medium
# screen -dmS old-algo-new-lb-large ./exp.sh large

screen -dmS old-algo-new-lb bash -c "./exp.sh small ; ./exp.sh medium ; ./exp.sh large"
