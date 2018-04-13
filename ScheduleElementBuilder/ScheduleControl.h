#pragma once

#include<wx/wx.h>
#include <wx/panel.h>
#include"ScheduleItem.h"
#include<svector/sdialog.h>

class mainFrame;

class ScheduleControl : public wxPanel
{
public:
	ScheduleControl(wxWindow *parent, ScheduleItem *item) : wxPanel(parent), m_item(item) {}
	virtual const ScheduleItem &GetValueGeneric() = 0;
	void load(std::istream &stream) { m_item->load(stream); }
private:
	ScheduleItem *m_item;
};

class StareScheduleControl : public ScheduleControl
{
public:
	typedef StareScheduleItem ItemType;
	StareScheduleControl(wxWindow *parent);
	StareScheduleControl(wxWindow *parent, const StareScheduleItem &value);
	const ScheduleItem &GetValueGeneric()override;
private:
	void setup();
	sci::NumberTextCtrl<size_t> *m_nRaysControl;
	sci::DecimalNumberTextCtrl<double> *m_azimuthControl;
	sci::DecimalNumberTextCtrl<double> *m_elevationControl;
	StareScheduleItem m_value;
	size_t m_nRays;
	double m_elevation;
	double m_azimuth;
};

class RhiScheduleControl : public ScheduleControl
{
public:
	typedef RhiScheduleItem ItemType;
	RhiScheduleControl(wxWindow *parent);
	RhiScheduleControl(wxWindow *parent, const RhiScheduleItem &value);
	const ScheduleItem &GetValueGeneric()override;
private:
	void setup();
	sci::NumberTextCtrl<size_t> *m_nPointsControl;
	sci::DecimalNumberTextCtrl<double> *m_startAzimuthControl;
	sci::DecimalNumberTextCtrl<double> *m_startElevationControl;
	sci::DecimalNumberTextCtrl<double> *m_endElevationControl;
	size_t m_nPoints;
	double m_startAzimuth;
	double m_startElevation;
	double m_endElevation;
	RhiScheduleItem m_value;
};

class VadScheduleControl : public ScheduleControl
{
public:
	typedef VadScheduleItem ItemType;
	VadScheduleControl(wxWindow *parent);
	VadScheduleControl(wxWindow *parent, const VadScheduleItem &value);
	const ScheduleItem &GetValueGeneric()override;
private:
	void setup();
	sci::NumberTextCtrl<size_t> *m_nPointsControl;
	sci::DecimalNumberTextCtrl<double> *m_startAzimuthControl;
	sci::DecimalNumberTextCtrl<double> *m_elevationControl;
	size_t m_nPoints;
	double m_startAzimuth;
	double m_elevation;
	VadScheduleItem m_value;
};

class AddRowControl : public wxPanel
{
	static const int ID_ADD_BUTTON;
public:
	AddRowControl(wxWindow *parent, mainFrame *frame);
	void OnAddButton(wxCommandEvent& event);
private:
	wxComboBox *m_addCombo;
	mainFrame *m_mainFrame;

	DECLARE_EVENT_TABLE();
};

ScheduleControl *getNewControl(std::string name, wxWindow *background);
