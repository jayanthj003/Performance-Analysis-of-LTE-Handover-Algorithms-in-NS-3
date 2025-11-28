#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/position-allocator.h"
#include "ns3/spectrum-module.h"
#include "ns3/rng-seed-manager.h"


#include "ns3/bulk-send-helper.h"
#include "ns3/bulk-send-application.h"
#include "ns3/inet-socket-address.h"

#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"


#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Assgn2");

/**
 * UE Connection established notification.
 *
 * @param context The context.
 * @param imsi The IMSI of the connected terminal.
 * @param cellid The Cell ID.
 * @param rnti The RNTI.
 */



// To count number of handovers for each UE
std::vector<int> ueHandoverCount (24, 0);

// Aggregate throughput value
int agg_tput {0};

void printStats(FlowMonitorHelper &flowmon_helper, bool perFlowInfo)
{
	  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon_helper.GetClassifier());
	  std::string proto;
	  Ptr<FlowMonitor> monitor = flowmon_helper.GetMonitor ();
	  std::map < FlowId, FlowMonitor::FlowStats > stats = monitor->GetFlowStats();
	  double totalTimeReceiving;
	  uint64_t totalPacketsReceived, totalPacketsDropped, totalBytesReceived;

	  totalBytesReceived = 0, totalPacketsDropped = 0, totalPacketsReceived = 0, totalTimeReceiving = 0;
	  for (std::map< FlowId, FlowMonitor::FlowStats>::iterator flow = stats.begin(); flow != stats.end(); flow++)
	  {
	    Ipv4FlowClassifier::FiveTuple  t = classifier->FindFlow(flow->first);
	    switch(t.protocol)
	     {
	     case(6):
	         proto = "TCP";
	         break;
	     case(17):
	         proto = "UDP";
	         break;
	     default:
	         exit(1);
	     }
	     totalBytesReceived += (double) flow->second.rxBytes * 8;
	     totalTimeReceiving += flow->second.timeLastRxPacket.GetSeconds ();
	     totalPacketsReceived += flow->second.rxPackets;
	     totalPacketsDropped += flow->second.txPackets - flow->second.rxPackets;
	     if (perFlowInfo) {
		   agg_tput += ( ((double)flow->second.rxBytes*8) / (flow->second.timeLastRxPacket.GetSeconds ()) );
	     }
	   }
}



void
NotifyConnectionEstablishedUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
              << ": connected to CellId " << cellid << " with RNTI " << rnti << std::endl;
}

/**
 * UE Start Handover notification.
 *
 * @param context The context.
 * @param imsi The IMSI of the connected terminal.
 * @param cellid The actual Cell ID.
 * @param rnti The RNTI.
 * @param targetCellId The target Cell ID.
 */
void
NotifyHandoverStartUe(std::string context,
                      uint64_t imsi,
                      uint16_t cellid,
                      uint16_t rnti,
                      uint16_t targetCellId)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
              << ": previously connected to CellId " << cellid << " with RNTI " << rnti
              << ", doing handover to CellId " << targetCellId << std::endl;
}

/**
 * UE Handover end successful notification.
 *
 * @param context The context.
 * @param imsi The IMSI of the connected terminal.
 * @param cellid The Cell ID.
 * @param rnti The RNTI.
 */
void
NotifyHandoverEndOkUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
              << ": successful handover to CellId " << cellid << " with RNTI " << rnti << std::endl;
	
	++ueHandoverCount[imsi-1];
}

/**
 * eNB Connection established notification.
 *
 * @param context The context.
 * @param imsi The IMSI of the connected terminal.
 * @param cellid The Cell ID.
 * @param rnti The RNTI.
 */
void
NotifyConnectionEstablishedEnb(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
              << ": successful connection of UE with IMSI " << imsi << " RNTI " << rnti
              << std::endl;
}

/**
 * eNB Start Handover notification.
 *
 * @param context The context.
 * @param imsi The IMSI of the connected terminal.
 * @param cellid The actual Cell ID.
 * @param rnti The RNTI.
 * @param targetCellId The target Cell ID.
 */
void
NotifyHandoverStartEnb(std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti,
                       uint16_t targetCellId)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
              << ": start handover of UE with IMSI " << imsi << " RNTI " << rnti << " to CellId "
              << targetCellId << std::endl;
}

