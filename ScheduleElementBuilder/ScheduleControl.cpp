#include"ScheduleControl.h"
#include"mainFrame.h"

const int AddRowControl::ID_ADD_BUTTON = ::wxNewId();

BEGIN_EVENT_TABLE(AddRowControl, wxPanel)
EVT_BUTTON(ID_ADD_BUTTON, AddRowControl::OnAddButton)
END_EVENT_TABLE()


//List the different control types (as pointers) for use in vaiadic template calls
#define CONTROLPOINTERTYPES StareScheduleControl*, RhiScheduleControl*, VadScheduleControl*
//this is for use as the dummy arguments when we have variadic template functions that use the above types
#define CONTROLPOINTERNULLS nullptr, nullptr, nullptr

//Use variadic templates to iterate through all the types of control and create a new one with the
//correct shedule item name.

template< class T>
ScheduleControl *getNewControlVar(std::string typeName, wxWindow *parent, T dummy)
{
	typedef std::remove_pointer<T>::type firstType;
	if (ScheduleItemTraits<firstType::ItemType>::name() == typeName)
		return new firstType(parent);
	throw("Could not create a control with this type");
}

template< class T, class... Args>
ScheduleControl *getNewControlVar(std::string typeName, wxWindow *parent, T dummy, Args... remainingDummy)
{
	typedef std::remove_pointer<T>::type firstType;
	if (ScheduleItemTraits<firstType::ItemType>::name() == typeName)
		return new firstType(parent);
	return getNewControlVar<Args...>(typeName, parent, remainingDummy...);
}

//This is the externally visible function, that just creates a new control - the user doesn't see all the variadic stuff.
ScheduleControl *getNewControl(std::string typeName, wxWindow *parent)
{
	return getNewControlVar<CONTROLPOINTERTYPES>(typeName, parent, CONTROLPOINTERNULLS);
}

template< class T >
void getItemNamesVar(std::vector<std::string> &names, T dummy)
{
	typedef std::remove_pointer<T>::type firstType;
	names.push_back(ScheduleItemTraits<firstType::ItemType>::name());
}

template< class T, class... Args >
void getItemNamesVar(std::vector<std::string> &names, T dummy, Args... remainingDummy)
{
	typedef std::remove_pointer<T>::type firstType;
	names.push_back(ScheduleItemTraits<firstType::ItemType>::name());
	getItemNamesVar<Args...>(names, remainingDummy...);
}

std::vector<std::string> getItemNames()
{
	std::vector<std::string> result;
	getItemNamesVar<CONTROLPOINTERTYPES>(result, CONTROLPOINTERNULLS);
	return result;
}



StareScheduleControl::StareScheduleControl(wxWindow *parent)
	:ScheduleControl(parent, &m_value), m_value(1, 90, 84)
{
	setup();
}

StareScheduleControl::StareScheduleControl(wxWindow *parent, const StareScheduleItem &value)
	:ScheduleControl(parent, &m_value), m_value(value)
{
	setup();
}

