// @(#)root/tmva $Id$
/**********************************************************************************
 * Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 ***********************************************************************************/

//Usage:
//

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
//#include "TMVA/TMVAGui.h"

int BDT_Run2( TString filo = "MVAoutput.root", TString myMethodList = "" )
{
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the
   // corresponding lines from .rootrc

   // methods to be processed can be given as an argument; use format:
   //
   // > root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)


   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;
   
   // --- Boosted Decision Trees
   Use["BDT"]             = 1; // uses Adaptive Boost
   Use["BDTR"]            = 0; // uses real Adaptive Boost
   Use["BDTG"]            = 0; // uses Gradient Boost
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting 
   // ---------------------------------------------------------------

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return 1;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // --- Here the preparation phase begins

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "BDT_Run2.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory is 
   // the only TMVA object you have to interact with
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in
   // front of the "Silent" argument in the option string
   TMVA::Factory *factory = new TMVA::Factory( "BDT110_65-B", outputFile,
                                               "!V:!Silent:!Color:!DrawProgressBar:Transformations=I:AnalysisType=Classification" );//Transformations=I;D;P;U;G,D

   TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");
   // If you wish to modify default settings
   // (please check "root/tmva/tmva/src/Config.cxx" to see all available global options)
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]

   // Read training and test data
   // (it is also possible to use ASCII format as input -> see TMVA Users Guide)
   TString fname = filo;
   string inputfile = filo.Data();

   if (gSystem->AccessPathName( fname ))  // file does not exist in local directory
     {
       std::cout << "Input file could not be found!" << std::endl;
       return 1;
     }   
   TFile *input = TFile::Open( fname );
   
   std::cout << "--- TMVAClassification       : Using input file: " << input->GetName() << std::endl;
   

   std::cout << "Adding muon ID variables...";
   //VARIABLES
//     dataloader->AddVariable( "Qprod", 'I' );
     dataloader->AddVariable( "vMuonHitComb", 'F' );
     dataloader->AddVariable( "trkRelChi2", 'F' );
//     dataloader->AddVariable( "TMTrkMult100", 'F' );
     dataloader->AddVariable( "valPixHits", 'F' );
     dataloader->AddVariable( "innerChi2", 'F' );
     dataloader->AddVariable( "outerChi2", 'F' );
     dataloader->AddVariable( "timeAtIpInOutErr", 'F' );
     dataloader->AddVariable( "glbKinkFinder", 'F' );
     dataloader->AddVariable( "kinkFinder", 'F' );
//     dataloader->AddVariable( "dzRef", 'F' );
//     dataloader->AddVariable( "dxyRef", 'F' );
     dataloader->AddVariable( "LWH", 'F' );
//     dataloader->AddVariable( "dr", 'F' );
//     dataloader->AddVariable( "glbTrackProb", 'F' );
     dataloader->AddVariable( "trkValidFract", 'F' );
  //   dataloader->AddVariable( "NTrkVHits", 'F' );
     dataloader->AddVariable( "chi2LocPos", 'F' );
     dataloader->AddVariable( "chi2LocMom", 'F' );
     dataloader->AddVariable( "segComp", 'F' );
   //SPECTATORS
   dataloader->AddSpectator( "pID", "pID", "units",'I');
   dataloader->AddSpectator( "pt", "pt", "units",'F');
   dataloader->AddSpectator( "eta", "eta", "units",'F');
   dataloader->AddSpectator( "MomID", "MomID", "units",'I');
   std::cout << " Done." << std::endl;

   // --- Register the training and test trees

   string signaltreeinput = "";
   string backgroundtreeinput = "";
   bool skipper = true;

   Double_t signalWeight     = 1.0;
   Double_t backgroundWeight = 1.0;

   outputFile->cd();
   TTree *reduced = (TTree*)input->Get("candAnaMuMu/muonidtree");
   if (!reduced)
     {
       reduced = (TTree*)input->Get("candAnaMuMu/muonidtree");
       if (!reduced)
         {std::cout << "Could not open reduced tree." << endl;return 1;}
     }
   TTree *Trues(0);
//   TTree *Pions(0);
   TTree *Kaons(0);
   
   Float_t eta;
   Int_t pID;

   TCut etaCut = "TMath::Abs(eta) <= 1.4 && TMath::Abs(eta) <= 2.1";
   TCut ptCut = "pt > 4";
   TCut TruesCut = "TMath::Abs(pID) == 13 && (TMath::Abs(MomID)==511 || TMath::Abs(MomID)==531)";
