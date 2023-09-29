import os
import matplotlib.pyplot as plt

processes = [p for p in range(1, 7)]
segments = [1e3, 1e6, 1e8]

for i in segments:
    print(f'Segments count = {i}')
    for j in processes:
        print(f'\nProcesses count = {j}')
        os.system(f'./run.sh {j} {i}')
    print("")

time = []

file = open('time.txt', 'r')
for line in file:
    time.append(float(line[0 : len(line) - 1]))

fig, axs = plt.subplots(nrows=len(segments), ncols=1, figsize=(12, 10))
fig.tight_layout(pad=5.0)

for i in range (0, len(segments)):
    axs[i].plot(time[i * len(processes): (i + 1) * len(processes)], processes)
    axs[i].set_title(f'N = {segments[i]}', fontsize=12)
    axs[i].set_xlabel('Program execution time in seconds')
    axs[i].set_ylabel('Processes num')
    axs[i].grid()
plt.show()
