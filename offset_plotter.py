import pandas as pd
import matplotlib.pyplot as plt

offset_file = './nb_offset_report'

df = pd.read_csv(offset_file, sep=' ', index_col=False)

avg_df = df.groupby(['Speed', 'Neighbour_Cell_Offset'])['Handover_Num'].mean().reset_index()
avg_df.rename(columns={'Handover_Num': 'Avg_Handover_Num'}, inplace=True)

#print(avg_df)
import os

# Plot 1: Speed = 10 m/s
df_10 = avg_df[avg_df.Speed==10]
plt.bar(df_10.Neighbour_Cell_Offset, df_10.Avg_Handover_Num)
plt.xlabel('Neighbour Cell Offset (dB)')
plt.ylabel('Average Number of Handovers')
save_dir = f'Graph5/speed=10/'
os.makedirs(save_dir, exist_ok=True)
plt.savefig(os.path.join(save_dir,'offset_plot.png'), dpi=300)


# Plot 2: Speed = 50 m/s
df_50 = avg_df[avg_df.Speed==50]
plt.bar(df_50.Neighbour_Cell_Offset, df_50.Avg_Handover_Num)
plt.xlabel('Neighbour Cell Offset (dB)')
plt.ylabel('Average Number of Handovers')
save_dir = f'Graph5/speed=50/'
os.makedirs(save_dir, exist_ok=True)
plt.savefig(os.path.join(save_dir,'offset_plot.png'), dpi=300)


# Plot 3: Speed = 100 m/s
df_100 = avg_df[avg_df.Speed==100]
plt.bar(df_100.Neighbour_Cell_Offset, df_100.Avg_Handover_Num)
plt.xlabel('Neighbour Cell Offset (dB)')
plt.ylabel('Average Number of Handovers')
save_dir = f'Graph5/speed=100/'
os.makedirs(save_dir, exist_ok=True)
plt.savefig(os.path.join(save_dir,'offset_plot.png'), dpi=300)
