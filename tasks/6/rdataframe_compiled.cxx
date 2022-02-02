#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "Math/Vector4D.h"

template <typename T> using Vec = const ROOT::RVec<T>&;
using ROOT::Math::XYZTVector;

ROOT::RVec<std::size_t> find_trijet(Vec<XYZTVector> jets) {
  const auto c = ROOT::VecOps::Combinations(jets, 3);

  float distance = 1e9;
  const auto top_mass = 172.5;
  std::size_t idx = 0;
  for (auto i = 0u; i < c[0].size(); i++) {
    auto p1 = jets[c[0][i]];
    auto p2 = jets[c[1][i]];
    auto p3 = jets[c[2][i]];
    const auto tmp_mass = (p1 + p2 + p3).mass();
    const auto tmp_distance = std::abs(tmp_mass - top_mass);
    if (tmp_distance < distance) {
      distance = tmp_distance;
      idx = i;
    }
  }

  return {c[0][idx], c[1][idx], c[2][idx]};
}


float trijet_pt(Vec<float> pt, Vec<float> eta, Vec<float> phi, Vec<float> mass, Vec<std::size_t> idx)
{
    auto p1 = ROOT::Math::PtEtaPhiMVector(pt[idx[0]], eta[idx[0]], phi[idx[0]], mass[idx[0]]);
    auto p2 = ROOT::Math::PtEtaPhiMVector(pt[idx[1]], eta[idx[1]], phi[idx[1]], mass[idx[1]]);
    auto p3 = ROOT::Math::PtEtaPhiMVector(pt[idx[2]], eta[idx[2]], phi[idx[2]], mass[idx[2]]);
    return (p1 + p2 + p3).pt();
}


void rdataframe() {
    using ROOT::Math::PtEtaPhiMVector;
    using ROOT::VecOps::Construct;

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto df2 = df.Filter([](unsigned int n) { return n >= 3; }, {"nJet"}, "At least three jets")
                 .Define("JetXYZT", [](Vec<float> pt, Vec<float> eta, Vec<float> phi, Vec<float> m) {
                              return Construct<XYZTVector>(Construct<PtEtaPhiMVector>(pt, eta, phi, m));},
                         {"Jet_pt", "Jet_eta", "Jet_phi", "Jet_mass"})
                 .Define("Trijet_idx", find_trijet, {"JetXYZT"});
    auto h1 = df2.Define("Trijet_pt", trijet_pt, {"Jet_pt", "Jet_eta", "Jet_phi", "Jet_mass", "Trijet_idx"})
                 .Histo1D<float>({"", ";Trijet pt (GeV);N_{Events}", 100, 15, 40}, "Trijet_pt");
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
