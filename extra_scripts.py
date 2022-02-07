Import("env")

# List installed packages
env.Execute("$PYTHONEXE -m pip list")
env.Execute("$PYTHONEXE -m pip install --upgrade pip")

# Install custom packages from the PyPi registry
env.Execute("$PYTHONEXE -m pip install cryptography")

env.Execute("$PYTHONEXE scripts/cert.py -s api.github.com -n github > include/certs.h")
