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
  sum_x = 0;
  sum_y = 0;
  sum_v = 0;
  double tmp;
  for (i = mobilityStack.begin (); i != mobilityStack.end (); ++i)
      {
        model = (*i);
        velocity = std::sqrt (std::pow (model->GetVelocity ().x, 2) + std::pow (model->GetVelocity ().y, 2));
        tmp = model->GetPosition ().x - mean_x;
        sum_x += tmp * tmp;
        tmp = model->GetPosition ().y - mean_y;
        sum_y += tmp * tmp;
        tmp = velocity - mean_v;
        sum_v += tmp * tmp;
      }
  //double dev_x = std::sqrt (sum_x / (count - 1));
  //double dev_y = std::sqrt (sum_y / (count - 1));
  //double dev_v = std::sqrt (sum_v / (count - 1));
  //std::cout << Simulator::Now ().GetSeconds () <<"\t" << dev_v <<"\t" << dev_x<<"\t" << dev_y<< std::endl;
}


void Plotpos (std::vector<Ptr<MobilityModel> > mobilityStack)
{
  double pos_x = 0;
  double pos_y = 0;

  std::vector<Ptr<MobilityModel> >::iterator i;
  Ptr<MobilityModel> model;
  for (i = mobilityStack.begin (); i != mobilityStack.end (); ++i)
    {
      model = (*i);
      //velocity = std::sqrt (std::pow (model->GetVelocity ().x, 2) + std::pow (model->GetVelocity ().y, 2));
      pos_x= model->GetPosition ().x;
      pos_y= model->GetPosition ().y;
      std::cout << pos_x <<"\t" << pos_y<< std::endl;
      //sum_v += velocity;
    }
}

int main (int argc, char *argv[])
{

  Config::SetDefault ("ns3::RandomWaypointMobilityModel::Speed", StringValue ("ns3::UniformRandomVariable[Min=0.5|Max=2.0]"));
  Config::SetDefault ("ns3::RandomWaypointMobilityModel::Pause", StringValue ("ns3::ConstantRandomVariable[Constant=2.0]"));

  double count = 200;
  CommandLine cmd;
  cmd.Parse (argc, argv);

  NodeContainer nodes;
  nodes.Create (count);
  ObjectFactory pos;
  Ptr<PositionAllocator> taPositionAlloc;
  MobilityHelper mobility;
  pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
  pos.Set("X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  pos.Set("Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  taPositionAlloc = pos.Create()->GetObject<PositionAllocator>();

  mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
                             "Speed", StringValue ("ns3::UniformRandomVariable[Min=0.5|Max=2.0]"),
							 "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0]"),
							 "PositionAllocator", PointerValue (taPositionAlloc));
  mobility.SetPositionAllocator(taPositionAlloc);
  mobility.InstallAll ();

  SeedManager::SetSeed (12);
  std::vector<Ptr<MobilityModel> > mobilityStack;
  // Total simulation time, seconds
  double totalTime = 10000;
  // Populate the vector of mobility models.

  for (uint32_t i = 0; i < count; i++)
  {
    // Create a new mobility model.
	Ptr<MobilityModel> model = nodes.Get(i)->GetObject<MobilityModel> ();
    //Ptr<MobilityModel> model = mobility.Create ()->GetObject<MobilityModel> ();

    // Add this mobility model to the stack.
    mobilityStack.push_back (model);
    Simulator::Schedule (Seconds (0.0), &Object::Initialize, model);
  }
  for (uint32_t j = 0; j < totalTime; j++)
  {
	  Simulator::Schedule (Seconds (j), &DistribCompare,mobilityStack);

  }
  //Simulator::Schedule (Seconds (0), &Plotpos,mobilityStack);
  Simulator::Schedule (Seconds (totalTime), &Plotpos,mobilityStack);
  Simulator::Stop (Seconds (totalTime));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
