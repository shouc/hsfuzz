import time
import main
from multiprocessing import Process
for i in range(6):
    p1 = Process(target=main.main, args=(i,))
    p1.start()
time.sleep(10000000)

