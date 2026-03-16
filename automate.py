import os
import subprocess
import multiprocessing
import time

# List your module folders here
modules = [
    'Capture',
    'Parser',
    'SIPController',
    'SIPSessionManager',
    'SIPSessionMongoDB'
]

# Set build type: 'Debug' or 'Release'
BUILD_TYPE = 'Release'

def run_conan(module_path):
    try:
        subprocess.run([
            'conan', 'install', '.', '--build', 'missing',
            f'--settings=build_type={BUILD_TYPE}'], cwd=module_path, check=True,
                                                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError as e:
        print(f"[✗] Conan install failed for {module_path}\n→ Command: {e.cmd}")
        raise


def run_cmake(module_path):
    build_path = os.path.join(module_path, 'build')
    try:
        subprocess.run([
            'cmake', '..',
            f'-DCMAKE_BUILD_TYPE={BUILD_TYPE}'], cwd=build_path, check=True,
           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        subprocess.run(['cmake', '--build', '.'], cwd=build_path, check=True,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError as e:
        print(f"[✗] CMake failed for {module_path}\n→ Command: {e.cmd}")
        raise


def run_executable(module_name):
    module_path = os.path.abspath(module_name)
    build_path = os.path.join(module_path, 'build')
    executable = './main'
    
    if module_name == 'Capture':
        subprocess.run(['sudo', executable], cwd=build_path,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    else:
        subprocess.run([executable], cwd=build_path,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def setup_and_run(module_name):
    module_path = os.path.abspath(module_name)
    print(f"[~] Setting up {module_name}...")

    run_conan(module_path)
    run_cmake(module_path)
    run_executable(module_name)

if __name__ == "__main__":
    try:
        subprocess.run(['sudo', '-v'], check=True)
    except subprocess.CalledProcessError:
        print("[✗] Failed to authenticate sudo privileges.")
        exit(1)

    processes = []

    for module in modules:
        p = multiprocessing.Process(target=setup_and_run, args=(module,))
        p.start()
        processes.append(p)
        time.sleep(1)  # Optional: stagger startup

    time.sleep(3)
    print("\n[✓] All modules started. Waiting for them to finish (Ctrl+C to stop)...")

    for p in processes:
        p.join()
