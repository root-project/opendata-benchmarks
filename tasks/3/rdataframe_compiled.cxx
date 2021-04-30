#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"

void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto goodJetPt = [](const ROOT::RVec<float> &pt, const ROOT::RVec<float> &eta) { return pt[eta > 1.0]; };
    auto h = df.Define("goodJet_pt", goodJetPt, {"Jet_pt", "Jet_eta"})
               .Histo1D<ROOT::RVec<float>>({"", ";Jet p_{T} (GeV);N_{Events}", 100, 15, 60}, "goodJet_pt");

    TCanvas c;
    h->Draw();
    c.SaveAs("3_rdataframe_compiled.png");
}

int main() {
    rdataframe();
    return 0;
}
