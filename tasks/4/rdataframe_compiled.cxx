#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"

void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto filter = [](const ROOT::RVec<float> & pt) {
            return Sum(pt > 40) > 1;
    };
    auto h = df.Filter(filter, {"Jet_pt"}, "More than one jet with pt > 40")
               .Histo1D<float>({"", ";MET (GeV);N_{Events}", 100, 0, 200}, "MET_pt");

    TCanvas c;
    h->Draw();
    c.SaveAs("4_rdataframe_compiled.png");
}

int main() {
    rdataframe();
    return 0;
}
