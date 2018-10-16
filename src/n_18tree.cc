#include <iostream> 
#include <vector>
#include <stdlib.h> 
#include <utility> 
#include <chrono> 

#include <boost/optional.hpp> 
#include <TFile.h> 
#include <TChain.h> 
#include <TH1.h> 
#include <TLorentzVector.h> 
#include <TTree.h> 
#include <TRandom3.h> 

#include "Higgs2diphoton.hh" 

using namespace std; 
Higgs2diphoton Hdecay; 
bool sort_pt (TLorentzVector i, TLorentzVector j){return (i.Pt()>j.Pt());} 

int main(){
  //===============================================
  //Chains
  TChain chain_s("t3"); 
  TChain chain_b("t3"); 

  //Sig
  //chain.Add("/msu/data/t3work4/luisonig/H1jets_ggf/NTuplesFiles/H1.0j_GGFHT_B_6500_pt25.0_eta4.5_r100_100.root",0); 
  chain_s.Add("$TMPDIR/H1.0j_GGFHT_B_6500_pt25.0_eta4.5_r100_100.root",0); 
  chain_s.Add("$TMPDIR/H1.0j_GGFHT_B_6500_pt25.0_eta4.5_r100_101.root",0); 
  //Bg 
  //chain.Add("/msu/data/t3work4/gosam_diphoton_jet/born/born1_1.root",0); 
  chain_b.Add("$TMPDIR/born1_1.root",0); 
  chain_b.Add("$TMPDIR/born1_2.root",0); 
  chain_b.Add("$TMPDIR/born1.root",0); 
  //===============================================
  //Variables for trees
  Float_t s, yydr, y1dr, y2dr, ptratio, y1y, y2y, jety; 
  Float_t yydy, y1E, y2E, yyE, jetE, y1pt, y2pt, yypt, yyy, costhet; 
  //===============================================
  //Branches from chains
  constexpr size_t max = 10; 
  Float_t px[max], py[max], pz[max], E[max]; 
  Int_t nparticle, kf[max]; 
  Double_t weight; 

  chain_s.SetBranchAddress("nparticle",&nparticle);
  chain_s.SetBranchAddress("weight2",&weight);
  chain_s.SetBranchAddress("kf",kf);
  chain_s.SetBranchAddress("px",px);
  chain_s.SetBranchAddress("py",py);
  chain_s.SetBranchAddress("pz",pz);
  chain_s.SetBranchAddress("E",E);
  
  chain_b.SetBranchAddress("nparticle",&nparticle);
  chain_b.SetBranchAddress("weight2",&weight);
  chain_b.SetBranchAddress("kf",kf);
  chain_b.SetBranchAddress("px",px);
  chain_b.SetBranchAddress("py",py);
  chain_b.SetBranchAddress("pz",pz);
  chain_b.SetBranchAddress("E",E);
  //==============================================
  //List of pts to iterate over                                                 
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
  //=============================================
  //Iteration over the pt range 
  for (int i=0; i<9; ++i){
    //Create outfile 
    TString outname = "n_18_" + num[i] + ".root"; 
    TFile* out = TFile::Open(outname, "RECREATE"); 
    cout<<"Writing "<<outname<<endl; 

    //Set Branches 
    TTree* s_train = new TTree("sTrain", "Signal"); 
    TTree* b_train = new TTree("bTrain", "Bckground");
    TTree* s_test = new TTree("sTest", "Signal"); 
    TTree* b_test = new TTree("bTest", "Bckground");

    s_train->SetDirectory(out);
    b_train->SetDirectory(out);
    s_test->SetDirectory(out);
    b_test->SetDirectory(out);
  
    //Output
    s_train->Branch("s",&s); 
    s_train->Branch("yydr",&yydr); 
    s_train->Branch("y1dr",&y1dr); 
    s_train->Branch("y2dr",&y2dr); 
    s_train->Branch("ptratio",&ptratio); 
    s_train->Branch("y1y",&y1y); 
    s_train->Branch("y2y",&y2y); 
    s_train->Branch("yyy",&yyy); 
    s_train->Branch("jety",&jety); 
    s_train->Branch("yydy",&yydy); 
    s_train->Branch("y1E",&y1E); 
    s_train->Branch("y2E",&y2E); 
    s_train->Branch("yyE",&yyE); 
    s_train->Branch("jetE",&jetE); 
    s_train->Branch("y1pt",&y1pt); 
    s_train->Branch("y2pt",&y2pt); 
    s_train->Branch("yypt",&yypt); 
    s_train->Branch("costhet", &costhet); 
    s_train->Branch("weight",&weight); 
  
    b_train->Branch("s",&s); 
    b_train->Branch("yydr",&yydr); 
    b_train->Branch("y1dr",&y1dr); 
    b_train->Branch("y2dr",&y2dr); 
    b_train->Branch("ptratio",&ptratio); 
    b_train->Branch("y1y",&y1y); 
    b_train->Branch("y2y",&y2y); 
    b_train->Branch("yyy",&yyy); 
    b_train->Branch("jety",&jety); 
    b_train->Branch("yydy",&yydy); 
    b_train->Branch("y1E",&y1E); 
    b_train->Branch("y2E",&y2E); 
    b_train->Branch("yyE",&yyE); 
    b_train->Branch("jetE",&jetE); 
    b_train->Branch("y1pt",&y1pt); 
    b_train->Branch("y2pt",&y2pt); 
    b_train->Branch("yypt",&yypt); 
    b_train->Branch("costhet", &costhet); 
    b_train->Branch("weight",&weight); 
  
    s_test->Branch("s",&s); 
    s_test->Branch("yydr",&yydr); 
    s_test->Branch("y1dr",&y1dr); 
    s_test->Branch("y2dr",&y2dr); 
    s_test->Branch("ptratio",&ptratio); 
    s_test->Branch("y1y",&y1y); 
    s_test->Branch("y2y",&y2y); 
    s_test->Branch("yyy",&yyy); 
    s_test->Branch("jety",&jety); 
    s_test->Branch("yydy",&yydy); 
    s_test->Branch("y1E",&y1E); 
    s_test->Branch("y2E",&y2E); 
    s_test->Branch("yyE",&yyE); 
    s_test->Branch("jetE",&jetE); 
    s_test->Branch("y1pt",&y1pt); 
    s_test->Branch("y2pt",&y2pt); 
    s_test->Branch("yypt",&yypt); 
    s_test->Branch("costhet", &costhet); 
    s_test->Branch("weight",&weight); 
  
    b_test->Branch("s",&s); 
    b_test->Branch("yydr",&yydr); 
    b_test->Branch("y1dr",&y1dr); 
    b_test->Branch("y2dr",&y2dr); 
    b_test->Branch("ptratio",&ptratio); 
    b_test->Branch("y1y",&y1y); 
    b_test->Branch("y2y",&y2y); 
    b_test->Branch("yyy",&yyy); 
    b_test->Branch("jety",&jety); 
    b_test->Branch("yydy",&yydy); 
    b_test->Branch("y1E",&y1E); 
    b_test->Branch("y2E",&y2E); 
    b_test->Branch("yyE",&yyE); 
    b_test->Branch("jetE",&jetE); 
    b_test->Branch("y1pt",&y1pt); 
    b_test->Branch("y2pt",&y2pt); 
    b_test->Branch("yypt",&yypt); 
    b_test->Branch("costhet", &costhet); 
    b_test->Branch("weight",&weight);

    cout<<"Branches added"<<endl; 

    //=================================================
    const Long64_t entries_s = chain_s.GetEntries(); 
    //=================================================
    for (long j=0; j<entries_s; ++j){
      chain_s.GetEntry(j); 
      TLorentzVector higgs, y1, y2, jet, yy; 
      vector<TLorentzVector> jets, photons; 

      for (long k=0; k<nparticle; ++k){
        if (kf[k]==25){
          higgs.SetPxPyPzE(px[k],py[k],pz[k],E[k]);
        }
        else{jets.emplace_back(px[k],py[k],pz[k],E[k]);}

      }
      pair<TLorentzVector, TLorentzVector> diphoton = Hdecay(higgs);                            
      if (diphoton.first.Pt()>diphoton.second.Pt()){
        y1 = diphoton.first;
        y2 = diphoton.second;
      }                                                                           
      else{                                                                       
        y1 = diphoton.second;                                                
        y2 = diphoton.first;                                                 
      } 
      yy = higgs;
      
      sort(jets.begin(), jets.end(), sort_pt); 
      jet = jets[0]; 


      //Cuts
      //Mass                                                             
      bool select = ((yy.M()>121.)&&(yy.M()<129.)); 
      //Rapidity
      select &= (abs(y2.Rapidity())<2.4);                                    
      select &= (abs(y1.Rapidity())<2.4);                                    
      //PseudoRapidity Cut                                                        
      select &= (abs(y1.PseudoRapidity())<2.37);                             
      select &= !(1.37<abs(y1.PseudoRapidity()) && abs(y1.PseudoRapidity())<1.52);
      select &= (abs(y2.PseudoRapidity())<2.37);                             
      select &= !(1.37<abs(y2.PseudoRapidity()) && abs(y2.PseudoRapidity())<1.52);
      //Pt Cut                                                                    
      select &= (y1.Pt()>.35*yy.M());                                     
      select &= (y2.Pt()>.25*yy.M());                                     
      //Delta R                                                                   
      select &= (y2.DeltaR(jet)>.4);                                         
      //Jet Cuts                                                                  
      select &= (jet.Pt()>30);                                                    
      select &= (jet.Rapidity()<4.4);

      //Diphoton Cuts
      if (i==0){select &= (yy.Pt()<50.);}                                       
      if (i==1){select &= (yy.Pt()>50. && yy.Pt()<100.);}                       
      if (i==2){select &= (yy.Pt()>100. && yy.Pt()<150);}                       
      if (i==3){select &= (yy.Pt()>150. && yy.Pt()<200);}                       
      if (i==4){select &= (yy.Pt()>200. && yy.Pt()<250);}                       
      if (i==5){select &= (yy.Pt()>250. && yy.Pt()<300);}                       
      if (i==6){select &= (yy.Pt()>300. && yy.Pt()<350);}                       
      if (i==7){select &= (yy.Pt()>350. && yy.Pt()<400);}                       
      if (i==8){select &= (yy.Pt()>400);} 


      if (select){
        s = (yy+jet).M();
        yydr = abs(y1.DeltaR(y2));
        y1dr = abs(y1.DeltaR(jet));
        y2dr = abs(y2.DeltaR(jet));
        ptratio = abs(y1.Pt())/abs(y2.Pt());
        y1y = abs(y1.Rapidity());
        y2y = abs(y2.Rapidity());
        jety = abs(jet.Rapidity());
        yydy = abs(y1.Rapidity()-y2.Rapidity());
        yyE = abs(yy.E());
        y1E = abs(y1.E());
        y2E = abs(y2.E());
        jetE = abs(jet.E());
        yypt = abs(yy.Pt());
        y1pt = abs(y1.Pt());
        y2pt = abs(y2.Pt());
        yyy = abs(yy.Rapidity());                                               
        costhet = (sinh(abs(y1.PseudoRapidity()-y2.PseudoRapidity()))*2*y1.Pt()*y2.Pt())/(sqrt(1.+pow((yy.Pt()/yy.M()),2.))*pow((yy.M()),2.));
        
        weight = weight; 


        //========================================
        TRandom3 *r = new TRandom3(std::chrono::system_clock::now().time_since_epoch().count());
        double ran = r->Rndm();
        const bool tts = (ran<.5); 

        if (tts){s_train->Fill();}
        else{s_test->Fill();} 
        }
      }
    //=================================================
    const Long64_t entries_b = chain_b.GetEntries(); 
    //=================================================
    for (long j=0; j<entries_b; ++j){
      chain_b.GetEntry(j); 
      TLorentzVector higgs, y1, y2, jet, yy; 
      vector<TLorentzVector> jets, photons; 

      for (long k=0; k<nparticle; ++k){
        if (kf[k]==22){
          photons.emplace_back(px[k],py[k],pz[k],E[k]);
        }
        else{jets.emplace_back(px[k],py[k],pz[k],E[k]);}

      }
      sort(photons.begin(), photons.end(),sort_pt); 
      y1 = photons[0]; 
      y2 = photons[1];    
      yy = y1 + y2; 

      sort(jets.begin(), jets.end(), sort_pt); 
      jet = jets[0]; 


      //Cuts
      //Mass                                                             
      bool select = ((yy.M()>121.)&&(yy.M()<129.)); 
      //Rapidity
      select &= (abs(y2.Rapidity())<2.4);                                    
      select &= (abs(y1.Rapidity())<2.4);                                    
      //PseudoRapidity Cut                                                        
      select &= (abs(y1.PseudoRapidity())<2.37);                             
      select &= !(1.37<abs(y1.PseudoRapidity()) && abs(y1.PseudoRapidity())<1.52);
      select &= (abs(y2.PseudoRapidity())<2.37);                             
      select &= !(1.37<abs(y2.PseudoRapidity()) && abs(y2.PseudoRapidity())<1.52);
      //Pt Cut                                                                    
      select &= (y1.Pt()>.35*yy.M());                                     
      select &= (y2.Pt()>.25*yy.M());                                     
      //Delta R                                                                   
      select &= (y2.DeltaR(jet)>.4);                                         
      //Jet Cuts                                                                  
      select &= (jet.Pt()>30);                                                    
      select &= (jet.Rapidity()<4.4);

      //Diphoton Cuts
      if (i==0){select &= (yy.Pt()<50.);}                                       
      if (i==1){select &= (yy.Pt()>50. && yy.Pt()<100.);}                       
      if (i==2){select &= (yy.Pt()>100. && yy.Pt()<150);}                       
      if (i==3){select &= (yy.Pt()>150. && yy.Pt()<200);}                       
      if (i==4){select &= (yy.Pt()>200. && yy.Pt()<250);}                       
      if (i==5){select &= (yy.Pt()>250. && yy.Pt()<300);}                       
      if (i==6){select &= (yy.Pt()>300. && yy.Pt()<350);}                       
      if (i==7){select &= (yy.Pt()>350. && yy.Pt()<400);}                       
      if (i==8){select &= (yy.Pt()>400);} 


      if (select){
        s = (yy+jet).M();
        yydr = abs(y1.DeltaR(y2));
        y1dr = abs(y1.DeltaR(jet));
        y2dr = abs(y2.DeltaR(jet));
        ptratio = abs(y1.Pt())/abs(y2.Pt());
        y1y = abs(y1.Rapidity());
        y2y = abs(y2.Rapidity());
        jety = abs(jet.Rapidity());
        yydy = abs(y1.Rapidity()-y2.Rapidity());
        yyE = abs(yy.E());
        y1E = abs(y1.E());
        y2E = abs(y2.E());
        jetE = abs(jet.E());
        yypt = abs(yy.Pt());
        y1pt = abs(y1.Pt());
        y2pt = abs(y2.Pt());
        yyy = abs(yy.Rapidity());                                               
        costhet = (sinh(abs(y1.PseudoRapidity()-y2.PseudoRapidity()))*2*y1.Pt()*y2.Pt())/(sqrt(1.+pow((yy.Pt()/yy.M()),2.))*pow((yy.M()),2.));
        
        weight = weight; 


        //========================================
        TRandom3 *r = new TRandom3(std::chrono::system_clock::now().time_since_epoch().count());
        double ran = r->Rndm();
        const bool tts = (ran<.5); 

        if (tts){s_train->Fill();}
        else{s_test->Fill();} 
      }
    }
    out->Write(); 
    out->Close(); 
  }
}
