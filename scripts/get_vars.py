import os

version = os.environ.get("CLOUD_VERSION")
if version == None:
    version = "v0.0.0"

url = os.environ.get("CLOUD_DOWNLOAD_URL")
if url == None:
    url = "https://github.com/aviborg/esp-ota-sandbox/releases/latest"

print('-DCLOUD_VERSION=\\"{}\\" -DCLOUD_DOWNLOAD_URL=\\"{}\\"'.format(version, url))