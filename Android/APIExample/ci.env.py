import os
from pathlib import Path


def upsert_property(lines, key, value):
    target = f"{key}="
    replaced = False
    new_lines = []
    for line in lines:
        if line.startswith(target):
            new_lines.append(f"{target}{value}\n")
            replaced = True
        else:
            new_lines.append(line)
    if not replaced and value:
        new_lines.append(f"{target}{value}\n")
    return new_lines

def main():
    app_id = os.environ.get("AGORA_APP_ID", "")
    app_cert = os.environ.get("AGORA_APP_CERT", "") or os.environ.get("AGORA_APP_CERTIFICATE", "")
    local_properties = Path("./local.properties")
    lines = local_properties.read_text().splitlines(keepends=True) if local_properties.exists() else []
    lines = upsert_property(lines, "AGORA_APP_ID", app_id)
    lines = upsert_property(lines, "AGORA_APP_CERT", app_cert)
    local_properties.write_text("".join(lines))


if __name__ == "__main__":
    main()
