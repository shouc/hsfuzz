import os
REPO_DIR = "/home/shou/repos"
for i in os.listdir(REPO_DIR):
    folder = REPO_DIR + "/" + i
    os.system(f"mkdir {folder}/cov")
    os.chdir(folder)
    os.system(f"/home/shou/coding/hsfuzz/php-src/sapi/cli/php "
              f"-dextension=/home/shou/coding/hsfuzz/corbfuzz_ext/modules/hsfuzz.so -S 0.0.0.0:1194 &")
    os.chdir("/home/shou/coding/hsfuzz")
    os.system("timeout 1m python3 start.py")
    os.system("pkill -f \"php\"")

# /home/shou/repos/43337c58-1368-4e5a-809f-13de404cd475
