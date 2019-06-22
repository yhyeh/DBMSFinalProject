import os
import time


start_time = time.time()
os.system("./shell < test/system/testcases/large/t1.txt > t1.txt")
print (time.time() - start_time)

start_time = time.time()
os.system("./shell < test/system/testcases/large/t2.txt > t2.txt")
print (time.time() - start_time)

start_time = time.time()
os.system("./shell < test/system/testcases/large/t3.txt > t3.txt")
print (time.time() - start_time)

start_time = time.time()
os.system("./shell < test/system/testcases/large/t4.txt > t4.txt")
print (time.time() - start_time)

start_time = time.time()
os.system("./shell < test/system/testcases/large/t5.txt > t5.txt")
print (time.time() - start_time)