/**
 * eNB Handover end successful notification.
 *
 * @param context The context.
 * @param imsi The IMSI of the connected terminal.
 * @param cellid The Cell ID.
 * @param rnti The RNTI.
 */
void
NotifyHandoverEndOkEnb(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
              << ": completed handover of UE with IMSI " << imsi << " RNTI " << rnti << std::endl;
}

/**
 * Handover failure notification
 *
 * @param context The context.
 * @param imsi The IMSI of the connected terminal.
 * @param cellid The Cell ID.
 * @param rnti The RNTI.
 */
void
NotifyHandoverFailure(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB CellId " << cellid
              << " IMSI " << imsi << " RNTI " << rnti << " handover failure" << std::endl;
}

void 
CheckPosition(Ptr<Node> node, int i)
{
	Ptr<MobilityModel> m = node->GetObject<MobilityModel>();
	Vector position = m->GetPosition();
	std::cout << Simulator::Now().As(Time::S) << "--> " 
	<< "UE " << i << " at position " << position.x << ", " << position.y << ", " << position.z << "\n";
	Simulator::Schedule(Seconds(0.5), &CheckPosition, node, i);
}

static void
CwndChange(Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\t" << newCwnd);
    *stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << oldCwnd << "\t" << newCwnd
                         << std::endl;
}


