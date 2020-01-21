/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id: RooRealProxy.h,v 1.23 2007/07/12 20:30:28 wouter Exp $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_TEMPLATE_PROXY
#define ROO_TEMPLATE_PROXY

#include "RooAbsReal.h"
#include "RooArgProxy.h"
#include "RooAbsRealLValue.h"
#include "RooAbsCategory.h"

/**
\class RooProxy
\ingroup Roofitcore

A RooProxy is used to hold references to other RooFit objects in an expression tree.
A `RooGaussian(..., x, mean, sigma)` can e.g. store `x, mean, sigma` as `RooRealProxy (=
RooProxy<RooAbsReal>`. Both real-valued objects and category objects can be stored.

The proxy allows retrieving and assigning the object's current value (in single-value mode),
and retrieving batches of observable data or index states across multiple events.

Its base class RooArgProxy registers the proxied objects as "servers" of the object that holds the proxy.
When the value of a proxy is changed, the owner is notified. Renaming or exchanging objects that
serve values to the owner of the proxy is handled automatically.

Two typedefs have been defined for backward compatibility:
- `RooRealProxy = RooProxy<RooAbsReal>`. Any generic object that converts to a real value.
- `RooCategoryProxy = RooProxy<RooAbsCategory>`. Handle to generic category objects.
**/

template<class T>
class RooProxy : public RooArgProxy {
public:

  RooProxy() {} ;

  ////////////////////////////////////////////////////////////////////////////////
  /// Constructor with owner.
  /// \param[in] theName Name of this proxy (for printing).
  /// \param[in] desc Description what this proxy should act as.
  /// \param[in] owner The object that owns the proxy. This is important for tracking
  ///            of client-server dependencies.
  /// \param[in] valueServer Notify the owner if value changes.
  /// \param[in] shapeServer Notify the owner if shape (e.g. binning) changes.
  /// \param[in] proxyOwnsArg Proxy will delete the payload if owning.
  RooProxy(const char* theName, const char* desc, RooAbsArg* owner,
      Bool_t valueServer=true, Bool_t shapeServer=false, Bool_t proxyOwnsArg=false)
  : RooArgProxy(theName, desc, owner, valueServer, shapeServer, proxyOwnsArg) { }

  ////////////////////////////////////////////////////////////////////////////////
  /// Constructor with owner and proxied object.
  /// \param[in] theName Name of this proxy (for printing).
  /// \param[in] desc Description what this proxy should act as.
  /// \param[in] owner The object that owns the proxy. This is important for tracking
  ///            of client-server dependencies.
  /// \param[in] ref Reference to the object that the proxy should hold.
  /// \param[in] valueServer Notify the owner if value changes.
  /// \param[in] shapeServer Notify the owner if shape (e.g. binning) changes.
  /// \param[in] proxyOwnsArg Proxy will delete the payload if owning.
  RooProxy(const char* theName, const char* desc, RooAbsArg* owner, T& ref,
      Bool_t valueServer=true, Bool_t shapeServer=false, Bool_t proxyOwnsArg=false) :
        RooArgProxy(theName, desc, owner, ref, valueServer, shapeServer, proxyOwnsArg) { }


  ////////////////////////////////////////////////////////////////////////////////
  /// Copy constructor.
  /// It will accept any RooProxy instance, but attempt a dynamic_cast on its payload.
  /// \throw std::invalid_argument if the types of the payloads are incompatible.
  template<typename U>
  RooProxy(const char* theName, RooAbsArg* owner, const RooProxy<U>& other) :
    RooArgProxy(theName, owner, other) {
    if (_arg && !dynamic_cast<const T*>(_arg))
      throw std::invalid_argument("Tried to construct a RooProxy with incompatible payload.");
  }

  virtual TObject* Clone(const char* newName=0) const { return new RooProxy<T>(newName,_owner,*this); }

  /// Convert the proxy into a number.
  /// \return A category proxy will return the index state, real proxies the result of RooAbsReal::getVal(normSet).
  operator typename T::value_type() const {
    return retrieveValue(arg());
  }

  /// Get the label of the current category state. This function only makes sense for category proxies.
  const char* label() const {
    return arg().getLabel();
  }

  /// Check if the stored object has a range with the given name.
  bool hasRange(const char* rangeName) const {
    return arg().hasRange(rangeName);
  }

  /// Retrieve a batch of real or category data.
  /// \param[in] begin Begin of the range to be retrieved.
  /// \param[in] batchSize Size of the range to be retrieved. Batch may be smaller if no more data available.
  /// \return RooSpan<const double> for real-valued proxies, RooSpan<const int> for category proxies.
  RooSpan<const typename T::value_type> getValBatch(std::size_t begin, std::size_t batchSize) const {
    return retrieveBatchVal(begin, batchSize, arg());
  }

  /// Return reference to object held in proxy.
  const T& arg() const { return static_cast<const T&>(*_arg); }

  /// Return reference to the proxied object.
  const T& operator*() const {
    return static_cast<T&>(*_arg);
  }

  /// Return reference to the proxied object.
  T& operator*() {
    return static_cast<T&>(*_arg);
  }

