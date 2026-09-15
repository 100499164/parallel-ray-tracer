#!/bin/bash
set -Eeuo pipefail
export LD_LIBRARY_PATH="/opt/gcc-14/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

# Obtener el directorio donde se encuentra este script
# En SLURM, usar SLURM_SUBMIT_DIR si está disponible
if [ -n "${SLURM_SUBMIT_DIR:-}" ]; then
    SCRIPT_DIR="$SLURM_SUBMIT_DIR/ftest"
else
    SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi

# Tests válidos
sed -i 's/\r$//' "$SCRIPT_DIR/01_valid/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/01_valid/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/02_valid/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/02_valid/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/03_valid/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/03_valid/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/04_valid/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/04_valid/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/16_valid/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/16_valid/scene.txt"

# Tests inválidos
sed -i 's/\r$//' "$SCRIPT_DIR/05_invalid_unknown_scene_entity/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/05_invalid_unknown_scene_entity/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/05_invalid_unknown_scene_entity/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/06_invalid_insufficient_scene_info/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/06_invalid_insufficient_scene_info/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/06_invalid_insufficient_scene_info/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/07_invalid_too_many_scene_info/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/07_invalid_too_many_scene_info/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/07_invalid_too_many_scene_info/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/08_invalid_incorrect_scene_info/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/08_invalid_incorrect_scene_info/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/08_invalid_incorrect_scene_info/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/09_invalid_material_names_repeated/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/09_invalid_material_names_repeated/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/09_invalid_material_names_repeated/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/10_invalid_reference_undefined_materials/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/10_invalid_reference_undefined_materials/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/10_invalid_reference_undefined_materials/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/11_invalid_unknown_config_key/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/11_invalid_unknown_config_key/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/11_invalid_unknown_config_key/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/12_invalid_config_line_structure/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/12_invalid_config_line_structure/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/12_invalid_config_line_structure/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/13_invalid_unexpected_config_info/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/13_invalid_unexpected_config_info/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/13_invalid_unexpected_config_info/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/14_invalid_insufficient_config_info/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/14_invalid_insufficient_config_info/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/14_invalid_insufficient_config_info/scene.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/15_invalid_incorrect_config_info/config.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/15_invalid_incorrect_config_info/expected_stderr.txt"
sed -i 's/\r$//' "$SCRIPT_DIR/15_invalid_incorrect_config_info/scene.txt"

exec python3 "$SCRIPT_DIR/ftest.py"