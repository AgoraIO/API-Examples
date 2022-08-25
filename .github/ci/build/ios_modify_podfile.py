import os, sys

def modify_podfile(path: str):
	with open(path, 'r', encoding='utf-8') as f:
		content = f.read()
		content = content.replace("pod 'sdk', :path => 'sdk.podspec'", "pod 'sdk', :path => '../../sdk.podspec'")
		content = content.replace("pod 'Agora", "#pod 'Agora")
			
		f.close()
		with open(path, 'w', encoding='utf-8') as fw:
			fw.write(content)
			fw.close()
	

if __name__ == '__main__':
	path = sys.argv[1:][0]
	modify_podfile(path.strip())