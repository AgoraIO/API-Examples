#!/usr/bin/env python3
"""Compile/execute canonical template code. Missing compilers or inputs fail, never skip.

Native lifecycle tests use controlled dependencies. android-sdk checks real SDK interfaces;
Compile CI additionally stages complete templates into their actual application targets.
"""
import argparse
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[3]
FIXTURES = Path(__file__).parent / "tests/template_fixtures"


def run(*args, cwd):
    subprocess.run([str(arg) for arg in args], cwd=cwd, check=True, timeout=180)


def token_declaration(project):
    source = ROOT / "iOS" / project / project / "Common/NetworkManager/NetworkManager.swift"
    # Extract only the declaration, never the request body, headers or configuration.
    matches = re.findall(r"^\s*(func generateToken\([^\n]+)\s*\{\s*$", source.read_text(), re.M)
    if len(matches) != 1:
        raise ValueError(f"Expected one Token declaration in {source}")
    return matches[0].strip()


def swift_blocks(project):
    path = ROOT / "iOS" / project / ".agents/skills/upsert-case/SKILL.md"
    return re.findall(r"```swift\n(.*?)\n```", path.read_text(), re.S)


def apple(work):
    for project in ("APIExample", "APIExample-Audio", "APIExample-SwiftUI"):
        source = swift_blocks(project)[0]
        name = "<ExampleName>RTC" if project.endswith("SwiftUI") else "<ExampleName>Main"
        source = source[source.index(f"class {name}:"):].replace("<ExampleName>", "Template")
        (work / "Template.swift").write_text("import Foundation\n" + source)
        deps = (FIXTURES / "swift_dependencies.swift").read_text()
        (work / "Dependencies.swift").write_text(deps.replace("__TOKEN_DECLARATION__", token_declaration(project)))
        alias = "TemplateRTC" if project.endswith("SwiftUI") else "TemplateMain"
        setup = 'sut.setupRTC(configs: ["channelName": "test"])' if project.endswith("SwiftUI") else "sut.setupRTC()"
        scenarios = (FIXTURES / "swift_scenarios.swift").read_text()
        (work / "main.swift").write_text(f"typealias TestSubject = {alias}\nfunc setup(_ sut: TestSubject) {{ {setup} }}\n" + scenarios)
        run("xcrun", "swiftc", "-swift-version", "5", "-module-cache-path", work / "cache",
            work / "Dependencies.swift", work / "Template.swift", work / "main.swift", "-o", work / "swift-test", cwd=work)
        print(project, flush=True)
        run(work / "swift-test", cwd=work)

    # Let Swift generate the actual Objective-C selector from the project's declaration.
    bridge = '''import Foundation
@objc public class NetworkManager: NSObject {
    @objc public static let shared = NetworkManager()
    var requests: [(String, UInt, (String?) -> Void)] = []
    @objc public var requestCount: Int { requests.count }
    @objc public __TOKEN_DECLARATION__ { requests.append((channelName, uid, success)) }
    @objc public func respond(_ index: Int, token: String?) { requests[index].2(token) }
}
'''.replace("__TOKEN_DECLARATION__", token_declaration("APIExample-OC"))
    (work / "Network.swift").write_text(bridge)
    run("xcrun", "swiftc", "-swift-version", "5", "-module-cache-path", work / "cache",
        "-emit-library", "-emit-objc-header", "-emit-objc-header-path", work / "APIExample_OC-swift.h",
        "-module-name", "TemplateNetwork", work / "Network.swift", "-o", work / "libTemplateNetwork.dylib", cwd=work)
    skill = (ROOT / "iOS/APIExample-OC/.agents/skills/upsert-case/SKILL.md").read_text()
    implementation = re.findall(r"```objc\n(.*?)\n```", skill, re.S)[1]
    imports = re.findall(r'^#import "([^"\n]+)"', implementation, re.M)
    # Compile the import in the actual template; a nonexistent NetworkManager.h must fail.
    bridge_imports = ''.join(f'#import "{name}"\n' for name in imports if name not in ("<ExampleName>.h", "KeyCenter.h"))
    source = implementation[implementation.index("@interface <ExampleName>Main ()"):].replace("<ExampleName>", "Template")
    (work / "main.m").write_text((FIXTURES / "objc_dependencies.h").read_text() + bridge_imports + source + "\n" + (FIXTURES / "objc_scenarios.m").read_text())
    run("xcrun", "clang", "-fobjc-arc", "-fblocks", "-Werror=objc-method-access", "-framework", "Foundation",
        "-L", work, "-lTemplateNetwork", f"-Wl,-rpath,{work}", work / "main.m", "-o", work / "objc-test", cwd=work)
    print("APIExample-OC", flush=True)
    run(work / "objc-test", cwd=work)


