
#include <iostream>
#include <TString.h>

void checkChain(const char *chain, const char *tree) {
  char buffer[2000];
  char meta[2000];
  sprintf(buffer, "c%s", chain);
  ofstream CHK(buffer);

  TFile *f;
  TTree *t;
  int nentries(0), total(0);

  ifstream is(chain);

  while (is.getline(buffer, 2000, '\n')) {
    nentries = -1;
    sscanf(buffer, "%s", meta);
    f = 0;
    f = TFile::Open(meta);
    if (f && !(f->TestBit(TFile::kRecovered))) {
      t = 0;
      t = (TTree*)f->Get(tree);
      if (t) {
        nentries = t->GetEntries();
        if (nentries > 0) {
          total += nentries;
          cout << meta << "  " << nentries <<endl;
          CHK << meta << "  " << nentries <<endl;
        } else {
          cout << meta << " contains " << nentries << " entries" << endl;
        }
      } else {
        cout << "no tree found" << endl;
      }
    } else {
      if (f->TestBit(TFile::kRecovered)) {
        cout << "Skipping recovered file " << meta << endl;
      } else {
        cout << "Cannot open file " << meta << endl;
      }
    }
    if (f) {
	f->Close();
    }
  }
  CHK.close();
  cout << chain << ": " << total <<endl;
  cout << "----------------------------------------------------------------------" << endl;
  //   ofstream os(TString(chain)+TString(".cnt"));
  //   os << total << endl;
  //   os.close();
}

