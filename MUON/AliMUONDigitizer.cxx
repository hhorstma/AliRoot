/**************************************************************************
 * Copyright(c) 1998-2000, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/*
$Log$
Revision 1.4  2001/10/18 14:44:09  jchudoba
Define constant MAXTRACKS for maximum number of tracks associated with 1 digit

Revision 1.3  2001/10/04 20:01:54  jchudoba
changes for TTask implementation, some other small editing

Revision 1.2  2001/07/28 10:46:04  hristov
AliRunDigitizer.h included; typos corrected

Revision 1.1  2001/07/27 15:41:01  jchudoba
merging/digitization classes

*/
#include <TTree.h> 
#include <TVector.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TDirectory.h>
#include <iostream.h>

#include "AliMUONDigitizer.h"
#include "AliMUONConstants.h"
#include "AliMUONChamber.h"
#include "AliHitMap.h"
#include "AliMUONHitMapA1.h"
#include "AliMUON.h"
#include "AliMUONHit.h"
#include "AliMUONPadHit.h"
#include "AliMUONDigit.h"
#include "AliMUONTransientDigit.h"
#include "AliRun.h"
#include "AliPDG.h"
#include "AliRunDigitizer.h"

ClassImp(AliMUONDigitizer)

//___________________________________________
AliMUONDigitizer::AliMUONDigitizer() :AliDigitizer()
{
// Default ctor - don't use it
  ;
}

//___________________________________________
AliMUONDigitizer::AliMUONDigitizer(AliRunDigitizer* manager) 
    :AliDigitizer(manager)
{
// ctor which should be used
  fHitMap  = 0;
  fTDList  = 0;
  fHits    = 0;
  fPadHits = 0;
  fTrList  = 0;
  fAddress = 0;
  fDebug   = 0; 
  if (GetDebug()>2) 
    cerr<<"AliMUONDigitizer::AliMUONDigitizer"
	<<"(AliRunDigitizer* manager) was processed"<<endl;
}

//------------------------------------------------------------------------
AliMUONDigitizer::~AliMUONDigitizer()
{
// Destructor
  if (fHits)       delete fHits;
  if (fPadHits)    delete fPadHits;
  if (fHitMap)     delete fHitMap;
  if (fTDList)     delete fTDList;
  if (fTrList)     delete fTrList;
  if (fAddress)    delete fAddress; 
}

//------------------------------------------------------------------------
Bool_t AliMUONDigitizer::Exists(const AliMUONPadHit *mergable)
{
  AliMUONPadHit *padhit = (AliMUONPadHit*) mergable;
  return (fHitMap[fNch]->TestHit(padhit->PadX(),padhit->PadY()));
}

//------------------------------------------------------------------------
void AliMUONDigitizer::Update(AliMUONPadHit *mergable)
{
  AliMUONPadHit *padhit = (AliMUONPadHit*) mergable;    
  AliMUONTransientDigit* pdigit;
  Int_t ipx      = padhit->PadX();        // pad number on X
  Int_t ipy      = padhit->PadY();        // pad number on Y
  Int_t iqpad    = Int_t(padhit->QPad()); // charge per pad

  pdigit = (AliMUONTransientDigit*) fHitMap[fNch]->GetHit(ipx, ipy);
  // update charge
  //
  (*pdigit).AddSignal(iqpad);
  (*pdigit).AddPhysicsSignal(iqpad);		
  // update list of tracks
  //
  TObjArray* fTrList = (TObjArray*)pdigit->TrackList();
  Int_t lastEntry = fTrList->GetLast();
  TVector *pTrack = (TVector*)fTrList->At(lastEntry);
  TVector &ptrk   = *pTrack;
  TVector &trinfo = *((TVector*) (*fAddress)[fCountadr-1]);
  Int_t lastTrack = Int_t(ptrk(0));

  if (trinfo(0) != kBgTag) {
    if (lastTrack == fTrack) {
      Int_t lastCharge = Int_t(ptrk(1));
      lastCharge += iqpad;
      fTrList->RemoveAt(lastEntry);
      trinfo(1) = lastCharge;
      fTrList->AddAt(&trinfo,lastEntry);
    } else {
      fTrList->Add(&trinfo);
    }
  } else {
    if (lastTrack != -1) fTrList->Add(&trinfo);
  }
}

