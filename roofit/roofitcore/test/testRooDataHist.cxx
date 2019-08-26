// Tests for the RooWorkspace
// Authors: Stephan Hageboeck, CERN  01/2019

#include "RooDataHist.h"
#include "RooGlobalFunc.h"
#include "RooRealVar.h"
#include "RooHelpers.h"
#include "TH1D.h"
#include "TMath.h"
#include "TFile.h"

#include "gtest/gtest.h"

/// ROOT-8163
/// The RooDataHist warns that it has to adjust the binning of x to the next bin boundary
/// although the boundaries match perfectly.
TEST(RooDataHist, BinningRangeCheck_8163)
{
  RooHelpers::HijackMessageStream hijack(RooFit::INFO, RooFit::DataHandling, "dataHist");

  RooRealVar x("x", "x", 0., 1.);
  TH1D hist("hist", "", 10, 0., 1.);

  RooDataHist dataHist("dataHist", "", RooArgList(x), &hist);
  EXPECT_TRUE(hijack.str().empty()) << "Messages issued were: " << hijack.str();
}




double computePoissonUpper(double weight) {
  double upperLimit = weight;
  double CL;
  do {
    upperLimit += 0.001;
    CL = 0.;
    for (unsigned int i = 0; i <= (unsigned int)weight; ++i) {
      CL += TMath::PoissonI(i, upperLimit);
    }
//    std::cout << "Upper=" << upperLimit << "\tCL=" << CL << std::endl;
  } while (CL > 0.16);

  return upperLimit;
}

double computePoissonLower(double weight) {
  double lowerLimit = weight;
  double CL;
  do {
    CL = 0.;
    lowerLimit -= 0.001;
    for (unsigned int i = 0; i < (unsigned int)weight; ++i) {
      CL += TMath::PoissonI(i, lowerLimit);
    }
  } while (CL < 1. - 0.16);

  return lowerLimit;
}

TEST(RooDataHist, UnWeightedEntries)
{
  RooRealVar x("x", "x", -10., 10.);
  x.setBins(20);
  RooRealVar w("w", "w", 0., 10.);
  RooArgSet coordinates(x);

  constexpr double targetBinContent = 10.;
  RooDataHist dataHist("dataHist", "", RooArgList(x));
  for (unsigned int i=0; i < 200; ++i) {
    x.setVal(-10. + 20./200. * i);
    dataHist.add(coordinates);
  }

  EXPECT_EQ(dataHist.numEntries(), 20);
  ASSERT_EQ(dataHist.sumEntries(), 20 * targetBinContent);

  x.setVal(0.);
  RooArgSet* coordsAtZero = dataHist.get(coordinates)->snapshot();
  x.setVal(0.9);
  RooArgSet* coordsAtPoint9 = dataHist.get(coordinates)->snapshot();
  EXPECT_EQ(static_cast<RooRealVar*>(coordsAtZero->find(x))->getVal(),
            static_cast<RooRealVar*>(coordsAtPoint9->find(x))->getVal());

  const double weight = dataHist.weight();
  EXPECT_EQ(weight, targetBinContent);

  EXPECT_NEAR(dataHist.weightError(RooAbsData::Poisson),
      sqrt(targetBinContent), 0.7); // TODO What is the actual value?

  {
    double lo, hi;
    dataHist.weightError(lo, hi, RooAbsData::Poisson);
    EXPECT_LT(lo, hi);
    EXPECT_NEAR(lo, weight - computePoissonLower(weight), 3.E-2);
    EXPECT_NEAR(hi, computePoissonUpper(weight) - weight, 3.E-2);
  }

  EXPECT_NEAR(dataHist.weightError(RooAbsData::SumW2),
      sqrt(targetBinContent), 1.E-14);

  {
    double lo, hi;
    dataHist.weightError(lo, hi, RooAbsData::SumW2);
    EXPECT_NEAR(lo, sqrt(targetBinContent), 1.E-14);
    EXPECT_NEAR(lo, sqrt(targetBinContent), 1.E-14);
    EXPECT_EQ(lo, hi);
  }


  RooArgSet* coordsAt10 = dataHist.get(10)->snapshot();
  const double weightBin10 = dataHist.weight();

  EXPECT_NEAR(static_cast<RooRealVar*>(coordsAt10->find(x))->getVal(), 0.5, 1.E-1);
  EXPECT_EQ(weight, weightBin10);
}