  ////////////////////////////////////////////////////////////////////////////////
  /// Change object held in proxy into newRef
  bool setArg(T& newRef) {
    if (_arg) {
      if (std::string(arg().GetName()) != newRef.GetName()) {
        newRef.setAttribute(Form("ORIGNAME:%s", arg().GetName())) ;
      }
      return changePointer(RooArgSet(newRef), true);
    } else {
      return changePointer(RooArgSet(newRef), false, true);
    }
  }


  /// Assign a new value to the object pointed to by the proxy.
  /// This requires the payload to be assignable (RooAbsRealLValue or derived, RooAbsCategoryLValue).
  RooProxy<T>& operator=(typename T::value_type value) {
    lvptr(static_cast<T*>(nullptr))->operator=(value);
    return *this;
  }
  /// Set a category state using its state name. This function can only work for category-type proxies.
  RooProxy<T>& operator=(const std::string& newState) {
    static_assert(std::is_base_of<RooAbsCategory, T>::value, "Strings can only be assigned to category proxies.");
    lvptr(static_cast<RooAbsCategoryLValue*>(nullptr))->operator=(newState.c_str());
    return *this;
  }

  /// Query lower limit of range. This requires the payload to be RooAbsRealLValue or derived.
  double min(const char* rname=0) const  { return lvptr(static_cast<const T*>(nullptr))->getMin(rname) ; }
  /// Query upper limit of range. This requires the payload to be RooAbsRealLValue or derived.
  double max(const char* rname=0) const  { return lvptr(static_cast<const T*>(nullptr))->getMax(rname) ; }
  /// Check if the range has a lower bound. This requires the payload to be RooAbsRealLValue or derived.
  bool hasMin(const char* rname=0) const { return lvptr(static_cast<const T*>(nullptr))->hasMin(rname) ; }
  /// Check if the range has a upper bound. This requires the payload to be RooAbsRealLValue or derived.
  bool hasMax(const char* rname=0) const { return lvptr(static_cast<const T*>(nullptr))->hasMax(rname) ; }


private:
  /// Are we a real-valued proxy or a category proxy?
  using LValue_t = typename std::conditional<std::is_base_of<RooAbsReal, T>::value,
      RooAbsRealLValue, RooAbsCategoryLValue>::type;

  ////////////////////////////////////////////////////////////////////////////////
  /// Return l-value pointer to contents. If the contents derive from RooAbsLValue or RooAbsCategoryLValue,
  /// the conversion is safe, and the function directly returns the pointer using a static_cast.
  /// If the template parameter of this proxy is not an LValue type, then
  /// - in a debug build, a dynamic_cast with an assertion is used.
  /// - in a release build, a static_cast is forced, irrespective of what the type of the object actually is. This
  /// is dangerous, but equivalent to the behaviour before refactoring the RooFit proxies.
  /// \deprecated This function is unneccessary if the template parameter is RooAbsRealLValue (+ derived types) or
  /// RooAbsCategoryLValue (+derived types), as arg() will always return the correct type.
  const LValue_t* lvptr(const LValue_t*) const {
    return static_cast<const LValue_t*>(_arg);
  }
  /// \copydoc const LValue_t* lvptr(const LValue_t*) const
  LValue_t* lvptr(LValue_t*) {
    return static_cast<LValue_t*>(_arg);
  }
  /// \copydoc const LValue_t* lvptr(const LValue_t*) const
  const LValue_t* lvptr(const RooAbsArg*) const
  R__SUGGEST_ALTERNATIVE("The template argument of RooProxy needs to derive from RooAbsRealLValue or RooAbsCategoryLValue to safely call this function.") {
#ifdef NDEBUG
    return static_cast<const LValue_t*>(_arg);
#else
    auto theArg = dynamic_cast<const LValue_t*>(_arg);
    assert(theArg);
    return theArg;
#endif
  }
  /// \copydoc const LValue_t* lvptr(const LValue_t*) const
  LValue_t* lvptr(RooAbsArg*)
  R__SUGGEST_ALTERNATIVE("The template argument of RooProxy needs to derive from RooAbsRealLValue or RooAbsCategoryLValue to safely call this function.") {
#ifdef NDEBUG
    return static_cast<LValue_t*>(_arg);
#else
    auto theArg = dynamic_cast<LValue_t*>(_arg);
    assert(theArg);
    return theArg;
#endif
  }


  /// Retrieve index state from a category.
  typename T::value_type retrieveValue(const RooAbsCategory& cat) const {
    return cat.getIndex();
  }

  /// Retrieve value from a real-valued object.
  typename T::value_type retrieveValue(const RooAbsReal& real) const {
    return real.getVal(_nset);
  }

  /// Retrieve a batch of index states from a category.
  RooSpan<const typename T::value_type> retrieveBatchVal(std::size_t begin, std::size_t batchSize, const RooAbsCategory& cat) const {
    return cat.getValBatch(begin, batchSize);
  }

  /// Retrieve a batch of values from a real-valued object. The current normalisation set associated to the proxy will be passed on.
  RooSpan<const typename T::value_type> retrieveBatchVal(std::size_t begin, std::size_t batchSize, const RooAbsReal& real) const {
    return real.getValBatch(begin, batchSize, _nset);
  }


  ClassDef(RooProxy,1) // Proxy for a RooAbsReal object
};

#endif
