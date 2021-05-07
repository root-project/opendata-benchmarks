void rdataframe_jitted() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto h = df.Filter("Sum(Jet_pt > 40) > 1", "More than one jet with pt > 40")
               .Histo1D({"", ";MET (GeV);N_{Events}", 100, 0, 200}, "MET_pt");

    TCanvas c;
    h->Draw();
    c.SaveAs("4_rdataframe_jitted.png");
}
