import matplotlib.pyplot as plt
import numpy as np
import csv

datas = {}

with open('videos/out.txt','r') as csvfile:
    data = csv.DictReader(csvfile, delimiter=' ')
    for row in data:
        for fieldname in data.fieldnames:
            datas.setdefault(fieldname, []).append(float(row.get(fieldname)))

i = list(range(1, len(datas['xp'])+1))
plt.plot   (i, datas['xp'], label='Xp')
plt.plot   (i, datas['yp'], label='Yp')
plt.scatter(i, datas['zx'], label='Zx')
plt.scatter(i, datas['zy'], label='Zy')
plt.legend()
plt.show()
