void rdataframe_jitted() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "root://eospublic.cern.ch//eos/root-eos/benchmark/Run2012B_SingleMu.root");
    auto h = df.Histo1D({"", ";Jet p_{T} (GeV);N_{Events}", 100, 15, 60}, "Jet_pt");

    TCanvas c;
    h->Draw();
    c.SaveAs("2_rdataframe_jitted.png");
}
