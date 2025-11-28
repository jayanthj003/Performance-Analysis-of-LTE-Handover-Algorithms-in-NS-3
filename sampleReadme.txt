Instructions on running the assignment:
0. Kindly ensure all python modules listed in requirements.txt are installed.
1. Place the directory 'asg2' in ns-3-dev/scratch
2. Open terminal. Go to ns-3-dev/scratch/asg2
3. Execute the following commands:
	- chmod +x go.sh
	- ./go.sh
4. Generated statistics will be in the folder: scratch/asg2.
5. Generated plots will be in the folders: scratch/asg2/Graph* and scratch/asg2/Graph*_Bonus


Submission contains:
1. All the plots:
	- rem2.out (REM plot); plot using the script rem-plot.gp
	- Graph1: contains .png file of the rem plot (no need to run rem-plot.gp manually)
	- Graph2: contains subfolders speed=10, speed=50, speed=100 each 
		  containing .png of the required plots
	- Graph3: contains subfolders speed=10, speed=50, speed=100 each 
		  containing .png of the required plots
	- Graph4: contains subfolders speed=10, speed=50, speed=100 each 
		  containing .png of the required hysteresis plots
	- Graph5: contains subfolders speed=10, speed=50, speed=100 each 
		  containing .png of the required neighbour offset plots
	- Graph6: contains subfolders A2A4RsrqHandover, A3RsrpHandover,
	          NoOpHandover each containing the required average aggregate
	          system throughput plot.
	- Graph1_Bonus: contains subfolders speed=10, speed=50, speed=100 each
			containing .png of the required plots.
	- Graph2_Bonus: contains subfilders speed=10, speed=50, speed=100 each
			containing -png of the cwnd plots.
2. All the collected stats:
	- NoOp: avg agg system tput data for NoOpHandoverAlg
	- A3Rsrp: avg agg system tput data for A3RsrpHandoverAlg
	- A2A4Rsrq: avg agg system tput data for A2A4RsrqHandoverAlg
	- hyst_report: number of handovers for different hysteresis values
	- nb_offset_report: number of handovers for different neighbour offset 
			    values
3. Main NS-3 script (asg2.cc)
4. Python and gnuplot scripts for plotting data:
	- rem-plot.gp: Gnuplot script for plotting REM plot
	- offset_plotter.py: Plots Graph-5
	- hyst_plotter.py: Plots Graph-4
	- plot_agg_tputs.py: Plots Graph-6
	- plot_inst_tput.py: Plots Graph-3
	- plot_rsrp.py: Plots Graph-2
	- plot_inst_tput_bonus.py: Plots Graph-1 for bonus task
	- plot_cwnd_bonus.py: Plots Graph-2 for the bonus task
5. Additional Files:
	- max_handover_IMSI: used for identifying UE with maximum handovers and 
	  other intermediate operations.
	- requirements.txt: python modules required for successfully executing the 
	                    assignment.

