#ifdef __CINT__ 
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ class Roo1DTable+ ;
#pragma link C++ class RooAbsArg- ;
#pragma link C++ class stack<RooAbsArg*,deque<RooAbsArg*> > ;
#pragma link C++ class RooRefArray- ;
#pragma read sourceClass="RooAbsArg" targetClass="RooAbsArg" version="[1-4]" source="TList _proxyList" target="_proxyList" \
    code="{ TIterator* iter = onfile._proxyList.MakeIterator() ; TObject* tmpObj ; while ((tmpObj = iter->Next())) { _proxyList.Add(tmpObj) ; } delete iter ; }" 
#pragma read sourceClass="RooAbsArg" targetClass="RooAbsArg" version="[5]" source="TRefArray _proxyList" target="_proxyList" \
  code="{ _proxyList.GetSize() ; if (onfile._proxyList.GetSize()>0) { RooAbsArg::_ioEvoList[newObj] = new TRefArray(onfile._proxyList) ; } }" 
#pragma read sourceClass="RooAbsArg" targetClass="RooAbsArg" version="[1-6]"\
  source="RooRefCountList _serverList" target="_serverList" \
  code="{ _serverList = RooFit::STLRefCountListHelpers::convert(onfile._serverList); }"
#pragma read sourceClass="RooAbsArg" targetClass="RooAbsArg" version="[1-6]"\
  source="RooRefCountList _clientList" target="_clientList" \
  code="{ _clientList = RooFit::STLRefCountListHelpers::convert(onfile._clientList); }"
#pragma read sourceClass="RooAbsArg" targetClass="RooAbsArg" version="[1-6]"\
  source="RooRefCountList _clientListValue" target="_clientListValue" \
  code="{ _clientListValue = RooFit::STLRefCountListHelpers::convert(onfile._clientListValue); }"
#pragma read sourceClass="RooAbsArg" targetClass="RooAbsArg" version="[1-6]"\
  source="RooRefCountList _clientListShape" target="_clientListShape" \
  code="{ _clientListShape = RooFit::STLRefCountListHelpers::convert(onfile._clientListShape); }"
#pragma link C++ class RooAbsBinning- ;
#pragma link C++ class RooAbsCategory+ ;
#pragma read sourceClass="RooAbsCategory" targetClass="RooAbsCategory" version="[1]" \
  source="TObjArray _types" target="_types" \
  code="{TObject* obj; TIterator* it = onfile._types.MakeIterator();\
         while ((obj=it->Next())) {\
           auto cat = dynamic_cast<const RooCatType*>(obj); assert(cat);\
           _types.push_back(new RooCatType(*cat)); }\
         delete it; }";
#pragma link C++ class RooAbsCategoryLValue+ ;
#pragma link C++ class RooAbsCollection+ ;
#pragma read sourceClass="RooAbsCollection" targetClass="RooAbsCollection" version="[1]" source="" target="_allRRV" code="{ _allRRV=kFALSE ; }"
#pragma read sourceClass="RooAbsCollection" targetClass="RooAbsCollection" version="[2]"\
    source="RooLinkedList _list" target="_list" code="{ RooFIter iter = onfile._list.fwdIterator(); RooAbsArg * theArg;\
    while ((theArg = iter.next())) {_list.push_back(theArg);} }"