//------------------------------------------------------------------------
void AliMUONDigitizer::CreateNew(AliMUONPadHit *mergable)
{
// Create new AliMUONTransientDigit and add it to the fTDList

  AliMUONPadHit *padhit = (AliMUONPadHit*) mergable;    
  AliMUONTransientDigit* pdigit;

  Int_t ipx      = padhit->PadX();       // pad number on X
  Int_t ipy      = padhit->PadY();       // pad number on Y
  fTDList->AddAtAndExpand(
    new AliMUONTransientDigit(fNch,fDigits),fCounter);
  fHitMap[fNch]->SetHit(ipx, ipy, fCounter);
  fCounter++;
  pdigit = (AliMUONTransientDigit*)fTDList->At(fTDList->GetLast());
  // list of tracks
  TObjArray *fTrList = (TObjArray*)pdigit->TrackList();
  TVector &trinfo    = *((TVector*) (*fAddress)[fCountadr-1]);
  fTrList->Add(&trinfo);
}


//------------------------------------------------------------------------
Bool_t AliMUONDigitizer::Init()
{
// Initialization 
    
  fHits     = new TClonesArray("AliMUONHit",1000);
  fPadHits  = new TClonesArray("AliMUONPadHit",1000);
  return kTRUE;
}


//------------------------------------------------------------------------
//void AliMUONDigitizer::Digitize()
void AliMUONDigitizer::Exec(Option_t* option)
{

  TString optionString = option;
  if (optionString.Data() == "deb") {
    cout<<"AliMUONDigitizer::Exec: called with option deb "<<endl;
    fDebug = 3;
  }
  AliMUONChamber*   iChamber;
  AliSegmentation*  segmentation;
  
  if (GetDebug()>2) cerr<<"   AliMUONDigitizer::Digitize() starts"<<endl;
  fTDList = new TObjArray;
  if(!fAddress) fAddress = new TClonesArray("TVector",10000);

  AliMUON *pMUON  = (AliMUON *) gAlice->GetModule("MUON");
  if (!pMUON) {
    cerr<<"AliMUONDigitizer::Digitize Error:"
	<<" module MUON not found in the input file"<<endl;
    return;
  }
  pMUON->MakeBranchInTreeD(fManager->GetTreeD());
  fHitMap= new AliHitMap* [AliMUONConstants::NCh()];
  for (Int_t i = 0; i < AliMUONConstants::NCh(); i++) {fHitMap[i] = 0;}

  //
  // loop over cathodes
  //

  fCountadr = 0;
  for (int icat = 0; icat < 2; icat++) { 
    fCounter = 0;
    Int_t * nmuon = new Int_t [AliMUONConstants::NCh()];
    for (Int_t i = 0; i < AliMUONConstants::NCh(); i++) {
      iChamber = &(pMUON->Chamber(i));
//      if (!(iChamber->Nsec() == 1 && icat == 1)) {
	segmentation = iChamber->SegmentationModel(icat+1);
	fHitMap[i] = new AliMUONHitMapA1(segmentation, fTDList);
	nmuon[i] = 0;
//      }
    }


// Loop over files to digitize
	
    for (Int_t inputFile=0; inputFile<fManager->GetNinputs();
	 inputFile++) {
// Connect MUON branches

      TBranch *branchHits = 0;
      TBranch *branchPadHits = 0;
      TTree *treeH = fManager->GetInputTreeH(inputFile);
      if (GetDebug()>2) {
	cerr<<"   inputFile , cathode = "<<inputFile<<" "
	    <<icat<<endl;
	cerr<<"   treeH, fHits "<<treeH<<" "<<fHits<<endl;
      }
      if (treeH && fHits) {
	branchHits = treeH->GetBranch("MUON");
	if (branchHits) {
	  fHits->Clear();
	  branchHits->SetAddress(&fHits);
	}
	else
	  Error("Digitize","branch MUON was not found");
      }
      if (GetDebug()>2) cerr<<"   branchHits = "<<branchHits<<endl;

      if (treeH && fPadHits) {
	branchPadHits = treeH->GetBranch("MUONCluster");
	if (branchPadHits) 
	  branchPadHits->SetAddress(&fPadHits);
	else
	  Error("Digitize","branch MUONCluster was not found");
      }
      if (GetDebug()>2) cerr<<"   branchPadHits = "<<branchPadHits<<endl;

//
//   Loop over tracks
//

      Int_t ntracks = (Int_t) treeH->GetEntries();

      for (fTrack = 0; fTrack < ntracks; fTrack++) {
	if (GetDebug()>2) cerr<<"   fTrack = "<<fTrack<<endl;
	fHits->Clear();
	fPadHits->Clear();
	branchHits->GetEntry(fTrack);
	branchPadHits->GetEntry(fTrack);
	
//
//   Loop over hits

	AliMUONHit* mHit;
	for(int i = 0; i < fHits->GetEntriesFast(); ++i) 
	{	
	  mHit   = (AliMUONHit*) (*fHits)[i];
	  fNch = mHit->Chamber()-1;  // chamber number
	  iChamber = &(pMUON->Chamber(fNch));
	  if (fNch > AliMUONConstants::NCh()-1) {
	    cerr<<"AliMUONDigitizer: ERROR: "
		<<"fNch > AliMUONConstants::NCh()-1, fNch, NCh(): "
		<<fNch<<", "<< AliMUONConstants::NCh()<<endl;
	    return;
	  }
//
// Loop over pad hits
	  for (AliMUONPadHit* mPad =
		 (AliMUONPadHit*)pMUON->FirstPad(mHit,fPadHits);
	       mPad;
	       mPad = (AliMUONPadHit*)pMUON->NextPad(fPadHits))
	  {
	    Int_t cathode  = mPad->Cathode();      // cathode number
	    Int_t ipx      = mPad->PadX();         // pad number on X
	    Int_t ipy      = mPad->PadY();         // pad number on Y
	    Int_t iqpad    = Int_t(mPad->QPad());  // charge per pad
	    if (cathode != (icat+1)) continue;

	    new((*fAddress)[fCountadr++]) TVector(2);

	    TVector &trinfo = *((TVector*) (*fAddress)[fCountadr-1]);
	    Int_t mask = fManager->GetMask(inputFile);
	    trinfo(0) = (Float_t)(fTrack + mask);  // tag background
//		    trinfo(0) = (Float_t)fTrack;
	    if (inputFile == 0) {
	      trinfo(1) = (Float_t)iqpad;
	    } else {
	      trinfo(1) = kBgTag;
	    }
	    fDigits[0] = ipx;
	    fDigits[1] = ipy;
	    fDigits[2] = icat;
	    fDigits[3] = iqpad;
	    if (inputFile == 0) {
	      fDigits[4] = iqpad;
	    } else {
	      fDigits[4] = 0;
	    }
	    if (mHit->Particle() == kMuonPlus ||
		mHit->Particle() == kMuonMinus) {
	      fDigits[5] = (mPad->HitNumber()) + mask; 
	    } else fDigits[5] = -1;

	    // build the list of fired pads and update the info, 
	    // fDigits is input for Update(mPad)

	    if (!Exists(mPad)) {
	      CreateNew(mPad);
	    } else {
	      Update(mPad);
	    } //  end if Exists(mPad)
	  } //end loop over clusters
	} // hit loop
      } // track loop
    } // end file loop
    if (GetDebug()>2) cerr<<"END OF FILE LOOP"<<endl;

    Int_t tracks[MAXTRACKS];
    Int_t charges[MAXTRACKS];
    Int_t nentries = fTDList->GetEntriesFast();
	
    for (Int_t nent = 0; nent < nentries; nent++) {
      AliMUONTransientDigit *address = (AliMUONTransientDigit*)fTDList->At(nent);
      if (address == 0) continue; 
      Int_t ich = address->Chamber();
      Int_t   q = address->Signal(); 
      iChamber = &(pMUON->Chamber(ich));
//
//  Digit Response (noise, threshold, saturation, ...)
      AliMUONResponse * response = iChamber->ResponseModel();
      q = response->DigitResponse(q);
	    
      if (!q) continue;
	    
      fDigits[0] = address->PadX();
      fDigits[1] = address->PadY();
      fDigits[2] = address->Cathode();
      fDigits[3] = q;
      fDigits[4] = address->Physics();
      fDigits[5] = address->Hit();
	    
      TObjArray* fTrList = (TObjArray*)address->TrackList();
      Int_t nptracks = fTrList->GetEntriesFast();

      // this was changed to accomodate the real number of tracks

      if (nptracks > MAXTRACKS) {
	if (GetDebug() >0) {
	  cerr<<"AliMUONDigitizer: nptracks > 10 "<<nptracks;
	  cerr<<"reset to max value "<<MAXTRACKS<<endl;
	}
	nptracks = MAXTRACKS;
      }
      if (nptracks > 2 && GetDebug() >2) {
	cerr<<"AliMUONDigitizer: nptracks > 2 "<<nptracks<<endl;
	printf("cat,ich,ix,iy,q %d %d %d %d %d \n",icat,ich,fDigits[0],fDigits[1],q);
      }
      for (Int_t tr = 0; tr < nptracks; tr++) {
	TVector *ppP = (TVector*)fTrList->At(tr);
	if(!ppP ) {
	  cerr<<"Error: ppP = "<<ppP<<endl;
	  return;
	}
	TVector &pp  = *ppP;
	tracks[tr]   = Int_t(pp(0));
	charges[tr]  = Int_t(pp(1));
      }      //end loop over list of tracks for one pad
      // Sort list of tracks according to charge
      if (nptracks > 1) {
	SortTracks(tracks,charges,nptracks);
      }
      if (nptracks < MAXTRACKS ) {
	for (Int_t i = nptracks; i < MAXTRACKS; i++) {
	  tracks[i]  = 0;
	  charges[i] = 0;
	}
      }
	    
      // fill digits
      pMUON->AddDigits(ich,tracks,charges,fDigits);
      // delete fTrList;
    }

    fManager->GetTreeD()->Fill();

    pMUON->ResetDigits();  //
    fTDList->Clear();

	
    for(Int_t ii = 0; ii < AliMUONConstants::NCh(); ++ii) {
      if (fHitMap[ii]) {
	delete fHitMap[ii];
	fHitMap[ii] = 0;
      }
    }
    delete [] nmuon;    
  } //end loop over cathodes
  if (GetDebug()>2) 
    cerr<<"AliMUONDigitizer::Exec: writing the TreeD: "
	<<fManager->GetTreeD()->GetName()<<endl;
  fManager->GetTreeD()->Write(0,TObject::kOverwrite);
  delete [] fHitMap;
  delete fTDList;
    
  if (fAddress)    fAddress->Delete();
  if (fHits)    fHits->Delete();
  if (fPadHits) fPadHits->Delete();
}


