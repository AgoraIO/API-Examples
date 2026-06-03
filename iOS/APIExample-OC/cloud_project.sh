#!/usr/bin/env sh

PROJECT_PATH=$PWD

if [ "$WORKSPACE" = "" ]; then
	WORKSPACE=$PWD
fi


cd ${PROJECT_PATH} && pod install || exit 1
