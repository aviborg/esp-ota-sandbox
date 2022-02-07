import os

version = os.environ.get("CLOUD_VERSION")
if version == None:
    version = "v0.0.0"

url = os.environ.get("CLOUD_DOWNLOAD_URL")
if url == None:
    url = "https://github.com/aviborg/workflow-sandbox/releases/latest"
    #url = "https://github.com/aviborg/workflow-sandbox/releases/tag/v0.0.14"

print('-DCLOUD_VERSION=\\"{}\\" -DCLOUD_DOWNLOAD_URL=\\"{}\\"'.format(version, url))