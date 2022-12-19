#!/usr/bin/python
# -*- coding: UTF-8 -*-
import re
import os

def main():
    appId = ""
    if "AGORA_APP_ID" in os.environ:
        appId = os.environ["AGORA_APP_ID"]
    token = ""

    fileDirectory = ""
    if "File_Directory" in os.environ:
        fileDirectory = os.environ["File_Directory"]

    # KeyCenter.swift
    KeyCenterPath = fileDirectory + "/KeyCenter.swift"
    print("KeyCenterPath: %s" %KeyCenterPath)
    
    try:
        f = open(KeyCenterPath, 'r+')
        content = f.read()
        appString = "\"" + appId + "\""
        tokenString = "\"" + token + "\""
        contentNew = re.sub(r'<#YOUR APPID#>', appString, content)
        contentNew = re.sub(r'<#YOUR AppId#>', appString, content)
        contentNew = re.sub(r'<#YOUR Certificate#>', tokenString, contentNew)
        f.seek(0)
        f.write(contentNew)
        f.truncate()
    except IOError:
        print("Swift File is not accessible.")
    
    # KeyCenter.m
    KeyCenterPath = fileDirectory + "/KeyCenter.m"
    
    try:
        f = open(KeyCenterPath, 'r+')
        content = f.read()
        appString = "@\"" + appId + "\""
        tokenString = "@\"" + token + "\""
        contentNew = re.sub(r'<#Your App Id#>', appString, content)
        contentNew = re.sub(r'<#Temp Access Token#>', tokenString, contentNew)
        f.seek(0)
        f.write(contentNew)
        f.truncate()
    except IOError:
        print("OC File is not accessible.")

if __name__ == "__main__":
    main()