int main(int argc, char *argv[]) {


	// 50 RB in downlink, 50 in uplink
	Config::SetDefault("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue(50));
	Config::SetDefault("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue(50));

	// Set Tx Power of each eNB to 20dBm
	Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(20.0));

	// TCP Congestion Control Algorithm is TCP NewReno
	Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));

	// Set UdpClient Attributes
	Config::SetDefault("ns3::UdpClient::MaxPackets", UintegerValue(0));
	Config::SetDefault("ns3::UdpClient::Interval", TimeValue(MilliSeconds(10)));
	Config::SetDefault("ns3::UdpClient::PacketSize", UintegerValue(1500));



	double ue_mobility_speed = 10.0;
	std::string handover_algo_type = "ns3::A2A4RsrqHandoverAlgorithm";
	Time simTime = Seconds(10.0);
	bool plot_rem {false};
	double hysteresis {1.0};
	unsigned int neighbour_cell_offset {1};
	int ue_to_track {-1};
	bool agg_tput_collect {false};
	bool bonus {false};
	CommandLine cmd(__FILE__);
	cmd.AddValue("speed", "UE mobility speed", ue_mobility_speed);
	cmd.AddValue("handover_algo", "Handover algorithm type", handover_algo_type);
	cmd.AddValue("plot_rem", "Whether to generate rem or not", plot_rem);
	cmd.AddValue("hysteresis", "hysteresis value for A3RsrpHandoverAlgorithm", hysteresis);
	cmd.AddValue("neighbour_cell_offset", "neighbour cell offset value for A2A4RsrqHandoverAlgorithm", neighbour_cell_offset);
	cmd.AddValue("ue_to_track", "UE whose number of handovers you would like", ue_to_track);
	cmd.AddValue("agg_tput_collect", "whether or not to collect agg system tput", agg_tput_collect);
	cmd.AddValue("bonus", "whether or not to perform the bonus TCP task", bonus);
	cmd.Parse(argc, argv);

	Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
	Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
	lteHelper->SetEpcHelper(epcHelper);
	lteHelper->SetSchedulerType("ns3::PfFfMacScheduler"); // Proportional fair MAC scheduler
	lteHelper->SetHandoverAlgorithmType(handover_algo_type.c_str()); // Handover algorithm type (A2A4RsrqHandoverAlgorithm) by default

	if(handover_algo_type == std::string("ns3::A3RsrpHandoverAlgorithm"))
		lteHelper->SetHandoverAlgorithmAttribute("Hysteresis", DoubleValue(hysteresis));
	if(handover_algo_type == std::string("ns3::A2A4RsrqHandoverAlgorithm"))
		lteHelper->SetHandoverAlgorithmAttribute("NeighbourCellOffset", UintegerValue(neighbour_cell_offset));


	// Create a remote host
	NodeContainer remoteHostContainer;
	remoteHostContainer.Create(1);
	Ptr<Node> remoteHost = remoteHostContainer.Get(0);
	InternetStackHelper internet;
	internet.Install(remoteHostContainer);

	// Get PGW node
	Ptr<Node> pgw = epcHelper->GetPgwNode();

	// Connect the two with a point to point link of 1Gbps
	PointToPointHelper p2ph;
	p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("1Gb/s")));
	p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
	p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
	NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
	Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    //Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

	// This is from lena-x2-handover.cc
	// Routing of the Internet Host (towards the LTE network)
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    // interface 0 is localhost, 1 is the p2p device
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);


	// Now configure the topology
	NodeContainer ueNodes;
	NodeContainer enbNodes;
	enbNodes.Create(8); //  8 eNodeB's as asked in the assignment
	ueNodes.Create(3*8);// 3 UEs per eNB
	

	// eNB Placement
	Ptr<ListPositionAllocator> enbpositionAlloc = CreateObject<ListPositionAllocator> ();
	
	enbpositionAlloc->Add(Vector(0,0,0)); // Cell 1
	enbpositionAlloc->Add(Vector(0,-250,0)); // Cell 2
	enbpositionAlloc->Add(Vector(250,-250,0)); // Cell 3
	enbpositionAlloc->Add(Vector(250,0,0)); // Cell 4
	enbpositionAlloc->Add(Vector(500,0,0)); // Cell 5
	enbpositionAlloc->Add(Vector(500,-250,0)); // Cell 6
	enbpositionAlloc->Add(Vector(750,0,0)); // Cell 7
	enbpositionAlloc->Add(Vector(750,-250,0)); // Cell 8

	// UE initial placement
	Ptr<ListPositionAllocator> uepositionAlloc = CreateObject<ListPositionAllocator>();

	for(int i = 0; i < 8; ++i) {
		auto current_enb = enbpositionAlloc->GetNext();
		
		Ptr<UniformDiscPositionAllocator> udpa = CreateObject<UniformDiscPositionAllocator>();
		udpa->SetX(current_enb.x);
		udpa->SetY(current_enb.y);
		udpa->SetZ(0);
		udpa->SetRho(100);
		
		// 3 UEs per eNB.
		for (int j=0; j < 3; ++j) {
			uepositionAlloc->Add(udpa->GetNext());
		}
	}

	// eNB Mobility Model
	MobilityHelper enbmobility;
	enbmobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	enbmobility.SetPositionAllocator(enbpositionAlloc);
	enbmobility.Install(enbNodes);

	// UE Mobility Model
	std::string speed_string = (std::string("ns3::ConstantRandomVariable[Constant=") + std::to_string(ue_mobility_speed) + std::string("]"));
	MobilityHelper uemobility;
	uemobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",	// Random walk 2d mobility model
                          "Mode", StringValue("Time"),	
                          "Time", StringValue("1s"),	// Change direction every 1s
                          "Speed", StringValue(speed_string.c_str()),
                          "Bounds", RectangleValue(Rectangle(-250, 1000, -500, 250)));	// This ensures UEs move within range of the 8 cells
	
	uemobility.SetPositionAllocator(uepositionAlloc);
	uemobility.Install(ueNodes);

	// Install LTE Devices in eNB and UEs	
	NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
	NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

	// Install the IP stack on the UEs
	internet.Install(ueNodes);
	Ipv4InterfaceContainer ueIpIfaces;
    ueIpIfaces = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));

	// Attach all UEs to the eNBs
	for(int i = 0; i < 8; ++i) {
		for(int j=0; j<3; ++j) {
			lteHelper->Attach(ueLteDevs.Get(i*3+j), enbLteDevs.Get(i));
		}
	}

