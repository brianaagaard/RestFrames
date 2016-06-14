/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   MinMassesCombJigsaw.cc
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2015 Jan
///
//   This file is part of RestFrames.
//
//   RestFrames is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   RestFrames is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with RestFrames. If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////

#include "RestFrames/RestFrame.hh"
#include "RestFrames/MinMassesCombJigsaw.hh"

namespace RestFrames {

  ///////////////////////////////////////////////
  // MinMassesCombJigsaw class methods
  ///////////////////////////////////////////////
  MinMassesCombJigsaw::MinMassesCombJigsaw(const std::string& sname, 
					   const std::string& stitle) : 
    CombinatoricJigsaw(sname, stitle, 2, 2) {}

  MinMassesCombJigsaw::MinMassesCombJigsaw() : CombinatoricJigsaw() {}
  
  MinMassesCombJigsaw::~MinMassesCombJigsaw() {}

  void MinMassesCombJigsaw::Clear(){
    CombinatoricJigsaw::Clear();
  }

  MinMassesCombJigsaw& MinMassesCombJigsaw::Empty(){
    return MinMassesCombJigsaw::m_Empty;
  }

  void MinMassesCombJigsaw::AddFrame(const RestFrame& frame, int i){
    if(!frame) return;
    if(!GetGroup()) return;
    
    ConstRestFrameList frames = 
      frame.GetListVisibleFrames()+
      frame.GetListInvisibleFrames();
    int N = frames.GetN();
    for(int f = 0; f < N; f++){
      if(GetGroup().ContainsFrame(frames[f]))
	AddChildFrame(frames[f], i);
      AddDependancyFrame(frames[f], i);
    }
  }

  void MinMassesCombJigsaw::AddFrames(const ConstRestFrameList& frames, int i){
    int N = frames.GetN();
    for(int f = 0; f < N; f++)
      AddFrame(frames[f],i);
  }

  bool MinMassesCombJigsaw::AnalyzeEvent(){
    if(!IsSoundMind() || !GetGroup())
      return SetSpirit(false);

    if(!InitializeCombinatoric()){
      m_Log << LogWarning;
      m_Log << "Problem initializing event info" << LogEnd;
      return SetSpirit(false);
    }

    int Ninput = m_InputStates.GetN();

    bool DO_N3 = (Ninput >= 2) &&
      GetDependancyStates(0).GetN() == 1 &&
      GetDependancyStates(1).GetN() == 1;
    if(DO_N3){
      DO_N3 = (m_NForChild[&GetChildState(0)] <= 1) && 
	(m_NForChild[&GetChildState(1)] <= 1) && 
	!m_NExclusive[&GetChildState(0)] && 
	!m_NExclusive[&GetChildState(1)];
    }
    if(!DO_N3)
      return CombinatoricJigsaw::LoopCombinatoric();

    // DO 4 N^3 calculation
    std::vector<TLorentzVector> inputs;
    for(int i = 0; i < Ninput; i++)
      inputs.push_back(m_InputStates[i].GetFourVector());	
    
    // boost input vectors to CM frame
    TLorentzVector TOT(0.,0.,0.,0.);
    for(int i = 0; i < Ninput; i++) TOT += inputs[i];
    TVector3 boost = TOT.BoostVector();
    for(int i = 0; i < Ninput; i++) inputs[i].Boost(-boost);
    
    int ip_max[2];
    int jp_max[2];
    for(int i = 0; i < 2; i++) ip_max[i] = -1;
    for(int i = 0; i < 2; i++) jp_max[i] = -1;
    double metric_max = -1.;
    // Loop over all 2-jet seed probes
    int ip[2], jp[2];
    for(ip[0] = 0; ip[0] < Ninput-1; ip[0]++){
      for(ip[1] = ip[0]+1; ip[1] < Ninput; ip[1]++){
	TVector3 nRef = inputs[ip[0]].Vect().Cross(inputs[ip[1]].Vect());
	int Nhem[2];
	TLorentzVector hem[2];
	for(int i = 0; i < 2; i++){
	  Nhem[i] = 0;
	  hem[i].SetPxPyPzE(0.,0.,0.,0.);
	}
	// Loop over all jets
	for(int i = 0; i < Ninput; i++){
	  if((i == ip[0]) || (i ==ip[1])) continue;
	  int ihem = int(inputs[i].Vect().Dot(nRef) > 0.);
	  Nhem[ihem]++;
	  hem[ihem] += inputs[i];
	}
	// assign 2 probes
	for(jp[0] = 0; jp[0] < 2; jp[0]++){
	  for(jp[1] = 0; jp[1] < 2; jp[1]++){
	    if(jp[0] == jp[1] && Nhem[!jp[0]] == 0) continue;
	    TLorentzVector hem_probes[2];
	    for(int i = 0; i < 2; i++) hem_probes[i] = hem[i];
	    for(int i = 0; i < 2; i++) hem_probes[jp[i]] += inputs[ip[i]];
	    double metric = hem_probes[0].P() + hem_probes[1].P();
	    if(metric > metric_max){
	      metric_max = metric;
	      for(int i = 0; i < 2; i++) ip_max[i] = ip[i];
	      for(int i = 0; i < 2; i++) jp_max[i] = jp[i];
	    }
	  }
	}
      }
    }
    if(metric_max < 0)
      return false;
    
    // initialize output states
    for(int i = 0; i < 2; i++) GetChildState(i).ClearElements();
    for(int i = 0; i < 2; i++) GetChildState(jp_max[i]).AddElement(m_InputStates[ip_max[i]]);
    TVector3 nRef = inputs[ip_max[0]].Vect().Cross(inputs[ip_max[1]].Vect());
    for(int i = 0; i < Ninput; i++){
      if((i == ip_max[0]) || (i == ip_max[1])) continue;
      int ihem = int(inputs[i].Vect().Dot(nRef) > 0.);
      GetChildState(ihem).AddElement(m_InputStates[i]);
    }
    if(GetChildState(1).GetFourVector().M() > GetChildState(1).GetFourVector().M()){
      std::vector<VisibleStateList> flip;
      for(int i = 0; i < 2; i++) flip.push_back(GetChildState(i).GetElements());
      for(int i = 0; i < 2; i++) GetChildState(i).ClearElements();
      for(int i = 0; i < 2; i++) GetChildState(i).AddElements(flip[!i]);
    }
  
    ExecuteDependancyJigsaws();
  
    return SetSpirit(true);
  }

  double MinMassesCombJigsaw::EvaluateMetric() const {
    TLorentzVector P1 = GetDependancyStates(0).GetFourVector();
    TLorentzVector P2 = GetDependancyStates(1).GetFourVector();
   
    return GetP((P1+P2).M(), P1.M(), P2.M());
  }

  MinMassesCombJigsaw MinMassesCombJigsaw::m_Empty;

}

