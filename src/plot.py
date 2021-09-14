import matplotlib.pyplot as plt
import numpy as np
import csv

datas = {}

with open('videos/out.txt','r') as csvfile:
    data = csv.DictReader(csvfile, delimiter=' ')
    for row in data:
        for fieldname in data.fieldnames:
            datas.setdefault(fieldname, []).append(float(row.get(fieldname)))

i = list(range(1, len(datas['x'])+1))
fig, axs = plt.subplots(2, 2)

axs[0, 0].plot   (i, datas['x'],  label='X')
axs[0, 0].plot   (i, datas['y'],  label='Y')
axs[0, 0].scatter(i, datas['zx'], label='Z_x', marker='x')
axs[0, 0].scatter(i, datas['zy'], label='Z_y', marker='x')
axs[0, 0].set_title("X/Y")
axs[0, 0].legend()

axs[0, 1].plot   (i, datas['vx'],  label='Vx')
axs[0, 1].plot   (i, datas['vy'],  label='Vy')
axs[0, 1].scatter(i, datas['zvx'], label='Z_vx', marker='x')
axs[0, 1].scatter(i, datas['zvy'], label='Z_vy', marker='x')
axs[0, 1].set_title("Vx/Vy")
axs[0, 1].legend()

axs[1, 0].plot   (i, datas['ax'],  label='Ax')
axs[1, 0].plot   (i, datas['ay'],  label='Ay')
axs[1, 0].scatter(i, datas['zax'], label='Z_ax', marker='x')
axs[1, 0].scatter(i, datas['zay'], label='Z_ay', marker='x')
axs[1, 0].set_title("Ax/Ay")
axs[1, 0].legend()
plt.show()
