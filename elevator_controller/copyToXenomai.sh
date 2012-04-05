#!/bin/bash
CODE_DIR='/root/project/elevator_controller'
SSH_COM='ssh root@192.168.251.128'

$SSH_COM rm -rf $CODE_DIR/*
tar -czf - ./* | $SSH_COM tar -xzmf - -C $CODE_DIR
