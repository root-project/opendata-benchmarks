#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "Math/Vector4D.h"

template <typename T> using Vec = const ROOT::RVec<T>&;

ROOT::RVec<std::size_t> find_trijet(Vec<float> pt, Vec<float> eta, Vec<float> phi, Vec<float> mass)
{
    const auto c = ROOT::VecOps::Combinations(pt, 3);
    const auto make_p4 = [&](std::size_t idx) {
        return ROOT::Math::PtEtaPhiMVector(pt[idx], eta[idx], phi[idx], mass[idx]);
    };

    float trijet_mass = -1;
    float distance = 1e9;
    const auto top_mass = 172.5;
    std::size_t idx = 0;
    for (auto i = 0; i < c[0].size(); i++) {
        auto p1 = make_p4(c[0][i]);
        auto p2 = make_p4(c[1][i]);
        auto p3 = make_p4(c[2][i]);
        const auto tmp_mass = (p1 + p2 + p3).mass();
        const auto tmp_distance = std::abs(tmp_mass - top_mass);
        if (tmp_distance < distance) {
            distance = tmp_distance;
            trijet_mass = tmp_mass;
            idx = i;
        }

    }

    return {c[0][idx], c[1][idx], c[2][idx]};
};


void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto df2 = df.Filter([](unsigned int n) { return n >= 3; }, {"nJet"}, "At least three jets")
                 .Define("Trijet_idx", find_trijet, {"Jet_pt", "Jet_eta", "Jet_phi", "Jet_mass"});
    auto h1 = df2.Define("Trijet_pt",
                         [](const ROOT::RVec<float> &pt, const ROOT::RVec<std::size_t> &idx) { return Take(pt, idx); },
                         {"Jet_pt", "Trijet_idx"})
                 .Histo1D<ROOT::RVec<float>>({"", ";Trijet pt (GeV);N_{Events}", 100, 15, 40}, "Trijet_pt");
    auto h2 = df2.Define("Trijet_leadingBtag",
                         [](const ROOT::RVec<float> &btag, const ROOT::RVec<std::size_t> &idx) { return Max(Take(btag, idx)); },
                         {"Jet_btag", "Trijet_idx"})
                 .Histo1D<float>({"", ";Trijet leading b-tag;N_{Events}", 100, 0, 1}, "Trijet_leadingBtag");

    TCanvas c;
    c.Divide(2, 1);
    c.cd(1);
    h1->Draw();
    c.cd(2);
    h2->Draw();
    c.SaveAs("6_rdataframe_compiled.png");
}

int main() {
    rdataframe();
    return 0;
}
