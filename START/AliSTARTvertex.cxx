/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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
Revision 1.6  2001/10/19 05:29:38  alla
bug in meduim fixed

Revision 1.5  2001/07/27 13:03:12  hristov
Default Branch split level set to 99

Revision 1.4  2000/12/22 16:17:15  hristov
Updated  START code from Alla

Revision 1.3  2000/10/02 21:28:13  fca
Removal of useless dependecies via forward declarations
 
Revision 1.2  2000/07/13 16:41:29  fca
New START corrected for coding conventions

Revision 1.1  2000/03/24 17:46:58  alla
Vertex reconstruction

*/ 
#include <TObject.h>
#include "AliSTARTvertex.h"
#include "AliSTARTdigit.h"
#include "AliSTARThit.h"
#include "AliSTART.h"
#include "AliRun.h"
#include "AliMC.h"

//#include "TTree.h"
#include "TDirectory.h"

ClassImp(AliSTARTvertex)

AliSTARTvertex::AliSTARTvertex( Int_t * Zposit)
{
  //
  //     The creator for the AliSTARTvertex class. This routine fills the
  // AliSTARTvertex data members from the array vertex.
  // The order of the elements in the vertex array are
  //  fZposition = vertex[0],
  //

  Zposit = &fZposition ;
}

void AliSTARTvertex::Reconstruct(Int_t evNumber=1) 
{
  /***************************************************
  Resonstruct digits to vertex position
  ****************************************************/

  Int_t timediff;
  Float_t timePs;
  char nameTD[8],nameTR[8];

  TBranch *bRec=0;
  TBranch *bd;
  AliSTARTdigit *digits;
  AliSTARTvertex *vertex;
 
  Int_t buffersize=256;
 
  // TParticle *particle;
  digits = new AliSTARTdigit();
  vertex = new AliSTARTvertex();

 // Event ------------------------- LOOP  
   
  sprintf(nameTD,"TreeD%d",evNumber);
  printf("%s\n",nameTD);
  TTree *td = (TTree*)gDirectory->Get(nameTD);
  bd = td->GetBranch("START");
  bd->SetAddress(&digits);
  bd->GetEvent(0);
  sprintf(nameTR,"TreeR%d",evNumber);
  TTree *tr = new TTree(nameTR,"START");
  bRec = tr->Branch("START","AliSTARTvertex",&vertex,buffersize);
  if(digits->GetTime()!=999999)
    {
      timediff=digits->GetTime();     //time in number of channels
      timePs=(timediff-128)*10.;       // time in Ps channel_width =10ps
      Float_t c = 299792458/1.e9;  //speed of light cm/ps
      //Float_t c = 0.3;  //speed of light mm/ps
      Float_t Zposit=timePs*c;// for 0 vertex
      vertex->Set((Int_t) Zposit);
      tr->Fill();
      tr->Write();
      }

}




