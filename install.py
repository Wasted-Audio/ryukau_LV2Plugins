import os
import subprocess
import sys
from pathlib import Path

def get_xdg_config_home():
    config_home = os.getenv("XDG_CONFIG_HOME", None)
    if config_home is not None:
        return Path(config_home)

    config_home = os.getenv("HOME", None)
    if config_home is not None:
        return Path(config_home) / Path(".config")

    print("$XDG_CONFIG_HOME and $HOME is empty.\nInstallation failed.")
    exit(1)

def copy_dir(src: Path, dest: Path):
    """
    subprocess is used because dirs_exist_ok option for shutil.copytree()
    doesn't available on Python 3.7 or earlier.
    """
    print(f"Copying {src} to {dest}")
    subprocess.run(["cp", "-r", str(src), str(dest)])

config_home = get_xdg_config_home() if len(sys.argv) != 2 else Path(sys.argv[1])

config_home /= Path("UhhyouPlugins/")
config_home.mkdir(parents=True, exist_ok=True)

style_dir = Path("style")
if not style_dir.exists():
    print("style directory doesn't exists.\nInstallation failed.")
    exit(1)

copy_dir(style_dir, config_home)
