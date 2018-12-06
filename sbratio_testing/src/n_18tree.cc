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

Float_t bg_w = 3.0;   
Float_t sig_w = 1.0; 

using namespace std; 
Higgs2diphoton Hdecay; 
bool sort_pt (TLorentzVector i, TLorentzVector j){return (i.Pt()>j.Pt());} 

bool cuts(TLorentzVector yy, TLorentzVector y1, TLorentzVector y2, TLorentzVector jet){
  bool val = ((yy.M()>121.)&&(yy.M()<129.)); 
  //Rapidity
  val &= (abs(y2.Rapidity())<2.4);                                    
  val &= (abs(y1.Rapidity())<2.4);                                    
  //PseudoRapidity Cut                                                        
  val &= (abs(y1.PseudoRapidity())<2.37);                             
  val &= !(1.37<abs(y1.PseudoRapidity()) && abs(y1.PseudoRapidity())<1.52);
  val &= (abs(y2.PseudoRapidity())<2.37);                             
  val &= !(1.37<abs(y2.PseudoRapidity()) && abs(y2.PseudoRapidity())<1.52);
  //Pt Cut                                                                    
  val &= (y1.Pt()>.35*yy.M());                                     
  val &= (y2.Pt()>.25*yy.M());                                     
  //Delta R                                                                   
  val &= (y2.DeltaR(jet)>.4);                                         
  //Jet Cuts                                                                  
  val &= (jet.Pt()>30);                                                    
  val &= (jet.Rapidity()<4.4);
    
  return val; 
}

int main(){
  //===============================================
  //Chains
  TChain chain_s("t3"); 
  TChain chain_b("t3"); 

  //Sig
  chain_s.Add("$TMPDIR/H1.0j_GGFHT_B_6500_pt25.0_eta4.5_r100_100.root",0); 
  chain_s.Add("$TMPDIR/H1.0j_GGFHT_B_6500_pt25.0_eta4.5_r100_101.root",0); 
  
  //Bg 
  chain_b.Add("$TMPDIR/born1_1.root",0); 
  chain_b.Add("$TMPDIR/born1_2.root",0); 
  chain_b.Add("$TMPDIR/born1.root",0); 
  chain_b.Add("$TMPDIR/born2_1.root",0); 
  chain_b.Add("$TMPDIR/born2_2.root",0); 
  chain_b.Add("$TMPDIR/born2.root",0); 
  //===============================================
  //Variables for trees
  Float_t s, yydr, y1dr, y2dr, ptratio, y1y, y2y, jety; 
  Float_t yydy, y1E, y2E, yyE, jetE, y1pt, y2pt, yypt, yyy, costhet; 
  Float_t weights; 
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
  const vector<TString>
    num {"300","350","400","400p"};     
  //=============================================
  //Iteration over the pt range 
  for (int i=0; i<4; ++i){
    //Create outfile 
    TString outname = "n_18_" + num[i] + "_" + bg_w + ".root"; 
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
    //Gonna try something that might be dumb 
    vector<Float_t>
      vari{s,yydr,y1dr,y2dr,ptratio,y1y,y2y,yyy,jety,yydy,y1E,y2E,jetE,y1pt,y2pt,yypt,costhet,weights}; 
    
    const vector<TString>
      vari_s{"s","yydr","y1dr","y2dr","ptratio","y1y","y2y","yyy","jety",
        "yydy","y1E","y2E","jetE","y1pt","y2pt","yypt","costhet","weights"}; 
    
    for (int j=0; j<18; ++j){
      s_train->Branch(vari_s[j],&vari[j]); 
      b_train->Branch(vari_s[j],&vari[j]); 
      s_test->Branch(vari_s[j],&vari[j]); 
      b_test->Branch(vari_s[j],&vari[j]); 
    } 

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
/*
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
*/
      
      bool select = cuts(yy,y1,y2,jet); 
      
      //Diphoton Cuts
      if (i==0){select &= (yy.Pt()>250. && yy.Pt()<300);}                       
      if (i==1){select &= (yy.Pt()>300. && yy.Pt()<350);}                       
      if (i==2){select &= (yy.Pt()>350. && yy.Pt()<400);}                       
      if (i==3){select &= (yy.Pt()>400);} 


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
      if (i==0){select &= (yy.Pt()>250. && yy.Pt()<300);}                       
      if (i==1){select &= (yy.Pt()>300. && yy.Pt()<350);}                       
      if (i==2){select &= (yy.Pt()>350. && yy.Pt()<400);}                       
      if (i==3){select &= (yy.Pt()>400);} 


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
        
        weight =3*weight; 


        //========================================
        TRandom3 *r = new TRandom3(std::chrono::system_clock::now().time_since_epoch().count());
        double ran = r->Rndm();
        const bool tts = (ran<.5); 

        if (tts){b_train->Fill();}
        else{b_test->Fill();} 
      }
    }
    out->Write(); 
    out->Close(); 
  }
}
