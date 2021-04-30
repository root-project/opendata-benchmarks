#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"

void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto h = df.Histo1D<float>({"", ";MET (GeV);N_{Events}", 100, 0, 2000}, "MET_sumet");

    TCanvas c;
    h->Draw();
    c.SaveAs("1_rdataframe_compiled.png");
}

int main() {
    rdataframe();
}
