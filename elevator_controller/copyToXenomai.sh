#!/bin/bash
ssh root@192.168.251.128 rm -rf code/classes/sysc3303/project/elevator_controller/*
tar -czf - ./* | ssh root@192.168.251.128 tar -xzmf - -C code/classes/sysc3303/project/elevator_controller
