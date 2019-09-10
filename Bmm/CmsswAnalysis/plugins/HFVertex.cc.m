#include <memory>
#include "FWCore/Framework/interface/MakerMacros.h"
#include "HFVertex.h"
#include "Bmm/CmsswAnalysis/plugins/HFVirtualDecay.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "Bmm/CmsswAnalysis/interface/HFSequentialVertexFit.hh"
#include "Bmm/RootAnalysis/common/HFMasses.hh"
#include "Bmm/CmsswAnalysis/interface/HFDumpUtilities.hh"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "Bmm/RootAnalysis/rootio/TAna01Event.hh"

#include "RecoVertex/KinematicFit/interface/KinematicParticleFitter.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/KinematicConstrainedVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/MassKinematicConstraint.h"
#include "RecoVertex/KinematicFit/interface/MultiTrackMassKinematicConstraint.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"
#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/TrackReco/interface/fillCovariance.h"

#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/IPTools/interface/IPTools.h"

#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"

#include "CommonTools/Statistics/interface/ChiSquared.h"

enum { dimension = 5 };
typedef math::Error<dimension>::type CovarianceMatrix;

using namespace std;
using namespace edm;
using namespace reco;


HFVertex::HFVertex(const edm::ParameterSet & iConfig):
  HFVirtualDecay(iConfig),
  fRefType(iConfig.getUntrackedParameter<int>("refType", 3000068)) {
  dumpConfiguration();
}

void HFVertex::dumpConfiguration() {
  cout << "----------------------------------------------------------------------" << endl;
  cout << "--- HFVertex constructor" << endl;
  HFVirtualDecay::dumpConfiguration();
  cout << "---  refType                     " << fRefType << endl;
  cout << "----------------------------------------------------------------------" << endl;
}

void HFVertex::analyze(const Event& iEvent, const EventSetup& iSetup){

if (fVerbose > 0)  cout << "=== HFVertex run = " << iEvent.id().run()
                          << " evt = " << iEvent.id().event()
                          << " ==================================================================="
                          << endl;

 try {
    HFVirtualDecay::analyze(iEvent,iSetup);
  } catch(HFSetupException e) {
    cout << "==>HFVertex::analyze> " << e.fMsg << endl;
    return;
  }

  


}


//define this as a plug-in
DEFINE_FWK_MODULE(HFVertex);