//   TCut PionsCut = "TMath::Abs(pID) == 211";
   TCut KaonsCut = "TMath::Abs(pID) == 321";
   TCut varCut0 = "chi2LocMom<5000";
   TCut varCut1 = "chi2LocPos<2000";
   TCut varCut2 = "glbTrackTailProb<5000";
   TCut varCut3 = "kinkFinder<900";
   TCut varCut4 = "TMath::Log(2+glbKinkFinder)<50";
   TCut varCut5 = "timeAtIpInOutErr<4";
   TCut varCut6 = "outerChi2<1000";
   TCut varCut7 = "innerChi2<10";
   TCut varCut8 = "trkRelChi2<3";

   int bkgNevents(0);
   bkgNevents += reduced->GetEntries(KaonsCut);
//(ptCut+etaCut+PionsCut+varCut0+varCut1+varCut2+varCut3+varCut4+varCut5+varCut6+varCut7+varCut8);
//   bkgNevents += reduced->GetEntries(ptCut+etaCut+KaonsCut+varCut0+varCut1+varCut2+varCut3+varCut4+varCut5+varCut6+varCut7+varCut8);*/
   Trues = reduced->CopyTree(TruesCut);
//(ptCut+etaCut+TruesCut+varCut0+varCut1+varCut2+varCut3+varCut4+varCut5+varCut6+varCut7+varCut8);
   Trues->SetName("Trues");
//   Pions = reduced->CopyTree(ptCut+etaCut+PionsCut+varCut0+varCut1+varCut2+varCut3+varCut4+varCut5+varCut6+varCut7+varCut8);
//   Pions->SetName("Pions");
   Kaons = reduced->CopyTree(KaonsCut);
//(ptCut+etaCut+KaonsCut+varCut0+varCut1+varCut2+varCut3+varCut4+varCut5+varCut6+varCut7+varCut8);
   Kaons->SetName("Kaons");

   dataloader->AddSignalTree(Trues,signalWeight);
  // dataloader->AddBackgroundTree(Pions,backgroundWeight);
   dataloader->AddBackgroundTree(Kaons,backgroundWeight);
     

   //CONVENTIONAL SELECTION OF TRAINING AND TESTING EVENTS
   // if (!skipper) {std::cout << "How is the signal tree called?\n";}
   // if (skipper || (getline(cin,signaltreeinput) && signaltreeinput.empty()))
   //   {
   //     TTree *signal = (TTree*)input->Get("AnamuonID/muonIDtruesB");
   //     factory->AddSignalTree    ( signal,     signalWeight     );
   //   }
   // else
   //   {
   //     const char * signaltree = signaltreeinput.c_str();
   //     TTree *signal = (TTree*)input->Get(signaltree);
   //     factory->AddSignalTree    ( signal,     signalWeight     );
   //   }
   // if (!skipper) {std::cout << "How is the background tree called?\n";}
   // if (skipper || (getline(cin,backgroundtreeinput) && backgroundtreeinput.empty()))
   //   {
   //     TTree *backgroundP = (TTree*)input->Get("AnamuonID/muonIDfakesPionsB");
   //     factory->AddBackgroundTree( backgroundP, backgroundWeight );
   //     TTree *backgroundK = (TTree*)input->Get("AnamuonID/muonIDfakesKaonsB");
   //     factory->AddBackgroundTree( backgroundK, backgroundWeight );
   //   }
   // else
   //   {
   //     const char * backgroundtree = backgroundtreeinput.c_str();
   //     TTree *background = (TTree*)input->Get(backgroundtree);
   //     factory->AddBackgroundTree( background, backgroundWeight );
   //   }

   // if (inputfile.empty())
   //   {
   //     factory->SetBackgroundWeightExpression( "weight" );
   //   }


   // Apply additional cuts on the signal and background samples (can be different)
   TCut mycuts = ""; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   TCut mycutb = ""; // for example: TCut mycutb = "abs(var1)<0.5";

   // Tell the factory how to use the training and testing events
   //
   // If no numbers of events are given, half of the events in the tree are used 
   // for training, and the other half for testing:
       dataloader->PrepareTrainingAndTestTree( mycuts, "SplitMode=random:!V" );
   // To also specify the number of testing events, use:
   //    factory->PrepareTrainingAndTestTree( mycut,
   //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );
