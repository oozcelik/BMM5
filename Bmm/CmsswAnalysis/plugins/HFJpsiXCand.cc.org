#include <memory>
#include "FWCore/Framework/interface/MakerMacros.h"
#include "HFJpsiXCand.h"
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


HFJpsiXCand::HFJpsiXCand(const edm::ParameterSet & iConfig):
  HFVirtualDecay(iConfig),
  fRefType(iConfig.getUntrackedParameter<int>("refType", 3000068)) {
  dumpConfiguration();
}

void HFJpsiXCand::dumpConfiguration() {
  cout << "----------------------------------------------------------------------" << endl;
  cout << "--- HFJpsiXCand constructor" << endl;
  HFVirtualDecay::dumpConfiguration();
  cout << "---  refType                     " << fRefType << endl;
  cout << "----------------------------------------------------------------------" << endl;
}

void HFJpsiXCand::analyze(const Event& iEvent, const EventSetup& iSetup){

if (fVerbose > 0)  cout << "=== HFJpsiXCand run = " << iEvent.id().run()
                          << " evt = " << iEvent.id().event()
                          << " ==================================================================="
                          << endl;

 try {
    HFVirtualDecay::analyze(iEvent,iSetup);
  } catch(HFSetupException e) {
    cout << "==>HFJpsiXCand::analyze> " << e.fMsg << endl;
    return;
  }

 TLorentzVector p4m1, p4m2, p4psi, m1best, m2best;
 double mbest(-999.);
 int im1(-1), im2(-1);
 vector<int> midx = fListBuilder->getMuonList();
 cout << "==>HFJpsiXCand> #muons = " << midx.size() << endl;
 if (midx.size() < 2) return;

 for (unsigned int im = 0; im < midx.size(); ++im) {
   for (unsigned int in = im+1; in < midx.size(); ++in) {
     reco::TrackBaseRef m1(fTracksHandle, midx[im]);
     reco::Track t1(*m1);
     reco::TrackBaseRef m2(fTracksHandle, midx[in]);
     reco::Track t2(*m2);
     if (t1.charge()*t2.charge() > 0) continue;
     p4m1.SetXYZM(t1.px(), t1.py(), t1.pz(), MMUON);
     p4m2.SetXYZM(t2.px(), t2.py(), t2.pz(), MMUON);
     p4psi = p4m1 + p4m2;
     if (TMath::Abs(p4psi.M() - MJPSI) < TMath::Abs(mbest - MJPSI)) {
	cout << "HFJpsiXCand> replacing  m = " << mbest << " with " << p4psi.M()
	     << " from muon idxs = " << midx[im] << " and " << midx[in] << endl;
	mbest = p4psi.M();
	m1best = p4m1;
	m2best = p4m2;
	im1 = midx[im];
	im2 = midx[in];
     }
   }
 }
 if ((im1 < 0) || (im2 < 0)) {
   return;
 }

 vector<TransientTrack>vtt;
 TrackBaseRef rm1(fTracksHandle, im1); vtt.push_back(fTTB->build(*rm1));
 TrackBaseRef rm2(fTracksHandle, im2); vtt.push_back(fTTB->build(*rm2));

 KalmanVertexFitter fitter;

 if (1) {
   TwoTrackMinimumDistance md, md2;
   reco::Track t1(*rm1);
   reco::Track t2(*rm2);
   CovarianceMatrix cm = t1.covariance();
   CovarianceMatrix cm2 = t2.covariance();
//   cout << cm << endl;
//   cm*= 5.;
//   cout << cm << endl;
   Track tmpt(t1.chi2(), t1.ndof(), t1.referencePoint(), t1.momentum(), t1.charge(), cm);
   Track tmpt2(t2.chi2(), t2.ndof(), t2.referencePoint(), t2.momentum(), t2.charge(), cm2);
//   CovarianceMatrix cmt = tmpt.covariance();
   TransientTrack tt1  = fTTB->build(*rm1);
   TransientTrack tt1a = fTTB->build(tmpt);
   TransientTrack tt2  = fTTB->build(*rm2);
   TransientTrack tt2a = fTTB->build(tmpt2);
   vector<TransientTrack> vtt;
   vtt.push_back(tt1);
   vtt.push_back(tt2);
   TransientVertex myVertex = fitter.vertex(vtt);
   AnalyticalImpactPointExtrapolator extrapolator(fMagneticField);
   TrajectoryStateOnSurface tsos1 = extrapolator.extrapolate(tt1.initialFreeState(), myVertex.position());
   TrajectoryStateOnSurface tsos2 = extrapolator.extrapolate(tt2.initialFreeState(), myVertex.position());
   TrajectoryStateOnSurface tsos1a = extrapolator.extrapolate(tt1a.initialFreeState(), myVertex.position());
   TrajectoryStateOnSurface tsos2a = extrapolator.extrapolate(tt2a.initialFreeState(), myVertex.position());
   md.calculate(tsos1, tsos2);
   md2.calculate(tsos1a, tsos2a);

   cout << " XXXX doca1 " << md.distance() << " " << rm1->d0Error() << endl;

    CovarianceMatrix m_out, m_out2;
    Double_t data[5][5], data2[5][5];
    typedef unsigned int index;
    index idx = 0;
   // m_out(3,3) = sqrt(cm(3,3))*2;
    for (index i=0; i<5; i++) {
    for (index j=0; j <= i; j++)  {      
         data[i][j] = cm(i,j);
         data2[i][j] = cm2(i, j);
     //    if (sqrt(data[i][j]) == rm1->dszError() ) cout << " MATCH!!!! " << i << ", " << j << endl;
     }
    }
     
      math::XYZVector mom1, mom2;
      mom1 = t1.momentum();
      mom2 = t2.momentum();
      Double_t rescale = sqrt(data[3][3])/5;
      Double_t rescale2 = sqrt(data2[3][3])/5;
      
      /// dz error definition ///
      Double_t dzError = sqrt(data[4][4]) * mom1.R() / sqrt(mom1.Perp2()) ;
      Double_t resDzError = dzError/5;

      Double_t dzError2 = sqrt(data2[4][4]) * mom2.R() / sqrt(mom2.Perp2()) ;
      Double_t resDzError2 = dzError2/5;

      CovarianceMatrix m_res, m_res1;
      Double_t resm = resDzError * sqrt(mom1.Perp2()) / mom1.R();
      Double_t mdz = resm*resm;

      Double_t resm2 = resDzError2 * sqrt(mom2.Perp2()) / mom2.R();
      Double_t mdz2 = resm2*resm2;

      
   for (index i=0; i<5; i++) {
    for (index j=0; j <= i; j++)  {

         m_out(i,j) = data[i][j];
         m_out2(i,j) = data2[i][j];        

/*         if(i==3 && j==3) {
            m_out(i,j) = rescale*rescale;
            m_out2(i,j) = rescale2*rescale2;
         }*/
         if (i ==4 && j==4) {
            m_out(i, j) = mdz;
            m_out2(i, j) = mdz2;
        }
         
//         else m_out(i,j) = data[i][j];
   }
  }  

//    cout << "DEFINITION " << dzError << " ORIGINAL " << rm1->dzError() << " " << m_res << " " << m_out << " " << data << endl;
//    cout <<  " RESCALED 3x3 " << m_out(3,3) << " ORIGINAL 3x3 " << cm(3,3) << endl;    
//     cout << " m_out "  << m_out << " original " << cm << endl;

    Track restmp(t1.chi2(), t1.ndof(), t1.referencePoint(), t1.momentum(), t1.charge(), m_out);  
    Track restmp2(t2.chi2(), t2.ndof(), t2.referencePoint(), t2.momentum(), t2.charge(), m_out2);

   TransientTrack tt1res = fTTB->build(restmp);
   TransientTrack tt2res = fTTB->build(restmp2);
   vector<TransientTrack> vttres;
   vttres.push_back(tt1res);
   vttres.push_back(tt2res);
   TransientVertex myVertexRes = fitter.vertex(vttres);
   TrajectoryStateOnSurface tsosres1 = extrapolator.extrapolate(tt1res.initialFreeState(), myVertexRes.position());
   TrajectoryStateOnSurface tsosres2 = extrapolator.extrapolate(tt2res.initialFreeState(), myVertexRes.position());
  
     TwoTrackMinimumDistance mdres;
     mdres.calculate(tsosres1, tsosres2);

     cout << " XXXX doca2 " << mdres.distance() << endl;
//     cout << " cm2 " << cm2 << endl;
//     cout << " m_out2 " << m_out2 << endl;
 /*    cout << " XXXX doca1 " <<  md.distance() << " matrix " << cm(4,4) << " sqrt matrix " << sqrt(cm(4,4)) << " D0 error " << rm1->d0Error() << " dz Error " << rm1->dzError() << endl;
     cout << " XXXX doca2 " << mdres.distance() << " matrix " << m_out(4,4) << " sqrt matrix " << sqrt(m_out(4,4)) << " D0 Error " << rm2->d0Error() << " dz Error " << rm2->dzError() << endl;*/
   }
 }


//define this as a plug-in
DEFINE_FWK_MODULE(HFJpsiXCand);
