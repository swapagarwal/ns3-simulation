#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/gnuplot.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Assignment_4");

class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void ChangeRate(DataRate newrate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);
	void SendPacket2 (int);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::SendPacket2 (int m_bufSize)
{
  uint8_t* buffer = new uint8_t[m_bufSize];
  Ptr<Packet> packet = Create<Packet> (buffer, m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

void
MyApp::ChangeRate(DataRate newrate)
{
   m_dataRate = newrate;
   return;
}

/*static void
CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
{
  std::cout << Simulator::Now ().GetSeconds () << "\t" << newCwnd <<"\n";
}*/

void
IncRate (Ptr<MyApp> app, DataRate rate)
{
	app->ChangeRate(rate);
    return;
}

//       n0 ---+      +--- n5
//             |      |
//       n1 ---n3 -- n4--- n6
//             |      |
//       n2 ---+      +--- n7

int main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
	Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));
	//LogComponentEnable ("TcpL4Protocol", LOG_LEVEL_ALL);
	//LogComponentEnable ("PacketSink", LOG_LEVEL_ALL);
	//int bufSize = 10*1500;
	
	//Initialize Plot
	
	std :: string fileNameWithNoExtension = "plot-2d";
	std :: string graphicsFileName        = fileNameWithNoExtension + ".png";
	std :: string plotFileName            = fileNameWithNoExtension + ".plt";
	std :: string plotTitle               = "2-D Plot";
	std :: string dataTitle               = "2-D Data";

	// Instantiate the plot and set its title.
	Gnuplot plot (graphicsFileName);
	plot.SetTitle (plotTitle);

	// Make the graphics file, which the plot file will create when it
	// is used with Gnuplot, be a PNG file.
	plot.SetTerminal ("png");

	// Set the labels for each axis.
	plot.SetLegend ("X Values", "Y Values");

	// Set the range for the x axis.
	plot.AppendExtra ("set xrange [0:800]");

	// Instantiate the dataset, set its title, and make the points be
	// plotted along with connecting lines.
	Gnuplot2dDataset dataset;
	dataset.SetTitle (dataTitle);
	dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);
	
	// Initialization Complete
	
