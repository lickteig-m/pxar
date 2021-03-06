// -- Usage:
// ---------
//    ../bin/pXar -c '../scripts/phdacdacscan.C("vcal", "phdacdacscan.root", "testROC")'

// ----------------------------------------------------------------------
// scan all other DACs against "dac1" and display the average pulseheight as a 2D plot
// the API is programmed from the macro using PixTest::setDAC which forwards this to the api 
// (w/o dict direct api interaction is not yet possible)
void phdacdacscan(string dac1 = "vcal", string rootfile = "phdacdacscan.root", string cfgdirectory = "testROC") {
//   gSystem->Load("../lib/libpxar.dylib");
//   gSystem->Load("../lib/libpxargui.dylib");
//   gSystem->Load("../lib/libpxartests.dylib");
//   gSystem->Load("../lib/libpxarutil.dylib");
//   gSystem->Load("../lib/libpxarana.dylib");

  ConfigParameters *configParameters = ConfigParameters::Singleton();
  
  configParameters->setDirectory(cfgdirectory);
  string cfgFile = configParameters->getDirectory() + string("/configParameters.dat");
  configParameters->readConfigParameterFile(cfgFile);

  
  PixTestParameters *ptp = new PixTestParameters(configParameters->getDirectory() + "/" + configParameters->getTestParameterFileName()); 

  PixSetup *ap = new PixSetup("DEBUG", ptp, configParameters);  

  cout << "pxar: dumping results into " << rootfile << endl;
  TFile *rfile = TFile::Open(rootfile.c_str(), "RECREATE"); 
  
  PixTestFactory *factory = PixTestFactory::instance(); 
  
  PixTest *pt = factory->createTest("DacDacScan", ap); 
  pt->setDAC("ctrlreg", 4); 
  pt->setParameter("PHmap", "1"); 
  pt->setParameter("DAC1", dac1); 

  vector<string> dacs = configParameters->getDacs();
  for (unsigned int idac = 0; idac < dacs.size(); ++idac) {
    cout << "===> dacdacscan for vcal vs. " << dacs[idac] << endl;
    pt->setParameter("DAC2", dacs[idac]); 
    pt->doTest(); 
  }
    
  delete pt; 

  rfile->Close();

  ap->killApi();

  // -- and now plot/save them
  TFile *f = TFile::Open(rootfile.c_str()); 
  TH2D *h2(0); 
  for (unsigned int idac = 0; idac < dacs.size(); ++idac) {
    h2 = (TH2D*)f->Get(Form("DacDacScan/ph_vcal_%s_c11_r20_C0_V0", dacs[idac].c_str())); 
    if (0 == h2) continue;
    h2->Draw("colz");
    c0.SaveAs(Form("ph_vcal_%s.pdf", dacs[idac].c_str()));
  }

}
