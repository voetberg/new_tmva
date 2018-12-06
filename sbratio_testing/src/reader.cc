#include <iostream>         
#include <vector>           
#include <cstdlib>          
                            
#include <TChain.h>         
#include <TFile.h>          
#include <TTree.h>          
#include <TString.h>        
#include <TSystem.h>        
#include <TROOT.h>          
                            
#include <TMVA/Tools.h>     
#include <TMVA/Reader.h>    
#include <TMVA/MethodCuts.h>

#include <fstream>                                              
 
using namespace std; 

int main(){
  TMVA::Tools::Instance(); 
  map<string,int> Use; 

  TMVA::Reader* reader[9] = {
    new TMVA::Reader("Color:!Silent"), 
    new TMVA::Reader("Color:!Silent"), 
    new TMVA::Reader("Color:!Silent"), 
    new TMVA::Reader("Color:!Silent"), 
    new TMVA::Reader("Color:!Silent")
  };

  cout<<"Made Readers"<<endl; 
  
  vector<TString> num; 
  num.emplace_back("50");
  num.emplace_back("100");
  num.emplace_back("150");
  num.emplace_back("200");
  num.emplace_back("250");
  num.emplace_back("300");
  num.emplace_back("350");
  num.emplace_back("400");
  num.emplace_back("400p");

  //==============================================================
  Float_t yydr, y1dr, y2dr, ptratio, y1y, y2y, jety, s; 
  Float_t yydy, y1E, y2E, yyE, jetE, yyy, costhet; 
  Float_t y1pt, y2pt, yypt, jetpt; 
  
  //All in one file because I said so
  TString outname = "TrainingResult18_n_3bg.root";
  TFile* out_root = new TFile(outname, "RECREATE"); 
  
  //Output Histograms 
  TH1D* h_sig[9] = {
    new TH1D("50_Signal", "50_Signal", 200.,-1.,1.),
    new TH1D("100_Signal", "100_Signal", 200.,-1.,1.),
    new TH1D("150_Signal", "150_Signal", 200.,-1.,1.),
    new TH1D("200_Signal", "200_Signal", 200.,-1.,1.),
    new TH1D("250_Signal", "250_Signal", 200.,-1.,1.)
    new TH1D("300_Signal", "300_Signal", 200.,-1.,1.)
    new TH1D("350_Signal", "350_Signal", 200.,-1.,1.)
    new TH1D("400_Signal", "400_Signal", 200.,-1.,1.)
    new TH1D("400p_Signal", "400p_Signal", 200.,-1.,1.)
  };
  TH1D* h_bg[9] = {
    new TH1D("50_Background", "50_Background", 200.,-1.,1.),
    new TH1D("100_Background", "100_Background", 200.,-1.,1.),
    new TH1D("150_Background", "150_Background", 200.,-1.,1.),
    new TH1D("200_Background", "200_Background", 200.,-1.,1.),
    new TH1D("250_Background", "250_Background", 200.,-1.,1.)
    new TH1D("300_Background", "300_Background", 200.,-1.,1.)
    new TH1D("350_Background", "350_Background", 200.,-1.,1.)
    new TH1D("400_Background", "400_Background", 200.,-1.,1.)
    new TH1D("400p_Background", "400p_Background", 200.,-1.,1.)
  };

  TH1D* h_res_s[9] = {
    new TH1D("50_Res_Signal", "50_Res_Signal", 200.,-1.,1.),
    new TH1D("100_Res_Signal", "100_Res_Signal", 200.,-1.,1.),
    new TH1D("150_Res_Signal", "150_Res_Signal", 200.,-1.,1.),
    new TH1D("200_Res_Signal", "200_Res_Signal", 200.,-1.,1.),
    new TH1D("250_Res_Signal", "250_Res_Signal", 200.,-1.,1.)
    new TH1D("300_Res_Signal", "300_Res_Signal", 200.,-1.,1.)
    new TH1D("350_Res_Signal", "350_Res_Signal", 200.,-1.,1.)
    new TH1D("400_Res_Signal", "400_Res_Signal", 200.,-1.,1.)
    new TH1D("400p_Res_Signal", "400p_Res_Signal", 200.,-1.,1.)
  };

  TH1D* h_res_b[9] = {
    new TH1D("50_Res_Background", "50_Res_Background", 200.,-1.,1.),
    new TH1D("100_Res_Background", "100_Res_Background", 200.,-1.,1.),
    new TH1D("150_Res_Background", "150_Res_Background", 200.,-1.,1.),
    new TH1D("200_Res_Background", "200_Res_Background", 200.,-1.,1.),
    new TH1D("250_Res_Background", "250_Res_Background", 200.,-1.,1.)
    new TH1D("300_Res_Background", "300_Res_Background", 200.,-1.,1.)
    new TH1D("350_Res_Background", "350_Res_Background", 200.,-1.,1.)
    new TH1D("400_Res_Background", "400_Res_Background", 200.,-1.,1.)
    new TH1D("400p_Res_Background", "400p_Res_Background", 200.,-1.,1.)
  };  

  TH1D* h_res_d[9] = {
    new TH1D("50_Res_Data", "50_Res_Data", 200.,-1.,1.),
    new TH1D("100_Res_Data", "100_Res_Data", 200.,-1.,1.),
    new TH1D("150_Res_Data", "150_Res_Data", 200.,-1.,1.),
    new TH1D("200_Res_Data", "200_Res_Data", 200.,-1.,1.),
    new TH1D("250_Res_Data", "250_Res_Data", 200.,-1.,1.)
    new TH1D("300_Res_Data", "300_Res_Data", 200.,-1.,1.)
    new TH1D("350_Res_Data", "350_Res_Data", 200.,-1.,1.)
    new TH1D("400_Res_Data", "400_Res_Data", 200.,-1.,1.)
    new TH1D("400p_Res_Data", "400p_Res_Data", 200.,-1.,1.)
  };

  for (int i=0; i<9; ++i){
    reader[i]->AddVariable("s", &s); 
    reader[i]->AddVariable("yydr", &yydr); 
    reader[i]->AddVariable("y1dr", &y1dr); 
    reader[i]->AddVariable("y2dr", &y2dr); 
    reader[i]->AddVariable("ptratio", &ptratio); 
    reader[i]->AddVariable("y1y", &y1y); 
    reader[i]->AddVariable("y2y", &y2y); 
    reader[i]->AddVariable("yyy", &yyy); 
    reader[i]->AddVariable("jety", &jety); 
    reader[i]->AddVariable("yydy", &yydy); 
    
    reader[i]->AddVariable("y1E", &y1E); 
    reader[i]->AddVariable("y2E", &y2E); 
    reader[i]->AddVariable("yyE", &yyE); 
    reader[i]->AddVariable("jetE", &jetE); 
    reader[i]->AddVariable("y1pt", &y1pt); 
    reader[i]->AddVariable("y2pt", &y2pt);
    reader[i]->AddVariable("yypt", &yypt); 
    
    reader[i]->AddVariable("costhet", &costhet); 
    reader[i]->AddVariable("weight", &weight); 

    cout<<"Reader Variables Added"<<endl;
      
    //=========================================================
  
    //Add method to the reader
    TString w = "/home/voetberg/voetberg/bdt_tmva_outputs/weights/data_pt" + num[i] + "/weights/TMVAClassification_BDT_" + num[i] + ".weights.xml"; 
    reader[i]->BookMVA("method", w); 
  
    cout<<"Method Added to the Reader "+num[i]<<endl; 
  
    //Read data 
    TFile* in = TFile::Open("/msu/data/t3work9/voetberg/tmva_input/n_18vari/3bg/3bg_n_18_" + num[i] + ".root"); 
    TFile* in2 = TFile::Open("/msu/data/t3work9/voetberg/tmva_input/n_18vari/n_18_" + num[i] + "_reader.root"); 
  
    cout<<"Opened read data for range "+num[i]<<endl; 
  
    //===========================================================
 
    TTree* sig = (TTree*)in->Get("sTest"); 
    TTree* bg = (TTree*)in->Get("bTest"); 
    TTree* res_s = (TTree*)in2->Get("sig"); 
    TTree* res_b = (TTree*)in2->Get("bg"); 
    TTree* res_d = (TTree*)in2->Get("data"); 
  
    //===========================================================
    //Signal 
    sig->SetBranchAddress("s", &s); 
    sig->SetBranchAddress("yydr", &yydr); 
    sig->SetBranchAddress("y1dr", &y1dr); 
    sig->SetBranchAddress("y2dr", &y2dr); 
    sig->SetBranchAddress("ptratio", &ptratio); 
    sig->SetBranchAddress("y1y", &y1y); 
    sig->SetBranchAddress("y2y", &y2y); 
    sig->SetBranchAddress("yyy", &yyy); 
    sig->SetBranchAddress("jety", &jety); 
    sig->SetBranchAddress("yydy", &yydy); 
    
    sig->SetBranchAddress("y1E", &y1E); 
    sig->SetBranchAddress("y2E", &y2E); 
    sig->SetBranchAddress("yyE", &yyE); 
    sig->SetBranchAddress("jetE", &jetE); 
    sig->SetBranchAddress("y1pt", &y1pt); 
    sig->SetBranchAddress("y2pt", &y2pt);
    sig->SetBranchAddress("yypt", &yypt); 
    
    sig->SetBranchAddress("costhet", &costhet); 
  
    //Background
    bg->SetBranchAddress("s", &s); 
    bg->SetBranchAddress("yydr", &yydr); 
    bg->SetBranchAddress("y1dr", &y1dr); 
    bg->SetBranchAddress("y2dr", &y2dr); 
    bg->SetBranchAddress("ptratio", &ptratio); 
    bg->SetBranchAddress("y1y", &y1y); 
    bg->SetBranchAddress("y2y", &y2y); 
    bg->SetBranchAddress("yyy", &yyy); 
    bg->SetBranchAddress("jety", &jety); 
    bg->SetBranchAddress("yydy", &yydy); 
    
    bg->SetBranchAddress("y1E", &y1E); 
    bg->SetBranchAddress("y2E", &y2E); 
    bg->SetBranchAddress("yyE", &yyE); 
    bg->SetBranchAddress("jetE", &jetE); 
    bg->SetBranchAddress("y1pt", &y1pt); 
    bg->SetBranchAddress("y2pt", &y2pt);
    bg->SetBranchAddress("yypt", &yypt); 
    
    bg->SetBranchAddress("costhet", &costhet); 
    
    //Responses
    //Sig
    res_s->SetBranchAddress("s", &s); 
    res_s->SetBranchAddress("yydr", &yydr); 
    res_s->SetBranchAddress("y1dr", &y1dr); 
    res_s->SetBranchAddress("y2dr", &y2dr); 
    res_s->SetBranchAddress("ptratio", &ptratio); 
    res_s->SetBranchAddress("y1y", &y1y); 
    res_s->SetBranchAddress("y2y", &y2y); 
    res_s->SetBranchAddress("yyy", &yyy); 
    res_s->SetBranchAddress("jety", &jety); 
    res_s->SetBranchAddress("yydy", &yydy); 
    
    res_s->SetBranchAddress("y1E", &y1E); 
    res_s->SetBranchAddress("y2E", &y2E); 
    res_s->SetBranchAddress("yyE", &yyE); 
    res_s->SetBranchAddress("jetE", &jetE); 
    res_s->SetBranchAddress("y1pt", &y1pt); 
    res_s->SetBranchAddress("y2pt", &y2pt);
    res_s->SetBranchAddress("yypt", &yypt); 
    
    res_s->SetBranchAddress("costhet", &costhet); 
    
    //Background
    res_b->SetBranchAddress("s", &s); 
    res_b->SetBranchAddress("yydr", &yydr); 
    res_b->SetBranchAddress("y1dr", &y1dr); 
    res_b->SetBranchAddress("y2dr", &y2dr); 
    res_b->SetBranchAddress("ptratio", &ptratio); 
    res_b->SetBranchAddress("y1y", &y1y); 
    res_b->SetBranchAddress("y2y", &y2y); 
    res_b->SetBranchAddress("yyy", &yyy); 
    res_b->SetBranchAddress("jety", &jety); 
    res_b->SetBranchAddress("yydy", &yydy); 
    
    res_b->SetBranchAddress("y1E", &y1E); 
    res_b->SetBranchAddress("y2E", &y2E); 
    res_b->SetBranchAddress("yyE", &yyE); 
    res_b->SetBranchAddress("jetE", &jetE); 
    res_b->SetBranchAddress("y1pt", &y1pt); 
    res_b->SetBranchAddress("y2pt", &y2pt);
    res_b->SetBranchAddress("yypt", &yypt); 
    
    res_b->SetBranchAddress("costhet", &costhet); 
   
    //Data
    res_d->SetBranchAddress("s", &s); 
    res_d->SetBranchAddress("yydr", &yydr); 
    res_d->SetBranchAddress("y1dr", &y1dr); 
    res_d->SetBranchAddress("y2dr", &y2dr); 
    res_d->SetBranchAddress("ptratio", &ptratio); 
    res_d->SetBranchAddress("y1y", &y1y); 
    res_d->SetBranchAddress("y2y", &y2y); 
    res_d->SetBranchAddress("yyy", &yyy); 
    res_d->SetBranchAddress("jety", &jety); 
    res_d->SetBranchAddress("yydy", &yydy); 
    
    res_d->SetBranchAddress("y1E", &y1E); 
    res_d->SetBranchAddress("y2E", &y2E); 
    res_d->SetBranchAddress("yyE", &yyE); 
    res_d->SetBranchAddress("jetE", &jetE); 
    res_d->SetBranchAddress("y1pt", &y1pt); 
    res_d->SetBranchAddress("y2pt", &y2pt);
    res_d->SetBranchAddress("yypt", &yypt); 
    
    res_d->SetBranchAddress("costhet", &costhet); 
    
    
    //==========================================================
  
    const Long64_t sig_entries = sig->GetEntries(); 
    const Long64_t bg_entries = bg->GetEntries(); 
    const Long64_t res_s_entries = res_s->GetEntries(); 
    const Long64_t res_b_entries = res_b->GetEntries(); 
    const Long64_t res_d_entries = res_d->GetEntries(); 
    
    cout<<""<<endl; 
    cout<<""<<endl; 
    cout<<num[i]<<endl;  
    cout<<"Signal Events: "<<sig_entries<<endl; 
    cout<<"Background Events: "<<bg_entries<<endl; 
    cout<<"Classify Evnts, Pure Signal: "<<res_s_entries<<endl; 
    cout<<"Classify Evnts, Pure Background: "<<res_b_entries<<endl; 
    cout<<"Classify Evnts, Data: "<<res_d_entries<<endl; 
    cout<<""<<endl; 
    //=============================================================
  
    for (long j=0; j<sig_entries; ++j){
      sig->GetEntry(j); 
      h_sig[i]->Fill(reader[i]->EvaluateMVA("method"));    
    }
  
    cout<<"Filled Signal Histogram "+num[i]<<endl; 
    
    //=============================================================
    
    for (long j=0; j<bg_entries; ++j){
      bg->GetEntry(j); 
      h_bg[i]->Fill(reader[i]->EvaluateMVA("method")); 
    }
  
    cout<<"Filled Background Histogram "+num[i]<<endl; 
    
    //==============================================================
    
    float sig_n_s=0, bg_n_s=0;
    for (long j=0; j<res_s_entries; ++j){
      res_s->GetEntry(j); 
      
      float result = reader[i]->EvaluateMVA("method"); 
      h_res_s[i]->Fill(result);
  
      if (result>0){
        sig_n_s+=1; 
      }
      else{
        bg_n_s+=1; 
      }
    }
    cout<<""<<endl; 
    cout<<"Filled Signal Response Histogram "+num[i]":"<<endl;
    cout<<"Signal Ratio: "<<sig_n_s/(res_s_entries)<<endl; 
    cout<<"Background Ratio: "<<bg_n_s/(res_s_entries)<<endl; 
    
    //================================================================
  
    float sig_n_b=0, bg_n_b=0;
    for (long j=0; j<res_b_entries; ++j){
      res_b->GetEntry(j); 
      
      float result = reader[i]->EvaluateMVA("method"); 
      h_res_b[i]->Fill(result);
  
      if (result>0){
        sig_n_b+=1; 
      }
      else{
        bg_n_b+=1; 
      }
    }
    cout<<""<<endl; 
    cout<<"Filled Background Response Histogram "+num[i]<<endl;
    cout<<"Signal Ratio: "<<sig_n_b/(res_b_entries)<<endl; 
    cout<<"Background Ratio: "<<bg_n_b/(res_b_entries)<<endl; 
  
    //================================================================
  
    float sig_n_d=0, bg_n_d=0;
    for (long j=0; j<res_d_entries; ++j){
      res_d->GetEntry(j); 
      
      float result = reader[i]->EvaluateMVA("method"); 
      h_res_d[i]->Fill(result);
  
      if (result>0){
        sig_n_d+=1; 
      }
      else{
        bg_n_d+=1; 
      }
    }
    cout<<""<<endl; 
    cout<<"Filled Data Response Histogram" +num[i]<<endl;
    cout<<"Signal Ratio: "<<sig_n_d/(res_d_entries)<<endl; 
    cout<<"Background Ratio: "<<bg_n_d/(res_d_entries)<<endl; 
    
    //==================================================================
    delete reader[i]; 
  }
  out_root->Write(); 
  out_root->Close();
}
