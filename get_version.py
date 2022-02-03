import os

version = os.environ.get("CLOUD_VERSION")
if version == None:
    version = "v0.0.0"

print('-DCLOUD_VERSION=\\"{}\\"'.format(version))