for(int bufSize=10*1500;bufSize<=800*1500;)
{ 
  NS_LOG_INFO ("For Visualization!");
  CommandLine cmd;
  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create Nodes.");
  NodeContainer nodes;
  nodes.Create (8);

  NodeContainer n0n3 = NodeContainer (nodes.Get (0), nodes.Get (3)); //h1r1
  NodeContainer n1n3 = NodeContainer (nodes.Get (1), nodes.Get (3)); //h2r1
  NodeContainer n2n3 = NodeContainer (nodes.Get (2), nodes.Get (3)); //h3r1
  NodeContainer n3n4 = NodeContainer (nodes.Get (3), nodes.Get (4)); //r1r2
  NodeContainer n4n5 = NodeContainer (nodes.Get (4), nodes.Get (5)); //r2h4
  NodeContainer n4n6 = NodeContainer (nodes.Get (4), nodes.Get (6)); //r2h5
  NodeContainer n4n7 = NodeContainer (nodes.Get (4), nodes.Get (7)); //r2h6

  InternetStackHelper internet;
  internet.Install (nodes);

  NS_LOG_INFO ("Create Channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
  NetDeviceContainer d0d3 = p2p.Install (n0n3);
  NetDeviceContainer d1d3 = p2p.Install (n1n3);
  NetDeviceContainer d2d3 = p2p.Install (n2n3);
  NetDeviceContainer d4d5 = p2p.Install (n4n5);
  NetDeviceContainer d4d6 = p2p.Install (n4n6);
  NetDeviceContainer d4d7 = p2p.Install (n4n7);
	p2p.SetQueue("ns3::DropTailQueue","Mode",EnumValue (DropTailQueue::QUEUE_MODE_BYTES),"MaxBytes",UintegerValue (125000));
  p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("100ms"));
  NetDeviceContainer d3d4 = p2p.Install (n3n4);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i3 = ipv4.Assign (d0d3);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i3 = ipv4.Assign (d1d3);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i4 = ipv4.Assign (d3d4);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i5 = ipv4.Assign (d4d5);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i6 = ipv4.Assign (d4d6);

  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i7 = ipv4.Assign (d4d7);

  NS_LOG_INFO ("Enable static global routing.");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //tcp - h1 to h6
  uint16_t sinkPort1 = 8081;
  Address sinkAddress1 (InetSocketAddress (i4i7.GetAddress (1), sinkPort1));
  PacketSinkHelper packetSinkHelper1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort1));
  ApplicationContainer sinkApps1 = packetSinkHelper1.Install (nodes.Get (7));
  sinkApps1.Start (Seconds (0.));
  sinkApps1.Stop (Seconds (100.));

  Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	//ns3TcpSocket1->SetAttribute("SegmentSize",  ns3::UintegerValue(800));
	ns3TcpSocket1->SetAttribute("SndBufSize",  ns3::UintegerValue(bufSize));
	ns3TcpSocket1->SetAttribute("RcvBufSize",  ns3::UintegerValue(bufSize));
	//ns3TcpSocket1->SetRcvBufSize(10);
  //ns3TcpSocket1->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));

  Ptr<MyApp> app1 = CreateObject<MyApp> ();
  app1->Setup (ns3TcpSocket1, sinkAddress1, 1040, 1000000, DataRate ("20Mbps"));
  nodes.Get (0)->AddApplication (app1);
  app1->SetStartTime (Seconds (1.));
  app1->SetStopTime (Seconds (100.));

  //tcp - h4 to h3
  uint16_t sinkPort2 = 8082;
  Address sinkAddress2 (InetSocketAddress (i2i3.GetAddress (0), sinkPort2));
  PacketSinkHelper packetSinkHelper2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort2));
  ApplicationContainer sinkApps2 = packetSinkHelper2.Install (nodes.Get (2));
  sinkApps2.Start (Seconds (0.));
  sinkApps2.Stop (Seconds (100.));

  Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (nodes.Get (5), TcpSocketFactory::GetTypeId ());
	ns3TcpSocket2->SetAttribute("SndBufSize",  ns3::UintegerValue(bufSize));
	ns3TcpSocket2->SetAttribute("RcvBufSize",  ns3::UintegerValue(bufSize));

  //ns3TcpSocket2->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));

  Ptr<MyApp> app2 = CreateObject<MyApp> ();
  app2->Setup (ns3TcpSocket2, sinkAddress2, 1040, 1000000, DataRate ("20Mbps"));
  nodes.Get (5)->AddApplication (app2);
  app2->SetStartTime (Seconds (1.));
  app2->SetStopTime (Seconds (100.));

  //tcp - h1 to h2
  uint16_t sinkPort3 = 8083;
  Address sinkAddress3 (InetSocketAddress (i1i3.GetAddress (0), sinkPort3));
  PacketSinkHelper packetSinkHelper3("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort3));
  ApplicationContainer sinkApps3 = packetSinkHelper3.Install (nodes.Get (1));
  sinkApps3.Start (Seconds (0.));
  sinkApps3.Stop (Seconds (100.));

  Ptr<Socket> ns3TcpSocket3 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	ns3TcpSocket3->SetAttribute("SndBufSize",  ns3::UintegerValue(bufSize));
	ns3TcpSocket3->SetAttribute("RcvBufSize",  ns3::UintegerValue(bufSize));

  //ns3TcpSocket3->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));

  Ptr<MyApp> app3 = CreateObject<MyApp> ();
  app3->Setup (ns3TcpSocket3, sinkAddress3, 1040, 1000000, DataRate ("20Mbps"));
  nodes.Get (0)->AddApplication (app3);
  app3->SetStartTime (Seconds (1.));
  app3->SetStopTime (Seconds (100.));

  //tcp - h5 to h6
  uint16_t sinkPort4 = 8084;
  Address sinkAddress4 (InetSocketAddress (i4i7.GetAddress (1), sinkPort4));
  PacketSinkHelper packetSinkHelper4("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort4));
  ApplicationContainer sinkApps4 = packetSinkHelper4.Install (nodes.Get (7));
  sinkApps4.Start (Seconds (0.));
  sinkApps4.Stop (Seconds (100.));

  Ptr<Socket> ns3TcpSocket4 = Socket::CreateSocket (nodes.Get (6), TcpSocketFactory::GetTypeId ());
	ns3TcpSocket4->SetAttribute("SndBufSize",  ns3::UintegerValue(bufSize));
	ns3TcpSocket4->SetAttribute("RcvBufSize",  ns3::UintegerValue(bufSize));

  //ns3TcpSocket4->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));

  Ptr<MyApp> app4 = CreateObject<MyApp> ();
  app4->Setup (ns3TcpSocket4, sinkAddress4, 1040, 1000000, DataRate ("20Mbps"));
  nodes.Get (6)->AddApplication (app4);
  app4->SetStartTime (Seconds (1.));
  app4->SetStopTime (Seconds (100.));

  //udp - h2 to h3
  uint16_t sinkPort5 = 8085;
  Address sinkAddress5 (InetSocketAddress (i2i3.GetAddress (0), sinkPort5));
  PacketSinkHelper packetSinkHelper5 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort5));
  ApplicationContainer sinkApps5 = packetSinkHelper5.Install (nodes.Get (2));
  sinkApps5.Start (Seconds (0.));
  sinkApps5.Stop (Seconds (100.));

  Ptr<Socket> ns3UdpSocket5 = Socket::CreateSocket (nodes.Get (1), UdpSocketFactory::GetTypeId ());
	//ns3UdpSocket5->SetAttribute("SndBufSize",  ns3::UintegerValue(bufSize));
	ns3UdpSocket5->SetAttribute("RcvBufSize",  ns3::UintegerValue(bufSize));
  //ns3UdpSocket5->SetAttribute("SndBufSize",  ns3::UintegerValue(800));

  Ptr<MyApp> app5 = CreateObject<MyApp> ();
  app5->Setup (ns3UdpSocket5, sinkAddress5, 1040, 1000000, DataRate ("20Mbps"));
  nodes.Get (1)->AddApplication (app5);
  app5->SetStartTime (Seconds (1.));
  app5->SetStopTime (Seconds (100.));

  Simulator::Schedule (Seconds(10.0), &IncRate, app5, DataRate("100Mbps"));

  //udp - h4 to h5
  uint16_t sinkPort6 = 8086;
  Address sinkAddress6 (InetSocketAddress (i4i6.GetAddress (1), sinkPort6));
  PacketSinkHelper packetSinkHelper6 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort6));
  ApplicationContainer sinkApps6 = packetSinkHelper6.Install (nodes.Get (6));
  sinkApps6.Start (Seconds (0.));
  sinkApps6.Stop (Seconds (100.));

  Ptr<Socket> ns3UdpSocket6 = Socket::CreateSocket (nodes.Get (5), UdpSocketFactory::GetTypeId ());
	//ns3UdpSocket6->SetAttribute("SndBufSize",  ns3::UintegerValue(bufSize));
	ns3UdpSocket6->SetAttribute("RcvBufSize",  ns3::UintegerValue(bufSize));

  Ptr<MyApp> app6 = CreateObject<MyApp> ();
  app6->Setup (ns3UdpSocket6, sinkAddress6, 1040, 1000000, DataRate ("20Mbps"));
  nodes.Get (5)->AddApplication (app6);
  app6->SetStartTime (Seconds (1.));
  app6->SetStopTime (Seconds (100.));

  //Simulator::Schedule (Seconds(10.0), &IncRate, app6, DataRate("100Mbps"));

//
// Calculate Throughput using Flowmonitor
//
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds(15.0));
  Simulator::Run ();

	monitor->CheckForLostPackets ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  double Sumx = 0, SumSqx = 0;
  int n = 0;
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	  //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      
	  //std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ") - ";
	  //std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
	  //std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
	  //std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
	  
	  double TPut = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024;
      Sumx += TPut;
	  SumSqx += TPut * TPut ;
	  n++;
     }
	double FairnessIndex = (Sumx * Sumx)/ (6 * SumSqx) ;
	dataset.Add (bufSize/1500, FairnessIndex);
	std :: cout << " FairnessIndex:	" << FairnessIndex << std :: endl;


  monitor->SerializeToXmlFile("lab-1.flowmon", true, true);

  Simulator::Destroy ();
  if(bufSize < 100*1500) bufSize+=10*1500;
  else bufSize+=100*1500;
}

	// Add the dataset to the plot.
	plot.AddDataset (dataset);

	// Open the plot file.
	std :: ofstream plotFile (plotFileName.c_str());

	// Write the plot file.
	plot.GenerateOutput (plotFile);

	// Close the plot file.
	plotFile.close ();

  NS_LOG_INFO ("Done.");
}