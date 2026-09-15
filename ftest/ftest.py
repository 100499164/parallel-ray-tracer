#!/usr/bin/env python3
"""
Script de pruebas funcionales para render-2025
Ejecuta pruebas funcionales para implementaciones PAR
"""

import os
import sys
import math
import subprocess
import difflib
from pathlib import Path
from typing import Tuple, List, Dict

class Colors:
    """Códigos de colores ANSI para la terminal"""
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    BOLD = '\033[1m'
    RESET = '\033[0m'


class TestResult:
    """Representa el resultado de una prueba"""
    def __init__(self, name: str, passed: bool, message: str = ""):
        self.name = name
        self.passed = passed
        self.message = message


DEFAULT_THREADS = 4
DEFAULT_PARTITIONER = 0  # AUTO

class FunctionalTester:
    """Clase principal para ejecutar pruebas funcionales"""
    @staticmethod
    def read_ppm(filepath: Path) -> Tuple[int, int, List[Tuple[int, int, int]]]:
        """
        Lee un archivo PPM y devuelve (ancho, alto, lista_de_pixeles)
        Soporta formatos P3 (ASCII) y P6 (binario)
        """
        with open(filepath, 'rb') as f:
            # Leer el tipo de formato
            magic = f.readline().decode('ascii').strip()
            
            # Saltar comentarios
            line = f.readline().decode('ascii')
            while line.startswith('#'):
                line = f.readline().decode('ascii')
            
            # Leer dimensiones
            width, height = map(int, line.strip().split())
            
            # Leer valor máximo
            max_val = int(f.readline().decode('ascii').strip())
            
            pixels = []
            
            if magic == 'P3':  # ASCII
                # Leer todos los valores restantes
                data = f.read().decode('ascii').split()
                for i in range(0, len(data), 3):
                    r, g, b = int(data[i]), int(data[i+1]), int(data[i+2])
                    pixels.append((r, g, b))
            elif magic == 'P6':  # Binario
                # Leer bytes directamente
                pixel_data = f.read()
                for i in range(0, len(pixel_data), 3):
                    r, g, b = pixel_data[i], pixel_data[i+1], pixel_data[i+2]
                    pixels.append((r, g, b))
            else:
                raise ValueError(f"Formato PPM no soportado: {magic}")
            
            return width, height, pixels
    
    def __init__(self, base_dir: Path, impl_name: str):
        self.base_dir = base_dir
        self.ftest_dir = base_dir / "ftest"
        self.out_dir = base_dir / "out" / "build" / "default"
        self.impl_name = impl_name
        self.executable = self._find_executable(impl_name)
        self.umbral_max_dif_pixel = 150
        self.umbral_rmse = 10
        self.results = {impl_name: []}
    
    def _find_executable(self, impl_name: str) -> Path:
        """Encuentra el ejecutable para la implementación dada"""
        impl_dir = self.out_dir / impl_name
        for build_type in ["Release"]:
            executable = impl_dir / build_type / f"render-{impl_name}"
            if executable.exists() and executable.is_file():
                return executable
        raise FileNotFoundError(f"No se encontró el ejecutable render-{impl_name} en {impl_dir}")
    
    def _get_test_dirs(self) -> Tuple[List[Path], List[Path]]:
        """Obtiene las carpetas de pruebas válidas e inválidas ordenadas"""
        all_dirs = [d for d in self.ftest_dir.iterdir() if d.is_dir()]
        
        valid_tests = sorted([d for d in all_dirs if '_valid' in d.name],
                           key=lambda x: int(x.name.split('_')[0]))
        invalid_tests = sorted([d for d in all_dirs if '_invalid' in d.name],
                             key=lambda x: int(x.name.split('_')[0]))
        
        return valid_tests, invalid_tests
    
    def _run_test_executable(self, executable: Path, config_file: Path, 
                           scene_file: Path, output_ppm: Path, test_dir: Path) -> Tuple[int, str, str]:
        """Ejecuta el ejecutable con los archivos de configuración, escena y salida, usando hilos y particionador por defecto"""
        args = [
            str(executable),
            str(config_file),
            str(scene_file),
            str(output_ppm),
            str(DEFAULT_THREADS),
            str(DEFAULT_PARTITIONER)
        ]
        result = subprocess.run(
            args,
            cwd=str(test_dir),
            capture_output=True,
            text=True
        )
        return result.returncode, result.stdout, result.stderr
    

    def validar_imagenes(self, ruta_img1, ruta_img2):
        """
        Compara dos imágenes usando los umbrales especificados:
        1. Diferencia Máxima de Píxel (MPD)
        2. Error Cuadrático Medio (RMSE, según la descripción)
        
        Devuelve un tuple con:
        (es_aceptable, mpd_calculado, rmse_calculado, condicion_1_ok, condicion_2_ok)
        """
        try:
            # Leer imágenes PPM directamente
            ancho1, alto1, pixels1 = self.read_ppm(ruta_img1)
            ancho2, alto2, pixels2 = self.read_ppm(ruta_img2)
        except FileNotFoundError as e:
            print(f"Error: No se pudo encontrar el archivo: {e.filename}")
            return None
        except Exception as e:
            print(f"Error al abrir las imágenes: {e}")
            return None

        # Estandarización: verificar que las imágenes tengan el mismo tamaño
        # if (ancho1, alto1) != (ancho2, alto2):
        #     print(f"Advertencia: Las imágenes tienen tamaños diferentes. "
        #         f"img1: {ancho1}x{alto1}, img2: {ancho2}x{alto2}")
        #     return None

        total_pixeles = ancho1 * alto1

        # Si la imagen está vacía, se considera aceptable
        if total_pixeles == 0:
            return (True, 0.0, 0.0, True, True)

        max_diferencia_pixel = 0.0
        suma_cuadratica_total = 0.0

        # Recorrer todos los píxeles
        for i in range(total_pixeles):
            r1, g1, b1 = pixels1[i]
            r2, g2, b2 = pixels2[i]

            # --- Cálculo de la diferencia del píxel (según fórmula) ---
            dif_pixel = (abs(r1 - r2) + abs(g1 - g2) + abs(b1 - b2)) / 3.0

            # --- Condición 1: Diferencia Máxima ---
            # Se actualiza el valor máximo encontrado hasta ahora
            if dif_pixel > max_diferencia_pixel:
                max_diferencia_pixel = dif_pixel

            # --- Condición 2: Error Cuadrático Medio ---
            # 1. Se calcula el cuadrado de la diferencia
            dif_cuadrado = dif_pixel * dif_pixel
            # 2. Se suman todos los valores cuadráticos
            suma_cuadratica_total += dif_cuadrado

        # --- Calcular resultados finales ---

        # Valor final para la Condición 1:
        mpd_calculado = max_diferencia_pixel

        # Valor final para la Condición 2:
        # 2. (continuación) Se divide la suma por el número de píxeles (media)
        media_cuadratica = suma_cuadratica_total / total_pixeles
        # 3. Se calcula la raíz cuadrada (esto es técnicamente RMSE)
        rmse_calculado = math.sqrt(media_cuadratica)

        # --- Comprobar umbrales ---
        condicion_1_ok = (mpd_calculado < self.umbral_max_dif_pixel)
        condicion_2_ok = (rmse_calculado < self.umbral_rmse)

        # El resultado es aceptable SÓLO SI ambas condiciones se cumplen
        es_aceptable = condicion_1_ok and condicion_2_ok

        return (es_aceptable, mpd_calculado, rmse_calculado, condicion_1_ok, condicion_2_ok)
    
    def _compare_images(self, expected_ppm: Path, actual_ppm: Path) -> Tuple[bool, str]:
        """Compara dos archivos PPM"""
        es_aceptable, mpd, rmse, c1_ok, c2_ok = self.validar_imagenes(expected_ppm, actual_ppm)
        
        if es_aceptable:
            return True, f"Imágenes coinciden dentro de los umbrales (MPD: {mpd:.2f}, RMSE: {rmse:.2f})"
        else:
            detalles = []
            if not c1_ok:
                detalles.append(f"MPD ({mpd:.2f}) excede el umbral ({self.umbral_max_dif_pixel})")
            if not c2_ok:
                detalles.append(f"RMSE ({rmse:.2f}) excede el umbral ({self.umbral_rmse})")
            return False, "Diferencias encontradas: " + "; ".join(detalles)

    
    def _compare_stderr(self, expected_stderr: Path, actual_stderr: str) -> Tuple[bool, str]:
        """Compara el stderr esperado con el actual"""
        if not expected_stderr.exists():
            return False, f"Archivo esperado no encontrado: {expected_stderr}"
        
        with open(expected_stderr, 'r') as f:
            expected = f.read().strip()
        
        actual = actual_stderr.strip()
        
        if expected == actual:
            return True, "stderr coincide"
        else:
            # Generar diff para mostrar diferencias
            diff = '\n'.join(difflib.unified_diff(
                expected.splitlines(),
                actual.splitlines(),
                fromfile='expected_stderr.txt',
                tofile='actual_stderr',
                lineterm=''
            ))
            return False, f"stderr no coincide:\n{diff}"
    
    def _check_no_image_created(self, test_dir: Path, output_ppm: Path) -> Tuple[bool, str]:
        """Verifica que no se haya creado la imagen de salida"""
        if output_ppm.exists():
            return False, f"Se creó el archivo de salida {output_ppm.name} cuando no debería"
        
        return True, "No se creó la imagen de salida"
    
    def run_valid_test(self, test_dir: Path, executable: Path, impl_name: str) -> TestResult:
        """Ejecuta una prueba válida"""
        test_name = test_dir.name
        config_file = test_dir / "config.txt"
        scene_file = test_dir / "scene.txt"
        expected_ppm = test_dir / "s.ppm"
        output_ppm = test_dir / "test_out.ppm"
        
        print(f"  {Colors.CYAN}Ejecutando: {test_name}{Colors.RESET}")
        
        # Verificar que existen los archivos necesarios
        if not config_file.exists() or not scene_file.exists() or not expected_ppm.exists():
            return TestResult(test_name, False, "Archivos config.txt o scene.txt no encontrados")
        
        # Limpiar posible archivo de salida previo
        if output_ppm.exists():
            output_ppm.unlink()
        
        # Ejecutar el test
        returncode, stdout, stderr = self._run_test_executable(
            executable, config_file, scene_file, output_ppm, test_dir
        )

        # Verificar que el código de retorno sea 0 (éxito)
        if returncode != 0:
            return TestResult(test_name, False, f"El programa devolvió error (código {returncode}):\n{stderr}")

        # Verificar que se creó el archivo test_out.ppm
        if not output_ppm.exists():
            return TestResult(test_name, False, "No se creó el archivo de salida test_out.ppm")

        # Comparar la imagen de salida con la esperada
        images_ok, images_msg = self._compare_images(expected_ppm, output_ppm)
        if not images_ok:
            return TestResult(test_name, False, images_msg)

        # Por ahora solo devolvemos un placeholder
        return TestResult(test_name, True, "Prueba válida - lógica pendiente de implementar")
    
    def run_invalid_test(self, test_dir: Path, executable: Path, impl_name: str) -> TestResult:
        """Ejecuta una prueba inválida"""
        test_name = test_dir.name
        config_file = test_dir / "config.txt"
        scene_file = test_dir / "scene.txt"
        expected_stderr_file = test_dir / "expected_stderr.txt"
        output_ppm = test_dir / "test_out.ppm"
        
        print(f"  {Colors.CYAN}Ejecutando: {test_name}{Colors.RESET}")
        
        # Verificar que existen los archivos necesarios
        if not config_file.exists() or not scene_file.exists():
            return TestResult(test_name, False, "Archivos config.txt o scene.txt no encontrados")
        
        if not expected_stderr_file.exists():
            return TestResult(test_name, False, "Archivo expected_stderr.txt no encontrado")
        
        # Limpiar posible archivo de salida previo
        if output_ppm.exists():
            output_ppm.unlink()
        
        # Ejecutar el test
        returncode, stdout, stderr = self._run_test_executable(
            executable, config_file, scene_file, output_ppm, test_dir
        )
        
        # Verificar que el código de retorno no sea 0 (debe fallar)
        if returncode == 0:
            return TestResult(test_name, False, "El programa no devolvió error cuando debería")
        
        # Verificar que no se creó la imagen de salida
        no_image_ok, no_image_msg = self._check_no_image_created(test_dir, output_ppm)
        if not no_image_ok:
            return TestResult(test_name, False, no_image_msg)
        
        # Comparar stderr
        stderr_ok, stderr_msg = self._compare_stderr(expected_stderr_file, stderr)
        if not stderr_ok:
            return TestResult(test_name, False, stderr_msg)
        
        return TestResult(test_name, True, "stderr coincide y no se creó imagen")
    
    def run_tests_for_implementation(self):
        """Ejecuta todas las pruebas para la implementación actual (AOS o SOA)"""
        impl_name = self.impl_name
        executable = self.executable
        print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*70}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}Ejecutando pruebas para {impl_name.upper()}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}{'='*70}{Colors.RESET}\n")
        valid_tests, invalid_tests = self._get_test_dirs()
        if valid_tests:
            print(f"{Colors.BOLD}Pruebas Válidas:{Colors.RESET}")
            for test_dir in valid_tests:
                result = self.run_valid_test(test_dir, executable, impl_name)
                self.results[impl_name].append(result)
                self._print_test_result(result)
        if invalid_tests:
            print(f"\n{Colors.BOLD}Pruebas Inválidas:{Colors.RESET}")
            for test_dir in invalid_tests:
                result = self.run_invalid_test(test_dir, executable, impl_name)
                self.results[impl_name].append(result)
                self._print_test_result(result)
        self._print_implementation_summary(impl_name)
    
    def _print_test_result(self, result: TestResult):
        """Imprime el resultado de una prueba"""
        if result.passed:
            status = f"{Colors.GREEN}✓ PASS{Colors.RESET}"
        else:
            status = f"{Colors.RED}✗ FAIL{Colors.RESET}"
        
        print(f"    {status} - {result.name}")
        
        if not result.passed and result.message:
            # Indentar el mensaje de error
            for line in result.message.split('\n'):
                print(f"      {Colors.YELLOW}{line}{Colors.RESET}")
    
    def _print_implementation_summary(self, impl_name: str):
        """Imprime resumen de una implementación"""
        results = self.results[impl_name]
        passed = sum(1 for r in results if r.passed)
        failed = len(results) - passed
        
        print(f"\n{Colors.BOLD}Resumen {impl_name.upper()}:{Colors.RESET}")
        print(f"  Total:   {len(results)}")
        print(f"  {Colors.GREEN}Pasadas: {passed}{Colors.RESET}")
        print(f"  {Colors.RED}Fallidas: {failed}{Colors.RESET}")
    
    def print_final_summary(self):
        """Imprime el resumen final de todas las pruebas"""
        print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*70}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}RESUMEN FINAL{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}{'='*70}{Colors.RESET}\n")
        impl_name = self.impl_name
        total = len(self.results[impl_name])
        passed = sum(1 for r in self.results[impl_name] if r.passed)
        failed = total - passed
        print(f"{Colors.BOLD}Resultados {impl_name.upper()}:{Colors.RESET}")
        print(f"  Total:   {total}")
        print(f"  {Colors.GREEN}Pasadas: {passed}{Colors.RESET}")
        print(f"  {Colors.RED}Fallidas: {failed}{Colors.RESET}")
        print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*70}{Colors.RESET}\n")
        return 0 if failed == 0 else 1
    
    def run_all_tests(self) -> int:
        """Ejecuta todas las pruebas para la implementación actual"""
        try:
            self.run_tests_for_implementation()
            return self.print_final_summary()
        except Exception as e:
            print(f"{Colors.RED}Error durante la ejecución de pruebas:{Colors.RESET}")
            print(f"{Colors.RED}{str(e)}{Colors.RESET}")
            return 1


def main():
    """Función principal"""
    script_dir = Path(__file__).parent
    base_dir = script_dir.parent
    print(f"{Colors.BOLD}Script de Pruebas Funcionales (AOS y SOA){Colors.RESET}")
    print(f"Directorio base: {base_dir}\n")
    exit_code = 0
    try:
        impl = "par"
        print(f"\n{Colors.BOLD}{Colors.YELLOW}========== Lanzando pruebas para {impl.upper()} =========={Colors.RESET}\n")
        tester = FunctionalTester(base_dir, impl)
        code = tester.run_all_tests()
        sys.exit(code)
    except FileNotFoundError as e:
        print(f"{Colors.RED}Error:{Colors.RESET} {str(e)}")
        print(f"\n{Colors.YELLOW}Asegúrate de que el proyecto esté compilado.{Colors.RESET}")
        print(f"{Colors.YELLOW}El ejecutable debe estar en: out/build/default/<impl>/{Colors.RESET}")
        sys.exit(1)
    except Exception as e:
        print(f"{Colors.RED}Error inesperado:{Colors.RESET} {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    main()
