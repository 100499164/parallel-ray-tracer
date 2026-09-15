#!/bin/bash

set -Eeuo pipefail

# --- CONFIGURACIÓN DE RUTAS ---
PROYECTO_ROOT="${PWD}/.."
BIN_DIR="${PROYECTO_ROOT}/out/build/default/par/Release"
EJECUTABLE="render-par"

# --- DEFINICIÓN DE ARCHIVOS DE ENTRADA ---
CONFIG_FILE="${PROYECTO_ROOT}/config/config5.txt"
SCENE_FILE="${PROYECTO_ROOT}/config/scene5.txt"
OUTPUT_DIR="${PROYECTO_ROOT}/out"

# Exporta LD_LIBRARY_PATH
export LD_LIBRARY_PATH="/opt/gcc-14/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

echo "⚙️  Iniciando Batch Lento (1-2 hilos) con 3 iteraciones de perf..."

# --- BUCLE DE HILOS (1 y 2) ---
for threads in 1 2
do
    # Nombre de salida dinámico para no sobrescribir
    OUTPUT_FILE="${OUTPUT_DIR}/output5_${threads}threads.ppm"

    echo "================================================="
    echo "▶️  Ejecutando con ${threads} HILOS (x3 iteraciones)"
    echo "Salida: ${OUTPUT_FILE}"

    # --- EJECUCIÓN CON PERF STAT (-r 3) ---
    # -r 3: Ejecuta 3 veces y hace la media
    # -e ...: Contadores de rendimiento
    perf stat -r 3 -e task-clock,cycles,instructions,L1-dcache-load-misses,LLC-load-misses,page-faults \
      "${BIN_DIR}/${EJECUTABLE}" "${CONFIG_FILE}" "${SCENE_FILE}" "${OUTPUT_FILE}" "${threads}" "1"

    # --- VERIFICACIÓN ---
    if [ -f "${OUTPUT_FILE}" ]; then
       echo "✅ Renderizado (${threads} hilos) completado."
    else
       echo "❌ ERROR: No se generó ${OUTPUT_FILE}."
    fi
done

echo "--- Fin del Batch Lento ---"
