#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "Math/Vector4D.h"

template <typename T> using Vec = const ROOT::RVec<T>&;
using FourVector = ROOT::Math::PtEtaPhiMVector;

float additional_lepton_pt(Vec<float> pt, Vec<float> eta, Vec<float> phi, Vec<float> mass, Vec<int> charge, Vec<int> flavour)
{
    const auto c = Combinations(pt, 2);
    float lep_pt = -999;
    float best_mass = 99999;
    int best_i1 = -1;
    int best_i2 = -1;
    const auto z_mass = 91.2;
    const auto make_p4 = [&](std::size_t idx) {
        return ROOT::Math::PtEtaPhiMVector(pt[idx], eta[idx], phi[idx], mass[idx]);
    };

    for (auto i = 0; i < c[0].size(); i++) {
        const auto i1 = c[0][i];
        const auto i2 = c[1][i];
        if (charge[i1] == charge[i2]) continue;
        if (flavour[i1] != flavour[i2]) continue;
        const auto p1 = make_p4(i1);
        const auto p2 = make_p4(i2);
        const auto tmp_mass = (p1 + p2).mass();
        if (std::abs(tmp_mass - z_mass) < std::abs(best_mass - z_mass)) {
            best_mass = tmp_mass;
            best_i1 = i1;
            best_i2 = i2;
        }
    }

    if (best_i1 == -1) return lep_pt;

    for (auto i = 0; i < pt.size(); i++) {
        if (i != best_i1 && i != best_i2 && pt[i] > lep_pt) lep_pt = pt[i];
    }

    return lep_pt;
}

void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto concatF = [](const ROOT::RVec<float> &a, const ROOT::RVec<float> &b) { return Concatenate(a, b); };
    auto concatI = [](const ROOT::RVec<int> &a, const ROOT::RVec<int> &b) { return Concatenate(a, b); };
    auto df2 = df.Filter([](unsigned int nElectron, unsigned int nMuon) { return nElectron + nMuon > 2; },
                            {"nElectron", "nMuon"}, "At least three leptons")
                 .Define("Lepton_pt", concatF, {"Muon_pt", "Electron_pt"})
                 .Define("Lepton_eta", concatF, {"Muon_eta", "Electron_eta"})
                 .Define("Lepton_phi", concatF, {"Muon_phi", "Electron_phi"})
                 .Define("Lepton_mass", concatF, {"Muon_mass", "Electron_mass"})
                 .Define("Lepton_charge", concatI, {"Muon_charge", "Electron_charge"})
                 .Define("Lepton_flavour",[](unsigned int nMuon, unsigned int nElectron) {
                                              return Concatenate(ROOT::RVec<int>(nMuon, 0), ROOT::RVec<int>(nElectron, 1));
                                          },
                         {"nMuon", "nElectron"})
                 .Define("AdditionalLepton_pt", additional_lepton_pt,
                         {"Lepton_pt", "Lepton_eta", "Lepton_phi", "Lepton_mass", "Lepton_charge", "Lepton_flavour"})
                 .Filter([](float pt) { return pt != -999; }, {"AdditionalLepton_pt"}, "No valid lepton pair found.");
    auto h1 = df2.Histo1D<float>({"", ";MET (GeV);N_{Events}", 100, 0, 2000}, "MET_sumet");
    auto h2 = df2.Histo1D<float>({"", ";Lepton p_{T} (GeV);N_{Events}", 100, 15, 60}, "AdditionalLepton_pt");

    TCanvas c;
    c.Divide(2, 1);
    c.cd(1);
    h1->Draw();
    c.cd(2);
    h2->Draw();
    c.SaveAs("8_rdataframe_compiled.png");
}

int main() {
    rdataframe();
    return 0;
}
