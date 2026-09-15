#!/bin/bash

# ---
# Script de Pruebas Unitarias (Ubicado en 'utcommon')
#
# 1. Detiene la ejecución si cualquier comando falla (set -e).
# 2. Calcula la raíz del proyecto (subiendo un nivel desde 'utcommon').
# 3. Compila y prueba las configuraciones 'Debug' y 'Release'.
# ---

# (1) Detener el script si un comando falla
set -e

# --- (A) Configuración de Rutas y Opciones ---

# 1. Obtener el directorio donde se encuentra este script
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# 2. Directorio raíz (subiendo un nivel desde SCRIPT_DIR)
PROJECT_ROOT=$( dirname "$SCRIPT_DIR" )

# 3. Construir la ruta al directorio de build
BUILD_DIR="$PROJECT_ROOT/out/build/default"

# Array con todas las configuraciones
CONFIGS_TO_TEST=("Debug" "Release")

echo "===================================================="
echo "INICIANDO PRUEBAS UNITARIAS"
echo "Script ejecutado desde: $SCRIPT_DIR"
echo "Raíz del proyecto: $PROJECT_ROOT"
echo "Directorio de build: $BUILD_DIR"
echo "Configuraciones a probar: ${CONFIGS_TO_TEST[@]}"
echo "====================================================="

# --- (B) Bucle de Compilación y Pruebas ---
for config in "${CONFIGS_TO_TEST[@]}"; do

    echo ""
    echo "-----------------------------------------"
    echo " PASO 1: Compilando ($config)..."
    echo "-----------------------------------------"
    
    cmake --build "$BUILD_DIR" --config "$config"

    echo ""
    echo "-----------------------------------------"
    echo " PASO 2: Ejecutando Pruebas ($config)..."
    echo "-----------------------------------------"
    
    # Moverse al directorio de build para CTest
    cd "$BUILD_DIR"
    
    # Ejecutar CTest para esta configuración
    ctest -C "$config" --output-on-failure
    
    # Regresar a la raíz del proyecto para la siguiente iteración
    cd "$PROJECT_ROOT"

done

echo ""
echo "==================================================="
echo " TODAS LAS CONFIGURACIONES PASARON CON ÉXITO"
echo "==================================================="