/******************************************************************************************************************* */
	// This is from lena-x2-handover.cc
	//NS_LOG_LOGIC("setting up applications");

	// Install and start applications on UEs and remote host
    uint16_t dlPort = 10000;
	

    // randomize a bit start times to avoid simulation artifacts
    // (e.g., buffer overflows due to packet transmissions happening
    // exactly at the same time)
    Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable>();
    startTimeSeconds->SetAttribute("Min", DoubleValue(0.05));
    startTimeSeconds->SetAttribute("Max", DoubleValue(0.06));

	// This is to get the cwnd for different TCP clients
	for (uint32_t u = 0; u < 24; ++u)
    {
		
        Ptr<Node> ue = ueNodes.Get(u);
        
		// Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(ue->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

		// Set up downlink from Remote Host to UE
		ApplicationContainer clientApps;
        ApplicationContainer serverApps;

        Ptr<EpcTft> tft = Create<EpcTft>();
		
		++dlPort;
     
	 //   NS_LOG_LOGIC("installing UDP DL app for UE ");
        UdpClientHelper dlClientHelperUdp(ueIpIfaces.GetAddress(u), dlPort);

		
		BulkSendHelper dlClientHelperTcp("ns3::TcpSocketFactory", InetSocketAddress(ueIpIfaces.GetAddress(u), dlPort));
		dlClientHelperTcp.SetAttribute("SendSize", UintegerValue(10000));
		dlClientHelperTcp.SetAttribute("MaxBytes", UintegerValue(0));
		
		
		if (bonus)
		{
    		auto clientAppContainer = dlClientHelperTcp.Install(remoteHost);
    		clientApps.Add(clientAppContainer);

    		
    		Simulator::Schedule(
        		Seconds(1.001), 
        		[=]() {
					auto app = DynamicCast<BulkSendApplication> (clientAppContainer.Get(0));
					auto socket = DynamicCast<TcpSocketBase> (app->GetSocket());
            		AsciiTraceHelper asciiTraceHelper;
            		Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream(
                		"UE" + std::to_string(u+1) + ".cwnd");
					socket->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback(&CwndChange, stream));
        	});

		}

		else
			clientApps.Add(dlClientHelperUdp.Install(remoteHost));

        PacketSinkHelper dlPacketSinkHelper(
            (bonus) ? "ns3::TcpSocketFactory" : "ns3::UdpSocketFactory",
            InetSocketAddress(Ipv4Address::GetAny(), dlPort)
		);

		serverApps.Add(dlPacketSinkHelper.Install(ue));
        
		EpcTft::PacketFilter dlpf;
		// Defines a range of port numbers to be filtered: [dlPort, dlPort]
        dlpf.localPortStart = dlPort;
        dlpf.localPortEnd = dlPort;
        tft->Add(dlpf);

		EpsBearer bearer(EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
        lteHelper->ActivateDedicatedEpsBearer(ueLteDevs.Get(u), bearer, tft);

        Time startTime = Seconds(startTimeSeconds->GetValue());
        serverApps.Start(startTime);
		clientApps.Start(startTime);
		clientApps.Stop(simTime);

    }

	// Add X2 interface
    lteHelper->AddX2Interface(enbNodes);

	//lteHelper->EnableLogComponents();
	//LogComponentEnable("LteEnbRrc", LOG_LEVEL_INFO);
	//LogComponentEnable("LteUeRrc", LOG_LEVEL_INFO);
	//LogComponentEnable("BulkSendApplication", LOG_LEVEL_ALL);
	
	lteHelper->EnablePhyTraces();
    lteHelper->EnableMacTraces();
    lteHelper->EnableRlcTraces();
    lteHelper->EnablePdcpTraces();
    Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats();
    rlcStats->SetAttribute("EpochDuration", TimeValue(Seconds(0.05)));
    Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats();
    pdcpStats->SetAttribute("EpochDuration", TimeValue(Seconds(0.05)));

    //connect custom trace sinks for RRC connection establishment and handover notification
    // Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
    //                 MakeCallback(&NotifyConnectionEstablishedEnb));
    // Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
    //                 MakeCallback(&NotifyConnectionEstablishedUe));
    // Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
    //                 MakeCallback(&NotifyHandoverStartEnb));
    // Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
    //                 MakeCallback(&NotifyHandoverStartUe));
    // Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
    //                 MakeCallback(&NotifyHandoverEndOkEnb));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                    MakeCallback(&NotifyHandoverEndOkUe));

    // Hook a trace sink (the same one) to the four handover failure traces
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureNoPreamble",
                    MakeCallback(&NotifyHandoverFailure));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureMaxRach",
                    MakeCallback(&NotifyHandoverFailure));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureLeaving",
                     MakeCallback(&NotifyHandoverFailure));
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverFailureJoining",
                     MakeCallback(&NotifyHandoverFailure));

	

	Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper>();

	

