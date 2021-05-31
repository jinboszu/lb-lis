#!/bin/bash

# screen -dmS new-algo-old-lb-small ./exp.sh small
# screen -dmS new-algo-old-lb-medium ./exp.sh medium
# screen -dmS new-algo-old-lb-large ./exp.sh large

screen -dmS new-algo-old-lb bash -c "./exp.sh small ; ./exp.sh medium ; ./exp.sh large"