//------------------------------------------------------------------------
void AliMUONDigitizer::SortTracks(Int_t *tracks,Int_t *charges,Int_t ntr)
{
  //
  // Sort the list of tracks contributing to a given digit
  // Only the 3 most significant tracks are acctually sorted
  //
  
  //
  //  Loop over signals, only 3 times
  //
  
  Int_t qmax;
  Int_t jmax;
  Int_t idx[3] = {-2,-2,-2};
  Int_t jch[3] = {-2,-2,-2};
  Int_t jtr[3] = {-2,-2,-2};
  Int_t i,j,imax;
  
  if (ntr<3) imax=ntr;
  else imax=3;
  for(i=0;i<imax;i++){
    qmax=0;
    jmax=0;
    
    for(j=0;j<ntr;j++){
      
      if((i == 1 && j == idx[i-1]) 
	 ||(i == 2 && (j == idx[i-1] || j == idx[i-2]))) continue;
      
      if(charges[j] > qmax) {
	qmax = charges[j];
	jmax=j;
      }       
    } 
    
    if(qmax > 0) {
      idx[i]=jmax;
      jch[i]=charges[jmax]; 
      jtr[i]=tracks[jmax]; 
    }
    
  } 
  
  for(i=0;i<3;i++){
    if (jtr[i] == -2) {
      charges[i]=0;
      tracks[i]=0;
    } else {
      charges[i]=jch[i];
      tracks[i]=jtr[i];
    }
  }
}
