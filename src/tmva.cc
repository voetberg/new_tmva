#include <iostream>                                                             
#include <cstdlib>                                                              
#include <map>                                                                  
#include <string>                                                               
                                                                                
#include <TChain.h>                                                             
#include <TFile.h>                                                              
#include <TTree.h>                                                         
#include <TString.h>                                                            
#include <TObjString.h>                                                         
#include <TSystem.h>                                                            
                                                                                
#include <TMVA/Factory.h>                                                       
#include <TMVA/DataLoader.h>                                                    
#include <TMVA/Tools.h>                                                         
                                                                                
using namespace std;                                                            
                                                                                
int main(){                                                                     
  TMVA::Tools::Instance();                                                      
  map<string, int> Use;

  TFile* out = TFile::Open("n_bdt_tmva.root", "RECREATE"); 
  TMVA::Factory* factory = new TMVA::Factory("TMVAClassification", out, 
    "!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");

  cout<<"Made factory"<<endl; 

  //========================================================================
  vector<TString>num; 
  num.emplace_back("50");
  num.emplace_back("100");
  num.emplace_back("150");
  num.emplace_back("200");
  num.emplace_back("250");
  num.emplace_back("300");
  num.emplace_back("350");
  num.emplace_back("400");
  num.emplace_back("400p");
  

  TMVA::DataLoader*data[9] = {
    new TMVA::DataLoader("data_pt50"),
    new TMVA::DataLoader("data_pt100"),
    new TMVA::DataLoader("data_pt150"),
    new TMVA::DataLoader("data_pt200"),
    new TMVA::DataLoader("data_pt250"),
    new TMVA::DataLoader("data_pt300"),
    new TMVA::DataLoader("data_pt350"), 
    new TMVA::DataLoader("data_pt400"),
    new TMVA::DataLoader("data_pt400p")
  };
  
  cout<<"Made dataloaders"<<endl; 

  for (int i=0; i<9; ++i){
    TFile* in = new TFile("$TMPDIR/n_18vari/n_18_"+num[i]+".root"); 
    cout<<"read in file "<<in->GetName()<<endl; 

    data[i]->AddVariable("s",'F');       
    data[i]->AddVariable("yydr",'F');     
    data[i]->AddVariable("y1dr",'F');      
    data[i]->AddVariable("y2dr",'F');      
    data[i]->AddVariable("ptratio",'F');        
    data[i]->AddVariable("y1y",'F');  
    data[i]->AddVariable("y2y",'F');  
    data[i]->AddVariable("yyy",'F'); 
    data[i]->AddVariable("jety",'F');   
    data[i]->AddVariable("yydy",'F');   
                                     
    data[i]->AddVariable("y1E",'F'); 
    data[i]->AddVariable("y2E",'F'); 
    data[i]->AddVariable("yyE",'F'); 
    data[i]->AddVariable("jetE",'F');     
    
    data[i]->AddVariable("y1pt",'F');
    data[i]->AddVariable("y2pt",'F');
    data[i]->AddVariable("yypt",'F');
                                     
    data[i]->AddVariable("costhet",'F');     
  
    cout<<"problem not in variables"<<endl; 

    TTree* sigTrain = (TTree*)in->Get("sTrain");                             
    TTree* bgTrain = (TTree*)in->Get("bTrain");                               
    TTree* sigTest = (TTree*)in->Get("sTest");                               
    TTree* bgTest = (TTree*)in->Get("bTest"); 
  
    Float_t sig_w = 1./(sigTrain->GetEntries() + sigTest->GetEntries());  
    Float_t bg_w = 3./(bgTrain->GetEntries() + bgTest->GetEntries());   
  
    data[i]->AddSignalTree(sigTrain, sig_w, "Training"); 
    data[i]->AddSignalTree(sigTest, sig_w, "Testing"); 
    data[i]->AddBackgroundTree(bgTrain, bg_w, "Training"); 
    data[i]->AddBackgroundTree(bgTest, bg_w, "Testing"); 
  

    cout<<"Set data for dataloader"<<num[i]<<endl; 

    //Add weights 
    //Loading in weights 
    TMVA::DataLoader* weight_data = new TMVA::DataLoader("weight_data"); 
    weight_data->AddVariable("weight", 'F'); 
  
    weight_data->AddSignalTree(sigTrain, sig_w, "Training"); 
    weight_data->AddSignalTree(sigTest, sig_w, "Testing"); 
    weight_data->AddBackgroundTree(bgTrain, bg_w, "Training"); 
    weight_data->AddBackgroundTree(bgTest, bg_w, "Testing"); 
  
    data[i]->SetWeightExpression("weight"); 
  }
  //============================================================================
  //Factories 
  TString settings = "NTrees=800:MaxDepth=5:nCuts=40:BoostType=Grad:!UseRandomisedTrees:UseYesNoLeaf";
  
  for (int i=0; i<9; ++i){ 
    factory->BookMethod(data[i], TMVA::Types::kBDT, "BDT_" + num[i], settings);
  }

  factory->TrainAllMethods(); 
  factory->TestAllMethods(); 
  factory->EvaluateAllMethods(); 
  
  out->Write(); 
  out->Close(); 
  
  delete factory; 
  
  for (int i=0; i<9; ++i){
    delete data[i]; 
  }
}