if (plot_rem) {

	int n_channels = ChannelList::GetNChannels();
	int downlink_channel {};
	for(int i {0}; i < n_channels; ++i)
		if(ChannelList::GetChannel(i) == lteHelper->GetDownlinkSpectrumChannel())
			downlink_channel = i;
	std::string s = std::string("/ChannelList/") + std::to_string(downlink_channel);

    remHelper->SetAttribute("ChannelPath", StringValue(s.c_str()));
    remHelper->SetAttribute("OutputFile", StringValue("rem2.out"));
    remHelper->SetAttribute("XMin", DoubleValue(-250.0));
    remHelper->SetAttribute("XMax", DoubleValue(1000.0));
    remHelper->SetAttribute("YMin", DoubleValue(-350.0));
    remHelper->SetAttribute("YMax", DoubleValue(100.0));
    remHelper->SetAttribute("Z", DoubleValue(0.0));
    remHelper->Install();

}


	FlowMonitorHelper flowmon;
  	Ptr<FlowMonitor> Monitor;
  	Monitor = flowmon.Install(ueNodes);
  	Monitor = flowmon.Install(remoteHostContainer);


	Simulator::Stop(simTime);


	Simulator::Run();

	printStats(flowmon,true);	

	Simulator::Destroy();


	for(int i = 0; i < int(ueHandoverCount.size()); ++i)
		std::cout << "UE IMSI " << i+1 << " Handovers: " << ueHandoverCount[i] << "\n";

if (!plot_rem) {
	auto most_handover_ue = std::max_element(ueHandoverCount.begin(), ueHandoverCount.end());
	int idx {int(std::distance(ueHandoverCount.begin(), most_handover_ue))};
	
	std::ofstream max_handover_IMSI {"scratch/asg2/max_handover_IMSI"};
	if(ue_to_track == -1) {
		max_handover_IMSI << idx+1 << " " << ueHandoverCount[idx] << " " << RngSeedManager::GetRun() << " " << ue_mobility_speed;
	}
	else 
		max_handover_IMSI << ue_to_track << " " << ueHandoverCount[ue_to_track-1] << " " << RngSeedManager::GetRun() << " " << ue_mobility_speed;

	if(!agg_tput_collect && !bonus){
		if(handover_algo_type == std::string("ns3::A3RsrpHandoverAlgorithm")) {
		
			std::ofstream hyst_report;
			hyst_report.open("scratch/asg2/hyst_report", std::ios_base::app);
			std::cout << "Appending to file 'hyst_report'...\n";

			hyst_report << RngSeedManager::GetRun() << " " << ue_mobility_speed << " " 
						<< hysteresis << " "
						<< std::accumulate(ueHandoverCount.begin(), ueHandoverCount.end(), 0) << "\n";
		

		}
		else if(handover_algo_type == std::string("ns3::A2A4RsrqHandoverAlgorithm")) {

			std::ofstream nb_offset_report;
			nb_offset_report.open("scratch/asg2/nb_offset_report", std::ios_base::app);
			std::cout << "Appending to file 'nb_offset_report'...\n";

			nb_offset_report << RngSeedManager::GetRun() << " " << ue_mobility_speed << " " 
						<< neighbour_cell_offset << " "
						<< std::accumulate(ueHandoverCount.begin(), ueHandoverCount.end(), 0) << "\n";
		}
	}
	if(agg_tput_collect) {
		if(handover_algo_type == std::string("ns3::NoOpHandoverAlgorithm")) {
			std::ofstream no_op {"scratch/asg2/NoOp", std::ios_base::app};
			no_op << RngSeedManager::GetRun() << " " << ue_mobility_speed << " " << agg_tput << "\n";
			std::cout << "Appending to file 'NoOp' \n";

		} else if(handover_algo_type == std::string("ns3::A3RsrpHandoverAlgorithm")) {
			std::ofstream a3_rsrp {"scratch/asg2/A3Rsrp", std::ios_base::app};
			a3_rsrp << RngSeedManager::GetRun() << " " << ue_mobility_speed << " " << agg_tput << "\n";
			std::cout << "Appending to file 'A3Rsrp' \n";

		} else if(handover_algo_type == std::string("ns3::A2A4RsrqHandoverAlgorithm")) {
			std::ofstream a2_a4_rsrq {"scratch/asg2/A2A4Rsrq", std::ios_base::app};
			a2_a4_rsrq << RngSeedManager::GetRun() << " " << ue_mobility_speed << " " << agg_tput << "\n";
			std::cout << "Appending to file 'A2A4Rsrq' \n";
		}
	}
}
	return 0;
}
