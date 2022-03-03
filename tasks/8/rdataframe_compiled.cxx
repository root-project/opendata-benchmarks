#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "Math/Vector4D.h"

template <typename T> using Vec = const ROOT::RVec<T>&;
using FourVector = ROOT::Math::PtEtaPhiMVector;
constexpr static unsigned int PLACEHOLDER_VALUE = 99999;

unsigned int additional_lepton_idx(Vec<float> pt, Vec<float> eta, Vec<float> phi, Vec<float> mass, Vec<int> charge, Vec<int> flavour)
{
    const auto c = Combinations(pt, 2);
    float best_mass = PLACEHOLDER_VALUE;
    unsigned int best_i1 = PLACEHOLDER_VALUE;
    unsigned int best_i2 = PLACEHOLDER_VALUE;
    const auto z_mass = 91.2;
    const auto make_p4 = [&](std::size_t idx) {
        return ROOT::Math::PtEtaPhiMVector(pt[idx], eta[idx], phi[idx], mass[idx]);
    };

    for (auto i = 0u; i < c[0].size(); i++) {
        const auto i1 = c[0][i];
        const auto i2 = c[1][i];
        if (charge[i1] == charge[i2]) continue;
        if (flavour[i1] != flavour[i2]) continue;
        const auto tmp_mass = (make_p4(i1) + make_p4(i2)).mass();
        if (std::abs(tmp_mass - z_mass) < std::abs(best_mass - z_mass)) {
            best_mass = tmp_mass;
            best_i1 = i1;
            best_i2 = i2;
        }
    }

    if (best_i1 == PLACEHOLDER_VALUE) return PLACEHOLDER_VALUE;

    float max_pt = -999;
    unsigned int lep_idx = PLACEHOLDER_VALUE;
    for (auto i = 0u; i < pt.size(); i++) {
        if (i != best_i1 && i != best_i2 && pt[i] > max_pt) {
            max_pt = pt[i];
            lep_idx = i;
        }
    }

    return lep_idx;
}

void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto concatF = [](const ROOT::RVec<float> &a, const ROOT::RVec<float> &b) { return Concatenate(a, b); };
    auto concatI = [](const ROOT::RVec<int> &a, const ROOT::RVec<int> &b) { return Concatenate(a, b); };
    auto transverseMass = [](const ROOT::RVec<float> &Lepton_pt, const ROOT::RVec<float> &Lepton_phi,
                             float MET_pt, float MET_phi, unsigned int idx) {
        return sqrt(2.0 * Lepton_pt[idx] * MET_pt * (1.0 - cos(ROOT::VecOps::DeltaPhi(MET_phi, Lepton_phi[idx]))));
    };
    auto h = df.Filter([](unsigned int nElectron, unsigned int nMuon) { return nElectron + nMuon > 2; },
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
               .Define("AdditionalLepton_idx", additional_lepton_idx,
                       {"Lepton_pt", "Lepton_eta", "Lepton_phi", "Lepton_mass", "Lepton_charge", "Lepton_flavour"})
               .Filter([](unsigned int idx) { return idx != PLACEHOLDER_VALUE; }, {"AdditionalLepton_idx"}, "No valid lepton pair found.")
               .Define("TransverseMass", transverseMass,
                       {"Lepton_pt", "Lepton_phi", "MET_pt", "MET_phi", "AdditionalLepton_idx"})

               .Histo1D<double>({"", ";Transverse mass (GeV);N_{Events}", 100, 0, 200}, "TransverseMass");

    TCanvas c;
    h->Draw();
    c.SaveAs("8_rdataframe_compiled.png");
}

int main() {
    rdataframe();
    return 0;
}
