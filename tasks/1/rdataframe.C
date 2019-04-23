void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "/home/stefan/opendata_samples/Run2012B_SingleMu.root");
    auto h = df.Histo1D({"", ";MET (GeV);N_{Events}", 100, 0, 2000}, "MET_sumet");

    TCanvas c;
    h->Draw();
    c.SaveAs("1_rdataframe.png");
}

