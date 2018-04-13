#ifndef APEX_MAINFRAME_H
#define APEX_MAINFRAME_H

#include "app.h"
#include<svector/svector.h>
#include<svector/splot.h>
#include<svector/sreadwrite.h>
#include<svector/sstring.h>
#include<faam/faam.h>

class AddRowControl;
class ScheduleControl;

class mainFrame : public wxFrame
{
public:
	static const int ID_FILE_EXIT;
	static const int ID_FILE_SAVE;
	static const int ID_FILE_SAVE_AS;
	static const int ID_FILE_OPEN;
	static const int ID_FILE_NEW;
	static const int ID_FILE_EXPORT;
	static const int ID_HELP_ABOUT;
	static const int ID_REMOVE_BUTTON;
	static const int ID_UP_BUTTON;
	static const int ID_DOWN_BUTTON;

	mainFrame(wxFrame *frame, const wxString& title);
	~mainFrame();
	void addRow(AddRowControl *generatingControl, std::string type);
	int getMajorVersion() { return 1; }
	int getMinorVersion() { return 0; }
private:
	void OnExit(wxCommandEvent& event);
	void OnNew(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnLoad(wxCommandEvent &event);
	void OnExport(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnRemoveRow(wxCommandEvent& event);
	void OnMoveUpRow(wxCommandEvent& event);
	void OnMoveDownRow(wxCommandEvent& event);
	void swapControls(size_t index1, size_t index2);
	void save(std::string &filename);
	void load(std::string &filename);
	void removeRow(size_t index);
	void addRow(size_t index, ScheduleControl *newControl);
	bool clear();
	std::vector<AddRowControl*> m_addRowControls;
	std::vector<ScheduleControl*> m_scheduleControls;
	std::vector<wxButton*> m_removeButtons;
	std::vector<wxButton*> m_upButtons;
	std::vector<wxButton*> m_downButtons;
	wxPanel *m_background;
	wxFlexGridSizer *m_topSizer;
	std::string m_saveFile;
	bool m_saved;
	DECLARE_EVENT_TABLE();
};
#endif // APEX_MAINFRAME_H
