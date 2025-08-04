import os, sys

def modfiy(path, isReset):
    print(f'[modify_ios_keycenter] path === {path}, isReset === {isReset}')
    appId = os.environ.get('APP_ID')
    print(f'[modify_ios_keycenter] appId === {appId}')
    faceCaptureLicense = os.environ.get('FACE_CAPTURE_LICENSE')
    print(f'[modify_ios_keycenter] faceCaptureLicense === {faceCaptureLicense}')
    with open(path, 'r', encoding='utf-8') as file:
        contents = []
        for num, line in enumerate(file):
            line = line.strip()
            if "static let AppId" in line:
                print(f'[modify_ios_keycenter] Find `static let AppId` in line {num + 1}')
                if isReset:
                    line = "static let AppId: String = <#YOUR APPID#>"
                else:
                    line = f'static let AppId: String = "{appId}"'
                print(f'[modify_ios_keycenter] line {num + 1} changed to: {line}')
            elif "static let Certificate" in line:
                print(f'[modify_ios_keycenter] Find `static let Certificate` in line {num + 1}')
                if isReset:
                    line = "static let Certificate: String? = <#YOUR Certificate#>"
                else:
                    line = 'static let Certificate: String? = nil'
                print(f'[modify_ios_keycenter] line {num + 1} changed to: {line}')
            elif "static let FaceCaptureLicense" in line:
                print(f'[modify_ios_keycenter] Find `static let FaceCaptureLicense` in line {num + 1}')
                if isReset:
                    line = "static let FaceCaptureLicense: String? = nil"
                else:
                    line = f'static let FaceCaptureLicense: String? = "{faceCaptureLicense}"'
                print(f'[modify_ios_keycenter] line {num + 1} changed to: {line}')
            elif "static NSString * const APPID" in line:
                print(f'[modify_ios_keycenter] Find `static NSString * const APPID` in line {num + 1}')
                if isReset:
                    line = "static NSString * const APPID = <#YOUR APPID#>"
                else:
                    line = f'static NSString * const APPID = @"{appId}";'
                print(f'[modify_ios_keycenter] line {num + 1} changed to: {line}')
            elif "static NSString * const Certificate" in line:
                print(f'[modify_ios_keycenter] Find `static NSString * const Certificate` in line {num + 1}')
                if isReset:
                    line = "static NSString * const Certificate = <#YOUR Certificate#>"
                else:
                    line = 'static NSString * const Certificate = nil;'
                print(f'[modify_ios_keycenter] line {num + 1} changed to: {line}')
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
