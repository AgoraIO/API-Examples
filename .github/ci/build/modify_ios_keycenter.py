import os, sys

def modfiy(path, isReset):
    appId = os.environ.get('APP_ID')
    faceCaptureLicense = os.environ.get('FACE_CAPTURE_LICENSE')
    with open(path, 'r', encoding='utf-8') as file:
        contents = []
        for num, line in enumerate(file):
            line = line.strip()
            if "static let AppId" in line:
                if isReset:
                    line = "static let AppId: String = <#YOUR APPID#>"
                else:
                    line = f'static let AppId: String = "{appId}"'
            elif "static let Certificate" in line:
                if isReset:
                    line = "static let Certificate: String? = <#YOUR Certificate#>"
                else:
                    line = 'static let Certificate: String? = nil'
            elif "static let FaceCaptureLicense" in line:
                if isReset:
                    line = "static let FaceCaptureLicense: String? = nil"
                else:
                    line = f'static let FaceCaptureLicense: String? = "{faceCaptureLicense}"'
            elif "static NSString * const APPID" in line:
                if isReset:
                    line = "static NSString * const APPID = <#YOUR APPID#>"
                else:
                    line = f'static NSString * const APPID = @"{appId}";'
            elif "static NSString * const Certificate" in line:
                if isReset:
                    line = "static NSString * const Certificate = <#YOUR Certificate#>"
                else:
                    line = 'static NSString * const Certificate = nil;'
            contents.append(line)
        file.close()
        
        with open(path, 'w', encoding='utf-8') as fw:
            for content in contents:
                if "{" in content or "}" in content:
                    fw.write(content + "\n")
                else:
                    fw.write('\t'+content + "\n")
            fw.close()


if __name__ == '__main__':
    print(f'argv === {sys.argv[1:]}')
    path = sys.argv[1:][0]
    isReset = eval(sys.argv[1:][1])
    modfiy(path.strip(), isReset)
