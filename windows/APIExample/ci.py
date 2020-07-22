# -*- coding: UTF-8 -*-
import re
import os

def main():

    appId = r'"<enter your agora app id>"'
    if "AGORA_APP_ID" in os.environ:
        appId = os.environ["AGORA_APP_ID"]
        appString = "\""+appId+"\""
    else:
        appString = str(appId)
    f = open("./APIExample/stdafx.h", 'r+')
    content = f.read()
    contentNew = re.sub(r'"<enter your agora app id>"', appString, content)
    f.seek(0)
    f.write(contentNew)
    f.truncate()

if __name__ == "__main__":
    main()
