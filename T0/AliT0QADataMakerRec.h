#ifndef AliT0QADataMakerRec_H
#define AliT0QADataMakerRec_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//
// Alla.Maevskaya@cern.ch
// 


// --- ROOT system ---



// --- Standard library ---
// --- AliRoot header files ---

#include "AliQADataMakerRec.h"
#include "AliT0RecoParam.h" 

class AliT0QADataMakerRec: public AliQADataMakerRec {

public:
  AliT0QADataMakerRec() ;          // ctor
  AliT0QADataMakerRec(const AliT0QADataMakerRec& qadm) ;   
  AliT0QADataMakerRec& operator = (const AliT0QADataMakerRec& qadm) ;
  virtual ~AliT0QADataMakerRec(); // dtor

private:
  virtual void   InitRaws() ;    //book Digit QA histo
  virtual void   InitRecPoints();  //book cluster QA histo
  virtual void   InitDigits() ; 
  virtual void   InitESDs() ;      //book ESD QA histo 
  virtual void   MakeRaws(AliRawReader* rawReader) ;
  virtual void   MakeRecPoints(TTree * clusters)    ;  //Fill cluster QA histo
  virtual void   MakeDigits() {;} 
  virtual void   MakeDigits(TTree * digTree);
  virtual void   MakeESDs(AliESDEvent * esd) ;         //Fill hit QA histo
  virtual void   EndOfDetectorCycle(AliQAv1::TASKINDEX_t, TObjArray ** list) ;
  virtual void   StartOfDetectorCycle() ;

  const AliT0RecoParam* GetRecoParam() { return dynamic_cast<const AliT0RecoParam*>(fRecoParam);}

  // RS Commented by Ruben, read below:
  /*
  // RS: Don't use custom counters, they create problems with trigger cloning
  //     Use instead framework counters, incremented in the end of this routine
  // RS: There is some inconsistency here: the separation of physics and calib. events/histos is done by
  // fEventSpecie. Why do we book separate histos on different slots for calib and physics ? 
  // I am changing this in such way that we don't need local counters like fNumTriggers (the corresponding
  // histos now incremented in the MakeRaws, and for the normalization I will use the framework's counters
  // AliQADataMaker::GetEvCountCycle(...), AliQADataMaker::GetEvCountTotal(...)
  //
  // I think the histos xx+250 should be suppressed (the xx calib histos of specie==calibration will be 
  // used automatically)
  

  Int_t fNumTriggers[6];  //number of trigger signals;
  Int_t fNumTriggersCal[6];  //number of calibration  trigger signals;

  Int_t fnEventCal; 
  Int_t fnEventPhys; 
  Int_t feffC[24];
  Int_t feffPhysC[24]; 
  Int_t feffA[24]; 
  Int_t feffPhysA[24];
  Int_t feffqtc[24]; 
  Int_t feffqtcPhys[24];
  Float_t fTrEffCal[6];
  Float_t fTrEffPhys[6];
  TH1F*  fhTimeDiff[24];
*/
  Int_t fMeans[24];

  ClassDef(AliT0QADataMakerRec,7)  // description 

};

#endif // AliT0QADataMakerRec_H
