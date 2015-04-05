/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA
 *
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

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include <cmath>
#include "ns3/simulator.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/test.h"
#include "ns3/config.h"
#include "ns3/steady-state-random-waypoint-mobility-model.h"
#include "ns3/rng-seed-manager.h"

using namespace ns3;

void DistribCompare (std::vector<Ptr<MobilityModel> > mobilityStack)
{
  double velocity;
  double sum_x = 0;
  double sum_y = 0;
  double sum_v = 0;
  double count=200;

  std::vector<Ptr<MobilityModel> >::iterator i;
  Ptr<MobilityModel> model;
  for (i = mobilityStack.begin (); i != mobilityStack.end (); ++i)
    {
      model = (*i);
      velocity = std::sqrt (std::pow (model->GetVelocity ().x, 2) + std::pow (model->GetVelocity ().y, 2));
      sum_x += model->GetPosition ().x;
      sum_y += model->GetPosition ().y;
      sum_v += velocity;
    }
  double mean_x = sum_x / count;
  double mean_y = sum_y / count;
  double mean_v = sum_v / count;
  std::cout << Simulator::Now ().GetSeconds () <<"\t" << mean_v <<"\t" << mean_x<<"\t" << mean_y<< std::endl;
}
/*
static void CourseChange (std::string foo, Ptr<const MobilityModel> mobility)
{
  Vector pos = mobility->GetPosition ();
  Vector vel = mobility->GetVelocity ();
  std::cout << Simulator::Now ().GetSeconds () <<"\t" << mobility <<"\t" << pos.x <<"\t" << pos.y <<"\t" << vel.x <<"\t" << vel.y << std::endl;
}
*/
int main (int argc, char *argv[])
{
  /*

  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Mode", StringValue ("Time"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Time", StringValue ("2s"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Bounds", StringValue ("0|200|0|200"));

  CommandLine cmd;
  cmd.Parse (argc, argv);

  NodeContainer c;
  c.Create (100);

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Mode", StringValue ("Time"),
                               "Time", StringValue ("2s"),
                               "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                               "Bounds", StringValue ("0|200|0|200"));
  mobility.InstallAll ();
  *///Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
      //               MakeCallback (&CourseChange));

  SeedManager::SetSeed (123);
  std::vector<Ptr<MobilityModel> > mobilityStack;
  // Total simulation time, seconds
  double totalTime = 1000;

  ObjectFactory mobilityFactory;
  mobilityFactory.SetTypeId ("ns3::SteadyStateRandomWaypointMobilityModel");
  mobilityFactory.Set ("MinSpeed", DoubleValue (0.5));
  mobilityFactory.Set ("MaxSpeed", DoubleValue (2.0));
  mobilityFactory.Set ("MinPause", DoubleValue (0.0));
  mobilityFactory.Set ("MaxPause", DoubleValue (0.0));
  mobilityFactory.Set ("MinX", DoubleValue (0));
  mobilityFactory.Set ("MaxX", DoubleValue (1000));
  mobilityFactory.Set ("MinY", DoubleValue (0));
  mobilityFactory.Set ("MaxY", DoubleValue (1000));

  // Populate the vector of mobility models.
  double count = 200;
  for (uint32_t i = 0; i < count; i++)
    {
        // Create a new mobility model.
        Ptr<MobilityModel> model = mobilityFactory.Create ()->GetObject<MobilityModel> ();

        // Add this mobility model to the stack.
        mobilityStack.push_back (model);
        Simulator::Schedule (Seconds (0.0), &Object::Initialize, model);
    }
  for (uint32_t j = 0; j < totalTime; j++)
  {
	  Simulator::Schedule (Seconds (j), &DistribCompare,mobilityStack);
	  //Simulator::Schedule (Seconds (totalTime), &DistribCompare,mobilityStack);
  }

  Simulator::Stop (Seconds (totalTime));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
