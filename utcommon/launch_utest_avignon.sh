#!/bin/bash
set -Eeuo pipefail
export LD_LIBRARY_PATH="/opt/gcc-14/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

# Obtener el directorio correcto
# En SLURM, usar SLURM_SUBMIT_DIR si está disponible
if [ -n "${SLURM_SUBMIT_DIR:-}" ]; then
    UTEST_SCRIPT="$SLURM_SUBMIT_DIR/utcommon/utest.sh"
else
    SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
    UTEST_SCRIPT="$SCRIPT_DIR/utest.sh"
fi

sed -i 's/\r$//' "$UTEST_SCRIPT"
exec bash "$UTEST_SCRIPT"