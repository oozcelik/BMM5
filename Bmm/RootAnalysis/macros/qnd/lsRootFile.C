#include <iostream>
#include <fstream>
#include <TCanvas.h>
#include <TROOT.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include "TCanvas.h"
#include "TStyle.h"
#include <iostream>
#include <fstream>
#include "TH1.h"
#include "TGraph.h"
#include "TColor.h"
#include "TKey.h"

void lsDir(string oname) {
  ofstream OUT(oname);
  TKey *key(0);
  string sname;
  TIter next(gDirectory->GetListOfKeys());
  while ((key = (TKey*)next())) {
    sname = key->GetName();
    OUT << sname << endl;
  }
  OUT.close();
}
