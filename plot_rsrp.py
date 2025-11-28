import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

rsrp_file = '../../DlRsrpSinrStats.txt'
max_handover_imsi_file = './max_handover_IMSI'

with open(max_handover_imsi_file, 'r') as file:
    s = file.read().split()

max_ho_imsi = int(s[0])
num_handovers = int(s[1])
rngRun = int(s[2])
speed = int(s[3])



### IMSI Plot ###
plt.title(f'Rsrp vs. Time; IMSI={max_ho_imsi}, num_handovers={num_handovers}')

df = pd.read_csv(rsrp_file, sep='\t', index_col=False)
df = df[df.IMSI == max_ho_imsi]


start_time = df.iloc[0, 0]
x_vals = np.arange(start_time, 10, 0.01)
indices = df.index[range(0, df.shape[0], 10)]

plt.xlabel('Time(sec/10\'s of ms)')
plt.ylabel('rsrp')


rsrp_dbm = 10 * np.log10(df.rsrp[indices]) + 30
plt.scatter(x_vals, rsrp_dbm, s=5)


import os
save_dir = f'Graph2/speed={speed}/'
os.makedirs(save_dir, exist_ok=True)
plt.savefig(os.path.join(save_dir,'rsrp_plot.png'), dpi=300)

