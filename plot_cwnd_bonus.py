import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

max_handover_imsi_file = './max_handover_IMSI'

with open(max_handover_imsi_file, 'r') as file:
    s = file.read().split()

max_ho_imsi = int(s[0])
num_handovers = int(s[1])
rngRun = int(s[2])
speed = int(s[3])

ue_cwnd_file = f'../../UE{max_ho_imsi}.cwnd'

### CWND plots ###
plt.title(f'CWND size vs. Time; IMSI={max_ho_imsi}, num_handovers={num_handovers}')

df = pd.read_csv(ue_cwnd_file, sep='\t', index_col=False)
df.columns = ['Time', 'oldCwnd', 'newCwnd']
df = df.drop(['newCwnd'], axis=1)

plt.xlabel('Time(sec)')
plt.ylabel('CWND(bytes)')

plt.plot(df.Time, df.oldCwnd)
plt.gca().xaxis.set_major_locator(ticker.MultipleLocator(0.10))

for tick, tick_val in zip(plt.gca().xaxis.get_major_ticks(), plt.gca().get_xticks()):
    if abs(tick_val % 1.0) < 1e-8:
        tick.tick1line.set_markersize(10)
        tick.tick2line.set_markersize(10)
    else:
        tick.tick1line.set_markersize(5)
        tick.tick2line.set_markersize(5)



def format_fn(x, pos):
    return f'{x:.1f}' if abs(x % 1.0) < 1e-8 else ''  # using abs and 1e-8 to avoid float precision issues

plt.gca().xaxis.set_major_formatter(ticker.FuncFormatter(format_fn))

import os
save_dir = f'Graph2_Bonus/speed={speed}'
os.makedirs(save_dir, exist_ok=True)
plt.savefig(os.path.join(save_dir,'cwnd_plot.png'), dpi=300)
