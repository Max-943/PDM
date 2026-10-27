Import("env")

from pathlib import Path
import hashlib
import shutil
import subprocess
import sys

PROJECT = Path(env.subst("$PROJECT_DIR"))
DBC = PROJECT / "dbc" / "pdm.dbc"
INCLUDE_OUT = PROJECT / "include" / "generated" / "pdm_dbc.h"
SOURCE_OUT = PROJECT / "src" / "generated" / "pdm_dbc.c"
HASH_FILE = PROJECT / ".pio" / "pdm_dbc.sha256"
TEMP = PROJECT / ".pio" / "dbc_generated"

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def generate(source, target, environment):
    if not DBC.exists():
        raise RuntimeError("DBC not found: {}".format(DBC))
    current = digest(DBC)
    old = HASH_FILE.read_text().strip() if HASH_FILE.exists() else ""
    if current == old and INCLUDE_OUT.exists() and SOURCE_OUT.exists():
        print("[DBC] pdm.dbc unchanged")
        return
    print("[DBC] validating and generating C source")
    if TEMP.exists():
        shutil.rmtree(TEMP)
    TEMP.mkdir(parents=True)
    cmd = [sys.executable, "-m", "cantools", "generate_c_source",
           "--database-name", "pdm_dbc", "--no-floating-point-numbers",
           "--output-directory", str(TEMP), str(DBC)]
    try:
        subprocess.run(cmd, check=True)
    except (subprocess.CalledProcessError, ModuleNotFoundError) as exc:
        raise RuntimeError(
            "cantools generation failed. Install once with: "
            "python -m pip install -r requirements.txt") from exc
    INCLUDE_OUT.parent.mkdir(parents=True, exist_ok=True)
    SOURCE_OUT.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(TEMP / "pdm_dbc.h", INCLUDE_OUT)
    shutil.copy2(TEMP / "pdm_dbc.c", SOURCE_OUT)
    HASH_FILE.parent.mkdir(parents=True, exist_ok=True)
    HASH_FILE.write_text(current)
    print("[DBC] generated pdm_dbc.h and pdm_dbc.c")

env.AddPreAction("buildprog", generate)
