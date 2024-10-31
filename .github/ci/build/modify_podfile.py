import os, sys

def modfiy(path, sdk_flag):
    with open(path, 'r', encoding='utf-8') as file:
        contents = []
        for num, line in enumerate(file):
            if "pod 'Agora" in line:
                if sdk_flag:
                    line = '\t'+"pod 'sdk', :path => '../../sdk.podspec'" + "\n"
            elif "pod 'sdk" in line:
                if sdk_flag:
                     line = ""
            elif 'sh .download_script' in line:
                line = line.replace('true', 'false') + "\n"
            contents.append(line)
        file.close()
        
        with open(path, 'w', encoding='utf-8') as fw:
            for content in contents:
                fw.write(content)
            fw.close()



if __name__ == '__main__':
    path = sys.argv[1]
    sdk_url_is_not_empty = sys.argv[2].lower() == 'true'
    modfiy(path.strip(), sdk_url_is_not_empty)
