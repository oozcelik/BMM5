#include "TFile.h"
#include "TH1F.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include <iostream>   
#include <string>  
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
#include "TH2.h"

void ROCcurve(){

// Open the file containing the tree.
TFile *myFile = TFile::Open("BDT110_65-B.root");

TTreeReader reader ("dataset/TestTree", myFile);

///// either signal or background
TTreeReaderArray<char> reader_array (reader, "className");


TTreeReaderValue<float> val (reader, "BDT");
TTreeReaderValue<int> class_id (reader, "classID");

double nentries = reader.GetEntries(true);
int signal_class;
int background_class = 0;

reader.SetEntry(0);

// This needed to assign it to an int 
TString className = static_cast<char*>(reader_array.GetAddress());

// This is to accomodate multi-class classification
if (className== "Signal") { // Signal
    signal_class = *class_id;
    cout << *class_id << " Signal " << signal_class << endl;
} else if (className== "Background") { // Background
    signal_class = 1-*class_id;
    cout << signal_class << " Background " << *class_id << endl;
} else {
    throw("ERROR!");
}



int n_true_positives = 0;
int n_false_positives = 0;
bool firstEntry = true;

TH2D *ROCcurve = new TH2D("h1", "ntuple", 100, 0, 1, 100, 0, 1);

///// *class_id is int class, either 0 or 1 depends on the order of the dataloader declarations, 
/////  i.e.,
/////  dataloader->AddSignalTree();     // class_id 0
////   dataloader->AddBackgroundTree(); // class_id 1

/// *val is the BDT cut value
 while (reader.Next()) {
   if (*val >= 0.3 &&  signal_class == *class_id) {
      n_true_positives++;
      cout << " in the loop sig " << *class_id << endl;
    }
   else if (*val > 0.3 && signal_class != *class_id ) {
     cout << " in the loop bck " << *class_id << endl;
     n_false_positives++;
   }
//// signal eff = N(BDT)/Nentries
   double sigeff = n_true_positives/nentries;
//// background eff = N(BDT)/Nentries
   double  backeff =  n_false_positives/nentries;


   ROCcurve->Fill(sigeff, backeff);
 }
  TCanvas *c1 = new TCanvas ("c1","ROC Curve",200,10,600,400);
  ROCcurve->Draw();
}