void StareScheduleControl::setup()
{
	m_elevation = m_value.getElevation();
	m_azimuth = m_value.getAzimuth();
	m_nRays = m_value.getNPoints();

	m_azimuthControl = new sci::DecimalNumberTextCtrl<double>(this, wxID_ANY, -99.99, 999.99, &m_azimuth);
	m_elevationControl = new sci::DecimalNumberTextCtrl<double>(this, wxID_ANY, -99.99, 999.99, &m_elevation);
	
	m_nRaysControl = new sci::NumberTextCtrl<size_t>(this, wxID_ANY, "%zu", &m_nRays);

	wxStaticBoxSizer *sizer = new wxStaticBoxSizer(wxHORIZONTAL, this, ScheduleItemTraits<ItemType>::name());
	sizer->Add(new wxStaticText(this, wxID_ANY, "Number of Repeats:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_nRaysControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(new wxStaticText(this, wxID_ANY, "Azimuth:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_azimuthControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(new wxStaticText(this, wxID_ANY, "Elevation:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_elevationControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	TransferDataToWindow();

	SetSizerAndFit(sizer);
}

const ScheduleItem & StareScheduleControl::GetValueGeneric()
{
	TransferDataFromWindow();
	m_value = StareScheduleItem(m_nRays, m_elevation, m_azimuth);
	return m_value;
}

RhiScheduleControl::RhiScheduleControl(wxWindow *parent)
	:ScheduleControl(parent, &m_value), m_value(1, 16, 0, 0, 180)
{
	setup();
}

RhiScheduleControl::RhiScheduleControl(wxWindow *parent, const RhiScheduleItem &value)
	: ScheduleControl(parent, &m_value), m_value(value)
{
	setup();
}

void RhiScheduleControl::setup()
{
	m_startElevation = m_value.getInitialElevation();
	m_startAzimuth = m_value.getInitialAzimuth();
	m_endElevation = m_value.getFinalElevation();
	m_nPoints = m_value.getNPoints();

	m_nPointsControl = new sci::NumberTextCtrl<size_t>(this, wxID_ANY, "%zu", &m_nPoints);
	m_startAzimuthControl = new sci::DecimalNumberTextCtrl<double>(this, wxID_ANY, -99.99, 999.99, &m_startAzimuth);
	m_startElevationControl = new sci::DecimalNumberTextCtrl<double>(this, wxID_ANY, -99.99, 999.99, &m_startElevation);
	m_endElevationControl = new sci::DecimalNumberTextCtrl<double>(this, wxID_ANY, -99.99, 999.99, &m_endElevation);


	wxStaticBoxSizer *sizer = new wxStaticBoxSizer(wxHORIZONTAL, this, ScheduleItemTraits<ItemType>::name());
	sizer->Add(new wxStaticText(this, wxID_ANY, "Number of Elevations:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_nPointsControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(new wxStaticText(this, wxID_ANY, "Start Azimuth:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_startAzimuthControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(new wxStaticText(this, wxID_ANY, "Start Elevation:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_startElevationControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(new wxStaticText(this, wxID_ANY, "End Elevation:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_endElevationControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	TransferDataToWindow();

	SetSizerAndFit(sizer);
}

const ScheduleItem & RhiScheduleControl::GetValueGeneric()
{
	TransferDataFromWindow();
	m_value = RhiScheduleItem(1, m_nPoints, m_startAzimuth, m_startElevation, m_endElevation);
	return m_value;
}

VadScheduleControl::VadScheduleControl(wxWindow *parent)
	:ScheduleControl(parent, &m_value), m_value(1, 75, 6, 0)
{
	setup();
}

VadScheduleControl::VadScheduleControl(wxWindow *parent, const VadScheduleItem &value)
	: ScheduleControl(parent, &m_value), m_value(value)
{
	setup();
}

void VadScheduleControl::setup()
{
	m_elevation = m_value.getElevation();
	m_startAzimuth = m_value.getInitialAzimuth();
	m_nPoints = m_value.getNPoints();

	m_nPointsControl = new sci::NumberTextCtrl<size_t>(this, wxID_ANY, "%zu", &m_nPoints);
	m_startAzimuthControl = new sci::DecimalNumberTextCtrl<double>(this, wxID_ANY, -99.99, 999.99, &m_startAzimuth);
	m_elevationControl = new sci::DecimalNumberTextCtrl<double>(this, wxID_ANY, -99.99, 999.99, &m_elevation);


	wxStaticBoxSizer *sizer = new wxStaticBoxSizer(wxHORIZONTAL, this, ScheduleItemTraits<ItemType>::name());
	sizer->Add(new wxStaticText(this, wxID_ANY, "Number of Azimuths:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_nPointsControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(new wxStaticText(this, wxID_ANY, "Start Azimuth:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_startAzimuthControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(new wxStaticText(this, wxID_ANY, "Elevation:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_elevationControl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	TransferDataToWindow();

	SetSizerAndFit(sizer);
}

const ScheduleItem & VadScheduleControl::GetValueGeneric()
{
	TransferDataFromWindow();
	m_value = VadScheduleItem(1, m_elevation, m_nPoints, m_startAzimuth);
	return m_value;
}

AddRowControl::AddRowControl(wxWindow *parent, mainFrame *frame)
:wxPanel(parent)
{
	std::vector<std::string> choices = getItemNames();
	std::vector<wxString> wx_choices;
	for(size_t i=0; i<choices.size(); ++i)
		wx_choices.push_back(wxString(choices[i]));
	m_addCombo = new wxComboBox(this, wxID_ANY, wx_choices[0], wxDefaultPosition, wxDefaultSize, wx_choices.size(), &wx_choices[0], wxCB_READONLY);
	wxButton *addButton = new wxButton(this, ID_ADD_BUTTON, "Add");
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	m_mainFrame = frame;

	sizer->Add(new wxStaticText(this, wxID_ANY, "Add Row Here: "), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(m_addCombo, 0, wxALL| wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(addButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	SetSizerAndFit(sizer);
}

void AddRowControl::OnAddButton(wxCommandEvent& event)
{
	m_mainFrame->addRow(this, std::string(m_addCombo->GetValue()));
}