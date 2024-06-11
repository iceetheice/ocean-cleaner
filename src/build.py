import subprocess

cc = "g++"
program_name = "game"

command = [cc, "main.cpp", "-o", program_name+".exe", "-lraylib", "-lgdi32", "-lwinmm"]

try:
    subprocess.run(command, check=True)
except subprocess.CalledProcessError as e:
    print(e.stderr)