#pragma link C++ class RooAbsData- ;
#pragma link C++ class RooAbsFunc+ ;
#pragma link C++ class RooAbsGenContext+ ;
#pragma link C++ class RooAbsTestStatistic+ ;
#pragma link C++ class RooAbsHiddenReal+ ;
#pragma link C++ class RooAbsIntegrator+ ;
#pragma link C++ class RooAbsLValue+ ;
#pragma link C++ class RooAbsMCStudyModule+ ;
#pragma link C++ class RooAbsOptTestStatistic+ ;
#pragma link C++ class RooAbsPdf+ ;
#pragma link C++ class RooAbsPdf::GenSpec+ ;
#pragma link C++ class RooAbsProxy+ ;
#pragma link C++ class RooAbsReal+ ;
#pragma link C++ class RooAbsRealLValue+ ;
#pragma link C++ class RooAbsRootFinder+ ;
#pragma link C++ class RooAbsString+ ;
#pragma link C++ class RooAcceptReject+ ;
#pragma link C++ class RooAddGenContext+ ;
#pragma link C++ class RooAddition+ ;
#pragma link C++ class RooAddModel+ ;
#pragma link C++ class RooAICRegistry+ ;
#pragma link C++ class RooArgList+ ;
#pragma link C++ class RooArgProxy+ ;
#pragma link C++ class RooArgSet+ ;
#pragma link C++ class RooBinnedGenContext+ ;
#pragma link C++ class RooBinning-;
#pragma link C++ class RooBrentRootFinder+ ;
#pragma link C++ class RooCategory- ;
#pragma link C++ class RooCategoryProxy+ ;
#pragma link C++ class RooCategorySharedProperties+ ;
#pragma link C++ class RooCatType+ ;
#pragma link C++ class RooChi2Var+ ;
#pragma link C++ class RooClassFactory+ ;
#pragma link C++ class RooCmdArg+ ;
#pragma link C++ class RooCmdConfig+ ;
#pragma link C++ class RooConstVar+ ;
#pragma link C++ class RooConvCoefVar+ ;
#pragma link C++ class RooConvGenContext+ ;
#pragma link C++ class RooConvIntegrandBinding+ ;
#pragma link C++ class RooCurve+ ;
#pragma link C++ class RooCustomizer+ ;
#pragma read sourceClass="RooDataHist" targetClass="RooDataHist" version="[3-4]" \
  source="Int_t _arrSize; Double_t* _wgt; Double_t* _errLo; Double_t* _errHi; Double_t* _sumw2; Double_t* _binv" \
  target="_wgt,_errLo,_errHi,_sumw2,_binv" \
  code="{ std::cout << \"_arrSize=\" << onfile._arrSize << std::endl; \
          std::cout << \"_wgt=\" << onfile._wgt << std::endl; \
          std::cout << \"_wgt[0]=\" << onfile._wgt[0] << std::endl; \
          _wgt.  assign(onfile._wgt,   onfile._wgt   + onfile._arrSize); \
          _errLo.assign(onfile._errLo, onfile._errLo + onfile._arrSize); \
          _errHi.assign(onfile._errHi, onfile._errHi + onfile._arrSize); \
          _sumw2.assign(onfile._sumw2, onfile._sumw2 + onfile._arrSize); \
          _binv. assign(onfile._binv,  onfile._binv  + onfile._arrSize); \
          std::cout << \"_wgt[0]=\" << onfile._wgt[0]  << '\t' << _wgt[0] << std::endl; \
          std::cout << \"_errLo[0]=\" << onfile._errLo[0] << '\t' << _errLo[0] << std::endl; \
          std::cout << \"_errHi[0]=\" << onfile._errHi[0] << '\t' << _errHi[0] << std::endl; \
          std::cout << \"_sumw2[0]=\" << onfile._sumw2[0] << '\t' << _sumw2[0] << std::endl; \
          std::cout << \"_binv[0]=\" << onfile._binv[0] << '\t' << _binv[0] << std::endl; \
          std::cout << \"Done writing.\" << std::endl;}";
#pragma link C++ class RooDataHist- ;
#pragma link C++ class RooDataProjBinding+ ;
#pragma link C++ class RooDataSet- ;
#pragma link C++ class RooDirItem+ ;
#pragma link C++ class RooDLLSignificanceMCSModule+ ;
#pragma link C++ class RooAbsAnaConvPdf+ ;
#pragma link C++ class RooAddPdf+ ;
#pragma link C++ class RooEfficiency+ ; 
#pragma link C++ class RooEffProd+ ;
#pragma link C++ class RooExtendPdf+ ;
#pragma link off class RooErrorHandler+ ;
#pragma link C++ class RooWrapperPdf+;
#endif
 

