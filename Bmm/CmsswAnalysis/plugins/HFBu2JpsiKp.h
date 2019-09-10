#ifndef _HFBU2JPSIKP_h_
#define _HFBU2JPSIKP_h_

#include "Bmm/CmsswAnalysis/plugins/HFVirtualDecay.h"
#include "Bmm/CmsswAnalysis/interface/HFTrackListBuilder.hh"

class HFBu2JpsiKp : public HFVirtualDecay {
  
 public:
  explicit HFBu2JpsiKp(const edm::ParameterSet&);
  float getScale(HFTrackListBuilder *fListBuilder);
  
 protected:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void dumpConfiguration();
  
  int           fPsiMuons;
  double        fPsiLo, fPsiHi;
};

#endif
