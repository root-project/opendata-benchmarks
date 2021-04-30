template <typename T> using Vec = const ROOT::RVec<T>&;
using FourVector = ROOT::Math::PtEtaPhiMVector;

auto compute_dimuon_masses(Vec<float> pt, Vec<float> eta, Vec<float> phi, Vec<float> mass, Vec<int> charge)
{
    ROOT::RVec<float> masses;
    const auto c = ROOT::VecOps::Combinations(pt.size(), 2);
    for (auto i = 0; i < pt.size(); i++) {
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
    auto h = df.Filter("nMuon >= 2", "At least two muons")
               .Define("Dimuon_mass", compute_dimuon_masses, {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass", "Muon_charge"})
               .Filter("Sum(Dimuon_mass > 60 && Dimuon_mass < 100) > 0",
                       "At least one dimuon system with mass in range [60, 100]")
               .Histo1D({"", ";MET (GeV);N_{Events}", 100, 0, 2000}, "MET_sumet");

    TCanvas c;
    h->Draw();
    c.SaveAs("5_rdataframe.png");
}
