import os

# access to global construction environment


#build_tag = env['PIOENV']
if os.environ.get("CLOUD_VERSION") == None:
    os.environ["CLOUD_VERSION"] = '"v0.0.0"'

print(os.environ)
