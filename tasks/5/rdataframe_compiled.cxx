#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "Math/Vector4D.h"

template <typename T> using Vec = const ROOT::RVec<T>&;
using FourVector = ROOT::Math::PtEtaPhiMVector;

auto compute_dimuon_masses(Vec<float> pt, Vec<float> eta, Vec<float> phi, Vec<float> mass, Vec<int> charge)
{
    ROOT::RVec<float> masses;
    const auto c = ROOT::VecOps::Combinations(pt, 2);
    for (auto i = 0u; i < c[0].size(); i++) {
        const auto i1 = c[0][i];
        const auto i2 = c[1][i];
        if (charge[i1] == charge[i2]) continue;
        const FourVector p1(pt[i1], eta[i1], phi[i1], mass[i1]);
        const FourVector p2(pt[i2], eta[i2], phi[i2], mass[i2]);
        masses.push_back((p1 + p2).mass());
    }
    return masses;
};


void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto h = df.Filter([](unsigned int n) { return n >= 2; }, {"nMuon"}, "At least two muons")
               .Define("Dimuon_mass", compute_dimuon_masses,
                       {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass", "Muon_charge"})
               .Filter([](const ROOT::RVec<float> &mass) { return Sum(mass > 60 && mass < 120) > 0; }, {"Dimuon_mass"},
                       "At least one dimuon system with mass in range [60, 120]")
               .Histo1D<float>({"", ";MET (GeV);N_{Events}", 100, 0, 200}, "MET_pt");

    TCanvas c;
    h->Draw();
    c.SaveAs("5_rdataframe_compiled.png");
}

int main() {
    rdataframe();
    return 0;
}
