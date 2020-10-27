from os import listdir, getcwd
import sys
from statistics import stdev
import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

# x = input("qual tipo de arquivo analisar")
# scheds = ('pequeno/', 'media/', 'grande/')
output = "./output/"
# input_ = "./input/"

# le as context changes e as deadlines

tests = []
stdevs_time = []
stdevs_memory = []
time_ = []
memory_ = []
data = ''
numbers = []

filenames = listdir(output)

means = []
stdevs = []
for num in range(1, 10):
    memory = []
    time = []
    for x in filenames:
        if "output"+str(num) in x:
            with open(output + x, "r") as file_:
                data = file_.read()
                numbers = [int(i) for i in data.split(' ')]
                time.append(numbers[0])
                memory.append(numbers[1])
                
    print(time, memory)
    if (len(time) == 0):
        time_.append(0)
        stdevs_time.append(0)
    else:
        time_.append(sum(time)/len(time))
        stdevs_time.append(stdev(time)) 
    if (len(memory) == 0):
        memory_.append(0)
        stdevs_memory.append(0)

    else :
        stdevs_memory.append(stdev(memory))
        memory_.append(sum(memory)/len(memory))


n = 30

interval_time = []
interval_memory = []
for x in range(0, 9):
    interval_time.append((1.699*stdevs_time[x])/pow(n,1/2))
    interval_memory.append((1.699*stdevs_memory[x])/pow(n,1/2))

bars = ("1", "2", "3", "4", "5", "6", "7", "8", "9")

y_pos = np.arange(len(bars))
print(interval_memory, interval_time)
yerr = [interval_memory[i] for i in range(0, 9)]

plt.figure(figsize=(20,20), dpi=100)
ax0 = plt.subplot(3, 3, 3)
plt.bar(y_pos, time_, align='center', yerr=yerr, alpha=0.5, capsize=7)
plt.xticks(y_pos, bars)
plt.ylabel('Média memória')
plt.title('Média de uso de memória durante as corridas')

plt.savefig("./pngs/memory.png")

y_pos = np.arange(len(bars))
yerr = [interval_time[i] for i in range(0, 9)]

plt.figure(figsize=(20,20), dpi=100)
ax0 = plt.subplot(3, 3, 3)
plt.bar(y_pos, time_, align='center', yerr=yerr, alpha=0.5, capsize=7)
plt.xticks(y_pos, bars)
plt.ylabel('Média tempo')
plt.title('Média de tempo passado durante as corridas')

plt.savefig("./pngs/time.png")


