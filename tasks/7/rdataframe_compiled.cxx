#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"

template <typename T> using Vec = const ROOT::RVec<T>&;

ROOT::RVec<int> find_isolated_jets(Vec<float> eta1, Vec<float> phi1, Vec<float> pt2, Vec<float> eta2, Vec<float> phi2)
{
    ROOT::RVec<int> mask(eta1.size(), 1);
    if (eta2.size() == 0) {
        return mask;
    }

    const auto ptcut = pt2 > 10;
    const auto eta2_ptcut = eta2[ptcut];
    const auto phi2_ptcut = phi2[ptcut];
    if (eta2_ptcut.size() == 0) {
        return mask;
    }

    const auto c = ROOT::VecOps::Combinations(eta1, eta2_ptcut);
    for (auto i = 0u; i < c[0].size(); i++) {
        const auto i1 = c[0][i];
        const auto i2 = c[1][i];
        const auto dr = ROOT::VecOps::DeltaR(eta1[i1], eta2_ptcut[i2], phi1[i1], phi2_ptcut[i2]);
        if (dr < 0.4) mask[i1] = 0;
    }
    return mask;
}


void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto h = df.Filter([](unsigned int n) { return n > 0; }, {"nJet"}, "At least one jet")
               .Define("goodJet_ptcut", [](const ROOT::RVec<float>& pt) { return pt > 30; }, {"Jet_pt"})
               .Define("goodJet_antiMuon",
                       find_isolated_jets,
                       {"Jet_eta", "Jet_phi", "Muon_pt", "Muon_eta", "Muon_phi"})
               .Define("goodJet_antiElectron",
                       find_isolated_jets,
                       {"Jet_eta", "Jet_phi", "Electron_pt", "Electron_eta", "Electron_phi"})
               .Define("goodJet",
                       [](const ROOT::RVec<int> &pt, const ROOT::RVec<int> &muon, const ROOT::RVec<int> &electron) {
                            return pt && muon && electron;
                       },
                       {"goodJet_ptcut", "goodJet_antiMuon", "goodJet_antiElectron"})
               .Filter([](const ROOT::RVec<int> &good) { return Sum(good) > 0; }, {"goodJet"})
               .Define("goodJet_sumPt",
                       [](const ROOT::RVec<int> &good, const ROOT::RVec<float> &pt) { return Sum(pt[good]); },
                       {"goodJet", "Jet_pt"})
               .Histo1D<float>({"", ";Jet p_{T} sum (GeV);N_{Events}", 100, 15, 200}, "goodJet_sumPt");

    TCanvas c;
    h->Draw();
    c.SaveAs("7_rdataframe_compiled.png");
}

int main() {
    rdataframe();
    return 0;
}
