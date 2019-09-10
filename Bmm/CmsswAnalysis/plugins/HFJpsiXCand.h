#ifndef _HFJPSIXCAND_h_
#define _HFJPSIXCAND_h_

#include "Bmm/CmsswAnalysis/plugins/HFVirtualDecay.h"
#include "Bmm/CmsswAnalysis/interface/HFTrackListBuilder.hh"

class HFJpsiXCand : public HFVirtualDecay {
  
 public:
  explicit HFJpsiXCand(const edm::ParameterSet&);

 protected:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void dumpConfiguration();
 
  int    fRefType;
   
};

#endif