//   dataloader->PrepareTrainingAndTestTree( mycuts, mycutb,
  //                                      "nTrain_Signal=10000:nTrain_Background=10000:SplitMode=Random:NormMode=NumEvents:!V" );

// TMVA::VariableImportance * vi = new TMVA::VariableImportance(dataloader);


// dataloader->PrepareTrainingAndTestTree(mycuts, mycutb,Form("SplitMode=Random:NormMode=NumEvents:SplitSeed=65:!V:nTrain_Signal=%d",(bkgNevents/2)*2));//:nTrain_Signal=2000:nTrain_Background=2000//nTrain_Signal=1000:nTest_Signal=1000:nTrain_Background=1000:nTest_Background=1000:

   // ---- Book MVA methods
   //
   // Please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable


   // Boosted Decision Trees
   if (Use["BDTG"]) // Gradient Boost
     factory->BookMethod( dataloader,TMVA::Types::kBDT, "BDTG",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2" );

   if (Use["BDT"])  // Adaptive Boost
     factory->BookMethod( dataloader,TMVA::Types::kBDT, "BDT",
                           "!H:!V:NTrees=10:MinNodeSize=1.5%:MaxDepth=12:BoostType=RealAdaBoost:AdaBoostBeta=0.3:UseBaggedBoost:BaggedSampleFraction=0.05:SeparationType=GiniIndex:nCuts=-1:CreateMVAPdfs:DoBoostMonitor" );

   if (Use["BDTR"])  // Adaptive Boost
     factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTR",
                           "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=RealAdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=-1:CreateMVAPdfs" );

   if (Use["BDTB"]) // Bagging
     factory->BookMethod( dataloader,TMVA::Types::kBDT, "BDTB",
                           "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20" );

   if (Use["BDTD"]) // Decorrelation + Adaptive Boost
     factory->BookMethod( dataloader,TMVA::Types::kBDT, "BDTD",
                           "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=-1:VarTransform=Decorrelate" );

   if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
     factory->BookMethod( dataloader,TMVA::Types::kBDT, "BDTMitFisher",
                           "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20" );

   // For an example of the category classifier usage, see: TMVAClassificationCategory
   // --------------------------------------------------------------------------------------------------

   // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events

   // ---- STILL EXPERIMENTAL and only implemented for BDT's ! 
   // factory->OptimizeAllMethods("SigEffAt001","Scan");
   //factory->OptimizeAllMethods("ROCIntegral","FitGA");

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   //write training results (containing variable ranking) into rankings.txt
 /*   ofstream rankingfile; 
    rankingfile.open("rankingB.txt");
    stringstream ranks;
    streambuf *old = cout.rdbuf();
    cout.rdbuf (ranks.rdbuf());*/

   // Train MVAs using the set of training events
   factory->TrainAllMethods(); // prints variable ranking -->batch mode


/*    rankingfile << ranks.rdbuf();
    cout.rdbuf (old);
    rankingfile.close();*/

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();
   
   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();
   // tmpfile->Close();


   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

   delete factory;
   delete dataloader;
 /*   string formulas[] = {"S/sqrt(S+B)","S/sqrt(B)","S/(2.5+sqrt(B))","S/(25+10*sqrt(B))"};
    for (int i=0;i<sizeof(formulas)/sizeof(formulas[0]); i++)
      {
        TMVA::mvaeffs("dataset", outfileName,true,formulas[i], 230);
        if (i<sizeof(formulas)/sizeof(formulas[0])-1)
    	 {cout << "Press ENTER to see the efficiencies for the next formula." << endl;}
        if (getline(cin,inputfile) || inputfile.empty())
    	 {continue;}
      }

     TMVA::mvaeffs("dataset", outfileName);*/

   // Launch the GUI for the root macros
   if (!gROOT->IsBatch()) TMVA::TMVAGui( outfileName );

   return 0;
}
 /*  if (!gROOT->IsBatch()) TMVA::TMVAGui( outfileName );

   return 0;
}*/

int main( int argc, char** argv )
{
   // Select methods (don't look at this code - not of interest)
   TString methodList; 
   for (int i=1; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(","); 
      methodList += regMethod;
   }
   return BDT_Run2(methodList); 
}
