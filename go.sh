#!/bin/bash

# Generate the REM Plot
# Use any run number. Doesn't matter.
../../ns3 run "scratch/asg2/asg2.cc --RngRun=1 --plot_rem=true"
mv ../../rem2.out ./
mkdir -p Graph1
gnuplot -persist rem-plot.gp


# Run simulation for rngRun=4, speed=50m/s to identify a UEx with maximum number of handovers
echo "Running NS-3 simulation (A3RsrpHandoverAlgorithm) with RngRun=4, speed=50 ..."
# Execute NS-3 simulation
../../ns3 run "scratch/asg2/asg2.cc --RngRun=4 --speed=50 --handover_algo=ns3::A3RsrpHandoverAlgorithm"

ue_with_max_handovers=$(head -n 1 max_handover_IMSI | awk '{print $1}')
echo "UE with max handovers: $ue_with_max_handovers"


# Fix seed at rngRun=4
# To get rsrp and inst_tput traces for speed=10m/s
../../ns3 run "scratch/asg2/asg2.cc --RngRun=4 --speed=10 --handover_algo=ns3::A3RsrpHandoverAlgorithm --ue_to_track=$ue_with_max_handovers"
python3 plot_rsrp.py
python3 plot_inst_tput.py

# To get rsrp trace and inst_tput traces for speed=50m/s
../../ns3 run "scratch/asg2/asg2.cc --RngRun=4 --speed=50 --handover_algo=ns3::A3RsrpHandoverAlgorithm --ue_to_track=$ue_with_max_handovers"
python3 plot_rsrp.py
python3 plot_inst_tput.py

# To get rsrp trace and inst_tput traces for speed=100m/s
../../ns3 run "scratch/asg2/asg2.cc --RngRun=4 --speed=100 --handover_algo=ns3::A3RsrpHandoverAlgorithm --ue_to_track=$ue_with_max_handovers"
python3 plot_rsrp.py
python3 plot_inst_tput.py

echo "Graph-2 and Graph-3 done." > G2G3Done.alert



# Create a new file for hysteresis measurements
echo "RngRun Speed Hysteresis Handover_Num" > hyst_report

# Collects data for Graph-4
for hysteresis in 1 3 6; do
    for rngrun in {4..8}; do  
        for speed in 10 50 100; do                    
            echo "Running NS-3 simulation (A3RsrpHandoverAlgorithm) with RngRun=$rngrun, speed=$speed, hysteresis=$hysteresis..."

            # Execute NS-3 simulation
            ../../ns3 run "scratch/asg2/asg2.cc --RngRun=$rngrun --speed=$speed --handover_algo=ns3::A3RsrpHandoverAlgorithm --hysteresis=$hysteresis"
                
        done
    done
done

# This plots Graph-4
python3 hyst_plotter.py
echo "Graph 4 Done." > G4Done.alert


# Create a new file for Neighbour Cell Offset measurements
echo "RngRun Speed Neighbour_Cell_Offset Handover_Num" > nb_offset_report

# Collects data for Graph-5
for offset in 1 2 5; do
    for rngrun in {4..8}; do  
        for speed in 10 50 100; do                            
            echo "Running NS-3 simulation (A2A4RsrqHandoverAlgorithm) with RngRun=$rngrun, speed=$speed, --neighbour_cell_offset=$offset..."

            # Execute NS-3 simulation
            ../../ns3 run "scratch/asg2/asg2.cc --RngRun=$rngrun --speed=$speed --handover_algo=ns3::A2A4RsrqHandoverAlgorithm --neighbour_cell_offset=$offset"                
        done    
    done
done

# This plots Graph-5
python offset_plotter.py
echo "Graph 5 Done." > G5Done.alert

# IMPORTANT: THREE SEPARATE TABLES
# ONE FOR: NOOP, A3 with hysteresis = best_value, A2A4 with offset = best_value each
# Each table must have format RngRun Speed AggTput

# Looking at the graphs, best value of hysteresis=3dB and best value of offset=2


# NoOp Handover algorithm
echo "RngRun Speed AggTput(bps)" > NoOp

for rngrun in {4..8}; do
    for speed in 10 50 100; do
        echo "Running NS-3 simulation (NoOpHandoverAlgorithm) with RngRun=$rngrun, speed=$speed..."

        # Execute NS-3 simulation
        ../../ns3 run "scratch/asg2/asg2.cc --RngRun=$rngrun --speed=$speed --handover_algo=ns3::NoOpHandoverAlgorithm --agg_tput_collect=true"
    done
done

# A3Rsrp Handover algorithm
echo "RngRun Speed AggTput(bps)" > A3Rsrp

for rngrun in {4..8}; do
    for speed in 10 50 100; do
        echo "Running NS-3 simulation (A3RsrpHandoverAlgorithm) with RngRun=$rngrun, speed=$speed, hysteresis=3..."

        # Execute NS-3 simulation
        ../../ns3 run "scratch/asg2/asg2.cc --RngRun=$rngrun --speed=$speed --handover_algo=ns3::A3RsrpHandoverAlgorithm --hysteresis=3 --agg_tput_collect=true"
    done
done

# A2A4Rsrq Handover algorithm
echo "RngRun Speed AggTput(bps)" > A2A4Rsrq

for rngrun in {4..8}; do
    for speed in 10 50 100; do
        echo "Running NS-3 simulation (A2A4RsrqHandoverAlgorithm) with RngRun=$rngrun, speed=$speed, offset=2..."

        # Execute NS-3 simulation
        ../../ns3 run "scratch/asg2/asg2.cc --RngRun=$rngrun --speed=$speed --handover_algo=ns3::A2A4RsrqHandoverAlgorithm --neighbour_cell_offset=2 --agg_tput_collect=true"
    done
done


# Plot the System Aggregate Throughputs for different Handover Algorithms
python3 plot_agg_tputs.py


# BONUS PLOTS
# Fix seed at rngRun=4
# To get rsrp and cwnd plots for speed=10m/s
../../ns3 run "scratch/asg2/asg2.cc --bonus=true --RngRun=4 --speed=10 --handover_algo=ns3::A3RsrpHandoverAlgorithm --ue_to_track=$ue_with_max_handovers"
python3 plot_inst_tput_bonus.py
python3 plot_cwnd_bonus.py

# To get rsrp trace and cwnd plots traces for speed=50m/s
../../ns3 run "scratch/asg2/asg2.cc --bonus=true --RngRun=4 --speed=50 --handover_algo=ns3::A3RsrpHandoverAlgorithm --ue_to_track=$ue_with_max_handovers"
python3 plot_inst_tput_bonus.py
python3 plot_cwnd_bonus.py

# To get rsrp trace and cwnd_plots traces for speed=100m/s
../../ns3 run "scratch/asg2/asg2.cc --bonus=true --RngRun=4 --speed=100 --handover_algo=ns3::A3RsrpHandoverAlgorithm --ue_to_track=$ue_with_max_handovers"
python3 plot_inst_tput_bonus.py
python3 plot_cwnd_bonus.py
