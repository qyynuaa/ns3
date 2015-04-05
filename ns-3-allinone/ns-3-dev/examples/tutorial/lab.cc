/* this file is lab2 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "visualizer-trace-helper.h"

using namespace ns3; 

void printTime(int interval) {
std::cout << Simulator::Now() << std::endl;
Simulator::Schedule(Seconds(interval), &printTime, interval);
}

int main(int args, char *argv[]) {
// Allows configuration from command line
uint32_t rows =4, cols = 4, nodeSpacing = 90, duration = 900, seed = 1;
std::string phyMode("DsssRate11Mbps");
CommandLine cmd;
cmd.AddValue (”phymode", ”Physical transmission mode", phyMode);
cmd.AddValue ("rows", "Rows of nodes", rows);
cmd.AddValue ("cols", "Columns of nodes", cols);
cmd.AddValue ("spacing", "Spacing between neighbouring nodes", nodeSpacing);
cmd.AddValue ("duration", "Duration of simulation", duration);
cmd.AddValue (”seed", ”Random seed for simulation", seed);
cmd.Parse (argc,argv);
uint32_t numNodes = rows * cols;
//Set Attribute Values and Random Seed
Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));
Config::SetDefault("ns3::ConstantRateWifiManager::DataMode", StringValue(phyMode));
Config::SetDefault("ns3::ConstantRateWifiManager::ControlMode", StringValue(phyMode));
Config::SetDefault("ns3::YansWifiPhy::RxGain", DoubleValue(-10));
Config::SetDefault("ns3::YansWifiPhy::TxGain", DoubleValue(1));
// Set seed for pseudorandom number generator
SeedManager::SetSeed (seed);
// Create nodes
NodeContainer nodes;
nodes.Create (numNodes);
//Ptr<Node> first = nodes.Get(0);
// Physical Layer
YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
wifiPhy.Set ("RxGain", DoubleValue (-10) );
wifiPhy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
// Channel
YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
wifiPhy.Set ("RxGain", DoubleValue (-10) );
wifiPhy.SetPcapFormat(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
YansWifiChannelHelper wifiChannel ;
wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
wifiPhy.SetChannel (wifiChannel.Create ());
// MAC layer
NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue(phyMode));
wifiMac.SetType ("ns3::AdhocWifiMac");

WifiHelper wifi;
wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);
// Enable Tracing
wifiPhy.EnablePcap ("MANET", devices);
// Stack, routing and addresses
OlsrHelper olsr;
InternetStackHelper internet;
internet.SetRoutingHelper (olsr);
internet.Install (nodes);
// Assign network addresses
Ipv4AddressHelper address;
address.SetBase ("10.0.0.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces = address.Assign (devices);
// Configure and Install Applications

// Server/Receiver
UdpServerHelper server (4000);
ApplicationContainer apps = server.Install (nodes.Get(1));
apps.Start (Seconds (1.0));
apps.Stop (Seconds (10.0));
// Client/Sender
UdpClientHelper client (interfaces.GetAddress (1), 4000);
client.SetAttribute ("MaxPackets", UintegerValue (320));
client.SetAttribute ("Interval", TimeValue (Seconds(0.05)));
client.SetAttribute ("PacketSize", UintegerValue (1024));
apps = client.Install (nodes.Get (0));
apps.Start (Seconds (2.0));
apps.Stop (Seconds (9.0));
//
Ptr<Application> app = node->GetApplication(0);
Ptr<NetDevice> nic = node->GetDevice(0);

Ptr<Ipv4> ip = nodes.Get(0)->GetObject<Ipv4>();
Ipv4Address addr = ip->GetAddress(1,0).GetLocal();
//Instantiate mobility helper
MobilityHelper mobility;

//Set up an object for position
ObjectFactory pos;

//Type = Rectangle
pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");

//Setup postion of X and Y
pos.Set ("X", RandomVariableValue (UniformVariable (0.0, 1000)));
pos.Set ("Y", RandomVariableValue (UniformVariable (0.0, 1000)));

//get the random position..
Ptr<PositionAllocator> taPositionAlloc = pos.Create()->GetObject<PositionAllocator> ();

//Set up the mobility model..
mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel", "Speed", RandomVariableValue (ConstantVariable (200)), "Pause", RandomVariableValue
(ConstantVariable (0)), "PositionAllocator", PointerValue (taPositionAlloc));

//set the random positions
mobility.SetPositionAllocator (taPositionAlloc);

//Install
mobility.Install (nodes);

//Connecting Trace Sources and Sinks

Simulator::Schedule(Seconds(1), &printTime, 1);
Simulator::Stop (Seconds (duration));
VisualizerTraceHelper traceHelper(duration*1000, nodes);
traceHelper.StartWritingFile("tracefile.txt");
Simulator::Run ();
traceHelper.EndWritingFile();
Simulator::Destroy ();
return 0;

} // End of main()
