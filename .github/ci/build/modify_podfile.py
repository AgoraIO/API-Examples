import os, sys

def modfiy(path):
    with open(path, 'r', encoding='utf-8') as file:
        contents = []
        for num, line in enumerate(file):
            if "pod 'Agora" in line:
                line = '\t'+"pod 'sdk', :path => '../../sdk.podspec'" + "\n"
            elif "pod 'sdk" in line:
                line = "" + "\n"
            elif 'sh .download_script' in line:
                line = line.replace('true', 'false') + "\n"
            contents.append(line)
        file.close()
        
        with open(path, 'w', encoding='utf-8') as fw:
            for content in contents:
                fw.write(content)
            fw.close()


if __name__ == '__main__':
    path = sys.argv[1:][0]
    modfiy(path.strip())