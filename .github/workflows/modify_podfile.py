import os, sys

def modfiy(path):
    with open(path, 'r', encoding='utf-8') as file:
        contents = []
        for num, line in enumerate(file):
            if "pod 'Agora" in line:
                line = '\t'+"pod 'sdk', :path => 'sdk.podspec'" + "\n"
            elif "pod 'sdk" in line:
                line = ""
            elif "pod 'Floaty" in line:
                line = '\t'+"pod 'Floaty', :git => 'https://gitee.com/shengwang-dependencies/Floaty.git'" + "\n"
            elif "pod 'AGEVideoLayout" in line:
                line = '\t'+"pod 'AGEVideoLayout', :git => 'https://gitee.com/shengwang-dependencies/AGEVideoLayout.git'" + "\n"
            elif "pod 'CocoaAsyncSocket" in line:
                line = '\t'+"pod 'CocoaAsyncSocket', :git => 'https://gitee.com/shengwang-dependencies/CocoaAsyncSocket.git'" + "\n"
            elif "pod 'SwiftLint" in line:
                line = '\t'+"pod 'SwiftLint', :git => 'https://gitee.com/shengwang-dependencies/SwiftLint', :commit => '1067113303c134ef472a71b30d21e5350de7889d'" + "\n"
            elif "pod 'ijkplayer" in line:
                line = '\t'+"pod 'ijkplayer', :path => 'ijkplayer.podspec'" + "\n"
            elif 'sh .download_script' in line:
                line = line.replace('#', '').replace('false', 'true')
            contents.append(line)
        file.close()
        
        with open(path, 'w', encoding='utf-8') as fw:
            for content in contents:
                fw.write(content)
            fw.close()


if __name__ == '__main__':
    path = sys.argv[1:][0]
    modfiy(path.strip())