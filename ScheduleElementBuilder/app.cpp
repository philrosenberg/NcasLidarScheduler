//This file is part of NCAS Lidar Scheduler - software to generate Schedules for the Halo Photonics Stream Line Lidar.
//Copyright(C) 2018  P.D.Rosenberg
//See the licence file distributed with this file for licence terms.

#include "app.h"
#include "mainFrame.h"

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	mainFrame* frame = new mainFrame(0L, wxT("NCAS Lidar Scheduler"));
	frame->SetIcon(wxICON(amain));
	frame->Show();
	return true;
}
