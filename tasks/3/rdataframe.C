void rdataframe() {
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("Events", "/home/stefan/opendata_samples/Run2012B_SingleMu.root");
    auto h = df.Define("goodJet_pt", "Jet_pt[Jet_eta > 1.0]")
               .Histo1D({"", ";Jet p_{T} (GeV);N_{Events}", 100, 15, 60}, "goodJet_pt");

    TCanvas c;
    h->Draw();
    c.SaveAs("3_rdataframe.png");
}