TEST(RooDataHist, WeightedEntries)
{
  RooRealVar x("x", "x", -10., 10.);
  x.setBins(20);
  RooRealVar w("w", "w", 0., 10.);
  RooArgSet coordinates(x);

  constexpr double targetBinContent = 20.;
  RooDataHist dataHist("dataHist", "", RooArgList(x));
  for (unsigned int i=0; i < 200; ++i) {
    x.setVal(-10. + 20./200. * i);
    dataHist.add(coordinates, 2.);
  }


  EXPECT_EQ(dataHist.numEntries(), 20);
  EXPECT_EQ(dataHist.sumEntries(), 20 * targetBinContent);

  x.setVal(0.);
  dataHist.get(coordinates)->snapshot();
  const double weight = dataHist.weight();
  ASSERT_EQ(weight, targetBinContent);


  const double targetError = sqrt(10*4.);

  EXPECT_NEAR(dataHist.weightError(RooAbsData::Poisson),
      targetError, 1.5); // TODO What is the actual value?

  {
    double lo, hi;
    dataHist.weightError(lo, hi, RooAbsData::Poisson);
    EXPECT_LT(lo, hi);
    EXPECT_NEAR(lo, weight - computePoissonLower(weight), 3.E-2);
    EXPECT_NEAR(hi, computePoissonUpper(weight) - weight, 3.E-2);
  }

  EXPECT_NEAR(dataHist.weightError(RooAbsData::SumW2),
      targetError, 1.E-14);

  {
    double lo, hi;
    dataHist.weightError(lo, hi, RooAbsData::SumW2);
    EXPECT_NEAR(lo, targetError, 1.E-14);
    EXPECT_NEAR(lo, targetError, 1.E-14);
    EXPECT_EQ(lo, hi);
  }


  RooArgSet* coordsAt10 = dataHist.get(10)->snapshot();
  const double weightBin10 = dataHist.weight();

  EXPECT_NEAR(static_cast<RooRealVar*>(coordsAt10->find(x))->getVal(), 0.5, 1.E-1);
  EXPECT_EQ(weight, weightBin10);
}

TEST(RooDataHist, ReadV4Legacy)
{
  TFile v4Ref("dataHistv4_ref.root");
  ASSERT_TRUE(v4Ref.IsOpen());

  RooDataHist* legacy = nullptr;
  v4Ref.GetObject("dataHist", legacy);
  ASSERT_NE(legacy, nullptr);

  RooDataHist& dataHist = *legacy;

  constexpr double targetBinContent = 20.;

  RooArgSet* legacyVals = dataHist.get(10)->snapshot();
  EXPECT_EQ(static_cast<RooAbsReal*>(legacyVals->find("x"))->getVal(),
      static_cast<RooAbsReal*>(dataHist.get(10)->find("x"))->getVal());


  EXPECT_EQ(dataHist.numEntries(), 20);
  EXPECT_EQ(dataHist.sumEntries(), 20 * targetBinContent);

  static_cast<RooRealVar*>(legacyVals->find("x"))->setVal(0.);
  RooArgSet* coordsAtZero = dataHist.get(*legacyVals)->snapshot();
  const double weight = dataHist.weight();
  ASSERT_EQ(weight, targetBinContent);


  const double targetError = sqrt(10*4.);

  EXPECT_NEAR(dataHist.weightError(RooAbsData::Poisson),
      targetError, 1.5); // TODO What is the actual value?

  {
    double lo, hi;
    dataHist.weightError(lo, hi, RooAbsData::Poisson);
    EXPECT_LT(lo, hi);
    EXPECT_NEAR(lo, weight - computePoissonLower(weight), 3.E-2);
    EXPECT_NEAR(hi, computePoissonUpper(weight) - weight, 3.E-2);
  }

  EXPECT_NEAR(dataHist.weightError(RooAbsData::SumW2),
      targetError, 1.E-14);

  {
    double lo, hi;
    dataHist.weightError(lo, hi, RooAbsData::SumW2);
    EXPECT_NEAR(lo, targetError, 1.E-14);
    EXPECT_NEAR(lo, targetError, 1.E-14);
    EXPECT_EQ(lo, hi);
  }


  RooArgSet* coordsAt10 = dataHist.get(10)->snapshot();
  const double weightBin10 = dataHist.weight();

  EXPECT_NEAR(static_cast<RooRealVar*>(coordsAt10->find("x"))->getVal(), 0.5, 1.E-1);
  EXPECT_EQ(weight, weightBin10);
}
