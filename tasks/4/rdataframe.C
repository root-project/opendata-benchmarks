void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "/home/stefan/opendata_samples/Run2012B_SingleMu.root");
    auto h = df.Filter("Sum(Jet_pt > 40 && abs(Jet_eta) < 1.0) > 1", "More than one jet with pt > 40 and abs(eta) < 1.0")
               .Histo1D({"", ";MET (GeV);N_{Events}", 100, 0, 2000}, "MET_sumet");

    TCanvas c;
    h->Draw();
    c.SaveAs("4_rdataframe.png");
}
