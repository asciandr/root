/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RooChebychev.h,v 1.6 2007/05/11 09:13:07 verkerke Exp $
 * Authors:                                                                  *
 *   GR, Gerhard Raven,   UC San Diego, Gerhard.Raven@slac.stanford.edu
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_CHEBYCHEV
#define ROO_CHEBYCHEV

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"

class RooRealVar;
class RooArgList ;

class RooChebychev : public RooAbsPdf {
public:

  RooChebychev() ;
  RooChebychev(const char *name, const char *title,
               RooAbsReal& _x, const RooArgList& _coefList, bool listIncludesC0 = false);

  RooChebychev(const RooChebychev& other, const char* name = 0);
  virtual TObject* clone(const char* newname) const { return new RooChebychev(*this, newname); }
  inline virtual ~RooChebychev() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;
  RooSpan<double> evaluateBatch(std::size_t begin, std::size_t batchSize) const;

  virtual void selectNormalizationRange(const char* rangeName=0, Bool_t force=kFALSE) ;
  
private:

  RooRealProxy _x;
  RooListProxy _coefList ;
  mutable TNamed* _refRangeName ; 
  bool _coefListIncludesC0{false};

  Double_t evaluate() const;
  Double_t evalAnaInt(const Double_t a, const Double_t b) const;

  ClassDef(RooChebychev,3) // Chebychev polynomial PDF
};

#endif
