#include "mainFrame.h"
#include "ScheduleControl.h"
#include <wx/filename.h>

const int mainFrame::ID_FILE_EXIT = ::wxNewId();
const int mainFrame::ID_FILE_SAVE = ::wxNewId();
const int mainFrame::ID_FILE_SAVE_AS = ::wxNewId();
const int mainFrame::ID_FILE_OPEN = ::wxNewId();
const int mainFrame::ID_FILE_NEW = ::wxNewId();
const int mainFrame::ID_FILE_EXPORT = ::wxNewId();
const int mainFrame::ID_HELP_ABOUT = ::wxNewId();
const int mainFrame::ID_REMOVE_BUTTON = ::wxNewId();
const int mainFrame::ID_UP_BUTTON = ::wxNewId();
const int mainFrame::ID_DOWN_BUTTON = ::wxNewId();

BEGIN_EVENT_TABLE(mainFrame, wxFrame)
EVT_MENU(ID_FILE_EXIT, mainFrame::OnExit)
EVT_MENU(ID_FILE_SAVE, mainFrame::OnSave)
EVT_MENU(ID_FILE_SAVE_AS, mainFrame::OnSaveAs)
EVT_MENU(ID_FILE_OPEN, mainFrame::OnLoad)
EVT_MENU(ID_FILE_NEW, mainFrame::OnNew)
EVT_MENU(ID_FILE_EXPORT, mainFrame::OnExport)
EVT_MENU(ID_HELP_ABOUT, mainFrame::OnAbout)
EVT_BUTTON(ID_REMOVE_BUTTON, mainFrame::OnRemoveRow)
EVT_BUTTON(ID_UP_BUTTON, mainFrame::OnMoveUpRow)
EVT_BUTTON(ID_DOWN_BUTTON, mainFrame::OnMoveDownRow)
END_EVENT_TABLE()

