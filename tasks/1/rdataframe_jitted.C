void rdataframe_jitted() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto h = df.Histo1D({"", ";MET (GeV);N_{Events}", 100, 0, 200}, "MET_pt");

    TCanvas c;
    h->Draw();
    c.SaveAs("1_rdataframe_jitted.png");
}

