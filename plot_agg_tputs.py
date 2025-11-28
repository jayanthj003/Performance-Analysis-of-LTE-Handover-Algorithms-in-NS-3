import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

noop_file = './NoOp'
a3rsrp_file = './A3Rsrp'
a2a4rsrq_file = './A2A4Rsrq'

df_noop = pd.read_csv(noop_file, index_col=False, sep=' ')
df_a3rsrp = pd.read_csv(a3rsrp_file, index_col=False, sep=' ')
df_a2a4rsrq = pd.read_csv(a2a4rsrq_file, index_col=False, sep=' ')

avg_noop_tput = df_noop.groupby('Speed')['AggTput(bps)'].mean().reset_index()
avg_a3rsrp_tput = df_a3rsrp.groupby('Speed')['AggTput(bps)'].mean().reset_index()
avg_a2a4rsrq_tput = df_a2a4rsrq.groupby('Speed')['AggTput(bps)'].mean().reset_index()

plt.bar(avg_noop_tput.Speed, avg_noop_tput.loc[:, 'AggTput(bps)'])
plt.title('Average Aggregate System Throughput vs. Speed (NoOpHandoverAlgorithm)')
plt.xlabel('Speed (m/s)')
plt.ylabel('Average Aggregate System Throughput (bps)')
save_dir = f'Graph6/NoOpHandover/'
os.makedirs(save_dir, exist_ok=True)
plt.savefig(os.path.join(save_dir,'noop_handover.png'), dpi=300)



plt.bar(avg_a3rsrp_tput.Speed, avg_a3rsrp_tput.loc[:, 'AggTput(bps)'])
plt.title('Average Aggregate System Throughput vs. Speed (A3RsrpHandoverAlgorithm)')
plt.xlabel('Speed (m/s)')
plt.ylabel('Average Aggregate System Throughput (bps)')
save_dir = f'Graph6/A3RsrpHandover/'
os.makedirs(save_dir, exist_ok=True)
plt.savefig(os.path.join(save_dir,'a3rsrp_handover.png'), dpi=300)



plt.bar(avg_a2a4rsrq_tput.Speed, avg_a2a4rsrq_tput.loc[:, 'AggTput(bps)'])
plt.title('Average Aggregate System Throughput vs. Speed (A2A4RsrqHandoverAlgorithm)')
plt.xlabel('Speed (m/s)')
plt.ylabel('Average Aggregate System Throughput (bps)')
save_dir = f'Graph6/A2A4RsrqHandover/'
os.makedirs(save_dir, exist_ok=True)
plt.savefig(os.path.join(save_dir,'a2a4rsrq_handover.png'), dpi=300)
