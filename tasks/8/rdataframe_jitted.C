template <typename T> using Vec = const ROOT::RVec<T>&;
using FourVector = ROOT::Math::PtEtaPhiMVector;

unsigned int additional_lepton_idx(Vec<float> pt, Vec<float> eta, Vec<float> phi, Vec<float> mass, Vec<int> charge, Vec<int> flavour)
{
    const auto c = Combinations(pt, 2);
    float best_mass = 99999;
    unsigned int best_i1 = 99999;
    unsigned int best_i2 = 99999;
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

    if (best_i1 == 99999) return 99999;

    float max_pt = -999;
    unsigned int lep_idx = 99999;
    for (auto i = 0u; i < pt.size(); i++) {
        if (i != best_i1 && i != best_i2 && pt[i] > max_pt) {
            max_pt = pt[i];
            lep_idx = i;
        }
    }

    return lep_idx;
}


void rdataframe_jitted() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto h = df.Filter("nElectron + nMuon > 2", "At least three leptons")
               .Define("Lepton_pt", "Concatenate(Muon_pt, Electron_pt)")
               .Define("Lepton_eta", "Concatenate(Muon_eta, Electron_eta)")
               .Define("Lepton_phi", "Concatenate(Muon_phi, Electron_phi)")
               .Define("Lepton_mass", "Concatenate(Muon_mass, Electron_mass)")
               .Define("Lepton_charge", "Concatenate(Muon_charge, Electron_charge)")
               .Define("Lepton_flavour", "Concatenate(ROOT::RVec<int>(nMuon, 0), ROOT::RVec<int>(nElectron, 1))")
               .Define("AdditionalLepton_idx", additional_lepton_idx,
                       {"Lepton_pt", "Lepton_eta", "Lepton_phi", "Lepton_mass", "Lepton_charge", "Lepton_flavour"})
               .Filter("AdditionalLepton_idx != 99999", "No valid lepton pair found.")
               .Define("TransverseMass",
                       "sqrt(2.0 * Lepton_pt[AdditionalLepton_idx] * MET_pt * (1.0 - cos(ROOT::VecOps::DeltaPhi(MET_phi, Lepton_phi[AdditionalLepton_idx]))))")
               .Histo1D({"", ";Transverse mass (GeV);N_{Events}", 100, 0, 200}, "TransverseMass");

    TCanvas c;
    h->Draw();
    c.SaveAs("8_rdataframe_jitted.png");
}
