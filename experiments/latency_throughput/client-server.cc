/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FifthScriptExample");

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                5 Mbps, 2 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off 
// application is not created until Application Start time, so we wouldn't be 
// able to hook the socket (now) at configuration time.  Second, even if we 
// could arrange a call after start time, the socket is not public so we 
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass 
// this socket into the constructor of our simple application which we then 
// install in the source node.
// ===========================================================================
//

/*
class MyApp : public Application 
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

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
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      //Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      Time tNext (MicroSeconds(20));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}
*/

Time last_time(Ptr<FlowMonitor> fm) {
  Time t = Time::Min();
  for(auto e : fm->GetFlowStats())
    t = std::max(t, e.second.timeLastRxPacket);
  return t;
}    

//std::vector<Time> delays_container;
//  std::vector<double> throughputs_container;

void latency_throughput_plot(Ptr<FlowMonitor> fm, std::string name) {

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream ("data/tcp/first_flow_" + name +".dat");
  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream ("data/tcp/second_flow_" + name +".dat");
  
  
  //GnuplotHelper plotHelper;
  //plotHelper.ConfigurePlot ("Latency Throughput TCP",
  //                          "Latency Throughput - TCP",
  //                          "Throughput (Kbps)",
  //                          "Latency (ns)");

  


  
  int i = 0;

  for(auto e : fm->GetFlowStats())
  {
      if(i)
      {
        for(uint32_t i = 0; i< e.second.delays_container.size(); i++)
        {
          *stream1->GetStream ()<< e.second.delays_container[i].GetNanoSeconds () << "\t" <<
                       e.second.throughputs_container[i] << std::endl;
        }
      }
      else
      {
        for(uint32_t i = 0; i< e.second.delays_container.size(); i++)
          {
            *stream2->GetStream () << e.second.delays_container[i].GetNanoSeconds () << "\t" <<
                         e.second.throughputs_container[i] << std::endl;
          }
      }
      i++;
  //    t = std::max(t, e.second.timeLastRxPacket); 
  }
  
  std::cout << i << std::endl;
}
/*static void
CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
}

static void
RxDrop (Ptr<const Packet> p)
{
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
}
*/

int 
main (int argc, char *argv[])
{
  
  
  //LogComponentEnable ("TcpClient", LOG_LEVEL_INFO);
  //LogComponentEnable ("TcpServer", LOG_LEVEL_INFO);
  //Time::SetResolution (Time::NS);
  
  bool tracing = true;
  Time serverInterval = MicroSeconds (20);
  Time clientInterval = MicroSeconds (50);
  std::string delay = "0ms";
  uint32_t clientMaximumPacketSize = 400; 
  double percentile = 99.9;
  double err = 0.0;
  std::string linkBandWidth = "1Mbps";


  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
  cmd.AddValue ("serverInterval",
                "service Time", serverInterval);
  cmd.AddValue("clientRate", "client rate", clientInterval);
  cmd.AddValue("delay", " ", delay);
  cmd.AddValue("err", " ", err);
  cmd.AddValue("percentile", "enter the percentile", percentile);
  cmd.AddValue("maxPacket", "maximum packet size of the client", clientMaximumPacketSize);
  cmd.AddValue("linkbandwidth", " ", linkBandWidth);
  
  cmd.Parse (argc, argv);
  
  
  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (linkBandWidth));
  pointToPoint.SetChannelAttribute ("Delay", StringValue (delay));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (err));
  devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  /*
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (interfaces.GetAddress (1), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (1));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (20.));

  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
  //ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3TcpSocket, sinkAddress, 1040, 10000, DataRate ("1Mbps"));
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (20.));
  */

  //Server Creation Started
  uint16_t server_port = 5000;
  Ptr<TcpServer> server = CreateObject<TcpServer> ();
  server->Setup(interfaces.GetAddress(0), server_port, 400, serverInterval);
  nodes.Get (0)->AddApplication(server);
  server->SetStartTime (Seconds (0.0));
  server->SetStopTime (Seconds (5.0));
  //Server Creation Ended
  
  //Client Creation Started
  Ptr<TcpClient> client = CreateObject<TcpClient> ();
  Address peerAddress (interfaces.GetAddress (0));
  client->SetRemote(peerAddress, server_port);
  client->SetInterval(clientInterval);
  client->SetMaximumPacketSize(clientMaximumPacketSize);
  nodes.Get (1)->AddApplication(client);
  client->SetStartTime (Seconds (1.0));
  client->SetStopTime (Seconds (5.0));
  
  //Client Creation Ended

  //devices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&RxDrop));

  // Flow monitor
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  if(tracing)
    flowMonitor = flowHelper.InstallAll();
  

  Simulator::Stop (Seconds (6.0));
  Simulator::Run ();
  
  if(tracing)
  {
    std::cout << "Now " << last_time(flowMonitor).As(Time::S) << std::endl;
    latency_throughput_plot(flowMonitor, linkBandWidth);
    flowMonitor->SerializeToXmlFile("data/client-server-" + linkBandWidth + ".xml", false, false);
  }
  
  Simulator::Destroy ();

  return 0;
}