def kotlin(work):
    template = (ROOT / "Android/APIExample-Compose/.agents/skills/upsert-case/references/composable-template.kt").read_text()
    source = template[template.index("private class ExampleCaseSession("):template.index("\n@Preview")]
    (work / "Main.kt").write_text((FIXTURES / "kotlin_dependencies.kt").read_text() + "\n" + source + "\n" + (FIXTURES / "kotlin_scenarios.kt").read_text())
    # A standard Kotlin CLI, including a caller-provided wrapper, may be selected via KOTLINC.
    run(os.environ.get("KOTLINC", "kotlinc"), work / "Main.kt", "-include-runtime", "-d", work / "test.jar", cwd=work)
    run("java", "-jar", work / "test.jar", cwd=work)


def android_sdk(work, sdk_jar, android_jar):
    for path in (sdk_jar, android_jar):
        if not path.is_file():
            raise ValueError(f"Missing SDK input: {path}")
    template = ROOT / "Android/APIExample-Audio/.agents/skills/upsert-case/references/fragment-template.java"
    (work / "AudioExampleCaseTemplate.java").write_text(template.read_text())
    # Only project scaffolding is doubled; Android lifecycle and RTC signatures are real.
    scaffolding = {
        "androidx/annotation/Nullable.java": "package androidx.annotation; public @interface Nullable {}",
        "io/agora/api/example/common/BaseFragment.java": "package io.agora.api.example.common; public class BaseFragment extends android.app.Fragment { protected String getAgoraAppId() { return \"\"; } }",
        "io/agora/api/example/common/widget/AudioSeatManager.java": "package io.agora.api.example.common.widget; public class AudioSeatManager {}",
        "io/agora/api/example/MainApplication.java": """package io.agora.api.example;
public class MainApplication extends android.app.Application {
 public Settings getGlobalSettings() { return null; }
 public static class Settings {
  public int getAreaCode() { return 0; }
  public io.agora.rtc2.proxy.LocalAccessPointConfiguration getPrivateCloudConfig() { return null; }
 }
}""",
    }
    for relative, source in scaffolding.items():
        target = work / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(source)
    compose = (ROOT / "Android/APIExample-Compose/.agents/skills/upsert-case/references/composable-template.kt").read_text()
    stats_imports = re.findall(r"^import (io\.agora\.[\w.]*RtcStats)$", compose, re.M)
    if len(stats_imports) != 1:
        raise ValueError("Expected one explicit SDK RtcStats import in Compose template")
    (work / "StatsProbe.java").write_text(f"import {stats_imports[0]}; class StatsProbe {{ RtcStats stats; }}")
    run("javac", "-J-Duser.language=en", "-J-Duser.country=US", "-classpath", os.pathsep.join(map(str, (sdk_jar, android_jar))),
        "-d", work / "classes", *sorted(work.rglob("*.java")), cwd=work)
    print("PASS: Audio lifecycle override and Compose RtcStats resolve against the supplied SDK", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("check", choices=("apple", "kotlin", "android-sdk"))
    parser.add_argument("--sdk-jar", type=Path)
    parser.add_argument("--android-jar", type=Path)
    args = parser.parse_args()
    if args.check == "android-sdk" and (args.sdk_jar is None or args.android_jar is None):
        parser.error("android-sdk requires --sdk-jar and --android-jar")
    with tempfile.TemporaryDirectory(prefix="ai-template-check-") as directory:
        work = Path(directory)
        if args.check == "apple":
            apple(work)
        elif args.check == "kotlin":
            kotlin(work)
        else:
            android_sdk(work, args.sdk_jar.resolve(), args.android_jar.resolve())


if __name__ == "__main__":
    main()