mainFrame::mainFrame(wxFrame *frame, const wxString& title)
	: wxFrame(frame, -1, title)
{
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(wxT(""));
	fileMenu->Append(ID_FILE_OPEN, wxT("Open\tCtrl+O"), wxT("Open Existing Configuration"));
	fileMenu->Append(ID_FILE_NEW, wxT("New\tCtrl+N"), wxT("Create New Configuration"));
	fileMenu->Append(ID_FILE_SAVE, wxT("Save\tCtrl+S"), wxT("Save Configuration"));
	fileMenu->Append(ID_FILE_SAVE_AS, wxT("Save As..."), wxT("Save Configuration As"));
	fileMenu->Append(ID_FILE_EXPORT, wxT("Export Lidar File..."), wxT("Export Lidar File"));
	fileMenu->Append(ID_FILE_EXIT, wxT("E&xit\tAlt+F4"), wxT("Exit the application"));
	mbar->Append(fileMenu, wxT("&File"));

	wxMenu* helpMenu = new wxMenu(wxT(""));
	helpMenu->Append(ID_HELP_ABOUT, wxT("&About\tF1"), wxT("Show info about this application"));
	mbar->Append(helpMenu, wxT("&Help"));

	SetMenuBar(mbar);
	m_background = new wxPanel(this);

	m_addRowControls.push_back(new AddRowControl(m_background, this));

	m_topSizer = new wxFlexGridSizer(1, 4, 3, 3);
	m_topSizer->Add (m_addRowControls[0], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_background->SetSizerAndFit(m_topSizer);
	this->Fit();

	m_saved = true;
}

void mainFrame::OnExit(wxCommandEvent& event)
{
	if( clear() )
		Close();
}

void mainFrame::OnNew(wxCommandEvent &event)
{
	clear();
	m_saved = true;
	m_saveFile = "";
}

void mainFrame::OnSave(wxCommandEvent& event)
{
	if (m_saveFile == "")
		OnSaveAs(event);
	else
		save(m_saveFile);
}

void mainFrame::OnSaveAs(wxCommandEvent& event)
{
	if (!Validate())
		return;

	wxFileDialog saveFileDialog(this, "Save lidar schedule as...", "C:\\Lidar\\System\\Scan parameters\\Step_Stare_Scan_Files\\", "", "txt files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;
	std::string filename = saveFileDialog.GetPath();

	save(filename);
}

void mainFrame::OnLoad(wxCommandEvent &event)
{
	wxFileDialog loadFileDialog(this, "Load lidar schedule from...", "C:\\Lidar\\System\\Scan parameters\\Step_Stare_Scan_Files\\", "", "txt files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (loadFileDialog.ShowModal() == wxID_CANCEL)
		return;
	std::string filename = loadFileDialog.GetPath();

	load(filename);
}

void mainFrame::OnExport(wxCommandEvent& event)
{
	if (!Validate())
		return;

	wxFileDialog exportFileDialog(this, "Export lidar schedule to...", "C:\\Lidar\\System\\Scan parameters\\Step_Stare_Scan_Files\\", "", "txt files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
	if (exportFileDialog.ShowModal() == wxID_CANCEL)
		return;
	std::string filename = exportFileDialog.GetPath();
	std::string tempFilename = wxFileName::CreateTempFileName(filename);
	std::fstream eout;
	eout.open(tempFilename.c_str(), std::ios::out);
	if (!eout.is_open())
	{
		wxMessageBox(wxString("Failed to create temp file ") + tempFilename.c_str() + ". Please check that the directory exists and is not write protected. Also non-ASCII characters in the file name may cause problems.");
		return;
	}
	for (size_t i = 0; i < m_scheduleControls.size(); ++i)
	{
		m_scheduleControls[i]->GetValueGeneric().write(eout);
	}
	eout.close();
	bool retry = true;
	while ( retry && !wxCopyFile(tempFilename, filename))
	{
		retry = wxYES==wxMessageBox("Could not write to " + filename + ". Check to ensure it is not read only or open in another program.\n\nDo you wish to retry?", "Write Error", wxYES_NO);
	}

	if (!wxRemoveFile(tempFilename))
	{
		wxMessageBox("Could not remove temp file " + filename + ". You will need to clean this file up yourself.", "Cleanup Error");
	}
}

void mainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(wxT("NCAS Lidar Scheduler Version 1.00.0"), wxT("About NCAS Lidar Scheduler..."));
}

void mainFrame::addRow(AddRowControl *generatingControl, std::string type)
{
	//create the new control
	ScheduleControl *newControl = NULL;
	if (type == "Stare")
	{
		newControl = new StareScheduleControl(m_background);
	}
	else if (type == "RHI")
	{
		newControl = new RhiScheduleControl(m_background);
	}
	else if (type == "VAD")
	{
		newControl = new VadScheduleControl(m_background);
	}

	//check we have created a new control
	if (!newControl)
	{
		wxMessageBox("Failed to create a new line of type " + type + ".", "Error");
		return;
	}

	//find which contol generated the event to add a new line
	size_t addControlIndex = -1;
	for (size_t i = 0; i < m_addRowControls.size(); ++i)
		if (m_addRowControls[i] == generatingControl)
			addControlIndex = i;

	addRow(addControlIndex, newControl);
}

void mainFrame::addRow(size_t index, ScheduleControl *newControl)
{
	//add the new control to the correct spot.
	m_scheduleControls.insert(m_scheduleControls.begin() + index, newControl);
	//add a new add row control - because these are generic, we can just append a new one to the end
	m_addRowControls.insert(m_addRowControls.begin() + index, new AddRowControl(m_background, this));
	//add the buttons
	m_removeButtons.insert(m_removeButtons.begin() + index, new wxButton(m_background, ID_REMOVE_BUTTON, "Delete"));
	m_upButtons.insert(m_upButtons.begin() + index, new wxButton(m_background, ID_UP_BUTTON, "Move Up"));
	m_downButtons.insert(m_downButtons.begin() + index, new wxButton(m_background, ID_DOWN_BUTTON, "Move Down"));

	//Insert the appropriate rows
	m_topSizer->SetRows(m_topSizer->GetRows() + 2);
	m_topSizer->Insert(index * 8, m_removeButtons[index], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_topSizer->Insert(index * 8, m_downButtons[index], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_topSizer->Insert(index * 8, m_upButtons[index], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_topSizer->Insert(index * 8, m_scheduleControls[index], 0, wxLeft | wxALIGN_CENTER_VERTICAL, 5);
	m_topSizer->InsertStretchSpacer(index * 8);
	m_topSizer->InsertStretchSpacer(index * 8);
	m_topSizer->InsertStretchSpacer(index * 8);
	m_topSizer->Insert(index * 8, m_addRowControls[index], 0, wxLeft | wxALIGN_CENTER_VERTICAL, 5);

	m_background->Fit();
	this->Fit();

	m_saved = false;
}

void mainFrame::OnRemoveRow(wxCommandEvent& event)
{
	wxObject *pressedButton = event.GetEventObject();
	size_t index = -1;
	for (size_t i = 0; i < m_removeButtons.size(); ++i)
	{
		if ((wxObject*)m_removeButtons[i] == pressedButton)
			index = i;
	}

	if (index == -1)
	{
		wxMessageBox("Something has gone wrong and you have tried to remove a row with a button that shouldn't exist. Sorry.", "Error");
		return;
	}

	removeRow(index);
}

void mainFrame::removeRow(size_t index)
{
	//remove the items. There are 8. 4 on the line with the actual scan pattern and 4 (including spacers) on the
	//line with the Add another line control.
	for (size_t i = 0; i < 8; ++i)
		m_topSizer->Remove(index * 8);

	//sort the sizing
	m_topSizer->SetRows(m_topSizer->GetRows() - 2);

	//remove and destory the controls
	m_addRowControls[index]->Destroy();
	m_addRowControls.erase(m_addRowControls.begin() + index);
	m_scheduleControls[index]->Destroy();
	m_scheduleControls.erase(m_scheduleControls.begin() + index);
	m_removeButtons[index]->Destroy();
	m_removeButtons.erase(m_removeButtons.begin() + index);
	m_upButtons[index]->Destroy();
	m_upButtons.erase(m_upButtons.begin() + index);
	m_downButtons[index]->Destroy();
	m_downButtons.erase(m_downButtons.begin() + index);

	m_saved = false;

	m_background->Layout();
	Layout();
	m_background->Fit();
	Fit();
}

bool mainFrame::clear()
{
	if (!m_saved)
	{
		int result = wxMessageBox("Do you wish to save changes made to the current schedule?", "Save changes", wxYES_NO|wxCANCEL|wxCANCEL_DEFAULT);
		if (result == wxCANCEL)
			return false;
		if (result == wxYES)
		{
			OnSave(wxCommandEvent());
			if (!m_saved)
				return false; //the save was cancelled
		}
	}

	while (m_scheduleControls.size() > 0)
		removeRow(0);

	m_saved = false;

	return true;
}

void mainFrame::OnMoveUpRow(wxCommandEvent& event)
{
	wxObject *pressedButton = event.GetEventObject();
	size_t index = -1;
	for (size_t i = 0; i < m_upButtons.size(); ++i)
	{
		if ((wxObject*)m_upButtons[i] == pressedButton)
			index = i;
	}

	if (index == -1)
	{
		wxMessageBox("Something has gone wrong and you have tried to move up a row with a button that shouldn't exist. Sorry.", "Error");
		return;
	}

	if (index != 0)
	{
		swapControls(index - 1, index);
	}
}

void mainFrame::OnMoveDownRow(wxCommandEvent& event)
{
	wxObject *pressedButton = event.GetEventObject();
	size_t index = -1;
	for (size_t i = 0; i < m_downButtons.size(); ++i)
	{
		if ((wxObject*)m_downButtons[i] == pressedButton)
			index = i;
	}

	if (index == -1)
	{
		wxMessageBox("Something has gone wrong and you have tried to move down a row with a button that shouldn't exist. Sorry.", "Error");
		return;
	}

	if (index != m_downButtons.size() - 1)
	{
		swapControls(index + 1, index);
	}
}

void mainFrame::swapControls(size_t index1, size_t index2)
{
	if (index1 > index2)
		std::swap(index1, index2);

	m_topSizer->Remove(index2 * 8 + 4);
	m_topSizer->Remove(index1 * 8 + 4);

	std::swap(m_scheduleControls[index1], m_scheduleControls[index2]);

	m_topSizer->Insert(index1 * 8 + 4, m_scheduleControls[index1], 0, wxLeft | wxALIGN_CENTER_VERTICAL, 5);
	m_topSizer->Insert(index2 * 8 + 4, m_scheduleControls[index2], 0, wxLeft | wxALIGN_CENTER_VERTICAL, 5);

	m_background->Layout();
	m_background->Fit();
	Fit();

	m_saved = false;
}


void mainFrame::save(std::string &filename)
{
	if (!Validate())
		return;
	std::string tempFilename = wxFileName::CreateTempFileName(filename);
	std::fstream sout;
	sout.open(tempFilename.c_str(), std::ios::out);
	if (!sout.is_open())
	{
		wxMessageBox(wxString("Failed to create temp file ") + tempFilename.c_str() + ". Please check that the directory exists and is not write protected. Also non-ASCII characters in the file name may cause problems.");
		return;
	}
	sout << "NCAS_LIDAR_SCHEDULE_BUILDER v " << getMajorVersion() << " " << getMinorVersion() << "\n";

	for (size_t i = 0; i < m_scheduleControls.size(); ++i)
	{
		sout << m_scheduleControls[i]->GetValueGeneric().getName() << " ";
		m_scheduleControls[i]->GetValueGeneric().save(sout);
		sout << "\n";
	}
	sout << "END";
	sout.close();
	bool retry = true;
	while (retry && !wxCopyFile(tempFilename, filename))
	{
		retry = wxYES == wxMessageBox("Could not write to " + filename + ". Check to ensure it is not read only or open in another program.\n\nDo you wish to retry?", "Write Error", wxYES_NO);
	}

	if (!wxRemoveFile(tempFilename))
	{
		wxMessageBox("Could not remove temp file " + filename + ". You will need to clean this file up yourself.", "Cleanup Error");
	}

	m_saved = true;
	m_saveFile = filename;
}

void mainFrame::load(std::string &filename)
{
	try
	{
		//clear the current data
		if (!clear())
			return;

		std::fstream sin;
		sin.open(filename.c_str(), std::ios::in);
		if (!sin.is_open())
		{
			wxMessageBox(wxString("Failed to open file ") + filename.c_str() + " for reading. Please check that the file exists and you have read access. Also non-ASCII characters in the file name may cause problems.");
			return;
		}
		std::string identifier;
		std::string versionPrefix;
		size_t majorVersion;
		size_t minorVersion;
		sin >> identifier >> versionPrefix >> majorVersion >> minorVersion;

		bool minorVersionTooHigh = false;
		if (identifier != "NCAS_LIDAR_SCHEDULE_BUILDER")
		{
			wxMessageBox("The file " + filename + " did not start with the identifier NCAS_LIDAR_SCHEDULE_BUILDER and therefor is not a file that can be read by this software. Note that this software cannot read files exported for use by the lidar. It can only read files created using the File->Save or File->Save As... menu items.", "Load Error");
			return;
		}
		if (versionPrefix != "v")
		{
			wxMessageBox("The file " + filename + " is missing the software version in the header and cannot be loaded.", "Load Error");
			return;
		}
		if (majorVersion > getMajorVersion())
		{
			std::stringstream message;
			message << "The file " + filename + " was written with software version " << majorVersion << "." << minorVersion << " but this software can only read files from version " << getMajorVersion() << ".xx or before.";
			wxMessageBox(message.str(), "Load Error");
			return;
		}
		if (minorVersion > getMinorVersion())
		{
			std::stringstream message;
			message << "The file " + filename + " was written with software version " << majorVersion << "." << minorVersion << ". This software is version " << getMajorVersion() << "." << getMinorVersion() << ". The file will load, but some scan patterns may not be available in this version and hence will be omitted. You will be prompted to chose a new filename if you save this schedule.";
			wxMessageBox(message.str(), "Load Warning");
			minorVersionTooHigh = true;
		}

		std::stringstream log;

		std::string pattern;
		sin >> pattern;

		while (pattern != "END")
		{
			ScheduleControl *newControl = NULL;
			if (pattern == "Stare")
			{
				StareScheduleItem value(1, 0, 0);
				value.load(sin);
				newControl = new StareScheduleControl(m_background, value);
				addRow(m_scheduleControls.size(), newControl);
			}
			else if (pattern == "RHI")
			{
				RhiScheduleItem value(1, 2, 0);
				value.load(sin);
				newControl = new RhiScheduleControl(m_background, value);
				addRow(m_scheduleControls.size(), newControl);
			}
			else if (pattern == "VAD")
			{
				VadScheduleItem value(1, 0, 2);
				value.load(sin);
				newControl = new VadScheduleControl(m_background, value);
				addRow(m_scheduleControls.size(), newControl);
			}
			else
			{
				log << "Found a pattern called " << pattern << " which this software does not support. This line will be ignored.\n";
				//read and ignore the remainder of the line.
				std::string temp;
				std::getline(sin, temp);
			}
			sin >> pattern;
			if (pattern != "END" && (sin.eof() || sin.bad() || sin.fail()))
			{
				log << "Found end of file or had a read fail before finding the END statement.\n";
			}
		}

		if (log.str().length() > 0)
		{
			wxMessageBox(log.str(), "Load Warnings");
		}

		sin.close();

		m_saved = true;
		if (minorVersionTooHigh)
		{
			m_saved = false;
			m_saveFile = "";
		}
	}
	catch (...)
	{
		wxMessageBox("Sorry, there was some unknown error when attempting to load the file. If the problem persists please submit the file to the software author for analysis." "Load Error");
	}
}

mainFrame::~mainFrame()
{
}