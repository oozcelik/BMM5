#ifndef _HFDUMPSTUFF_h_
#define _HFDUMPSTUFF_h_

#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "PhysicsTools/Utilities/interface/LumiReweightingStandAlone.h"

#include "DataFormats/Luminosity/interface/LumiSummary.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"
#include "DataFormats/Common/interface/ConditionsInEdm.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

class TFile;
class TTree;
class TAna01Event;

class TrackAssociatorBase;

// ----------------------------------------------------------------------
class HFDumpStuff : public edm::EDAnalyzer {
 public:
  explicit HFDumpStuff(const edm::ParameterSet&);
  ~HFDumpStuff();
  
 private:
  edm::LumiReWeighting lumi_weights;
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
//  edm::LumiReWeighting lumi_weights;
  int                  fVerbose; 
  std::string	       fCandidates1Label, fCandidates2Label, fCandidates3Label;
  edm::InputTag        fLumiSummaryLabel, fBeamSpotLabel, fPrimaryVertexLabel, fPrimaryVertexTracksLabel, fPileUpInfo; 
//  edm::EDGetTokenT<std::vector<PileupSummaryInfo>>fPileUp;
  std::string pileup_path_;
  edm::EDGetTokenT<LumiSummary> fTokenLumiSummary;
  edm::EDGetTokenT<reco::BeamSpot> fTokenBeamSpot;
  edm::EDGetTokenT<std::vector<reco::Track> > fTokenTrack;
  edm::EDGetTokenT<reco::VertexCollection> fTokenVertex;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo>>fPileUp;
  
};

#endif
