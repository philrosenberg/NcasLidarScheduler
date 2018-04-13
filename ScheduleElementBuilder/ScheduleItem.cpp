#include "ScheduleItem.h"
#include <ostream>
#include<iomanip>

double mapTo360(double angle)
{
	return (angle - floor(angle / 360.0)*360.0);
}

ScheduleItem::ScheduleItem(size_t nRaysPerPoint)
{
	m_nRaysPerPoint = nRaysPerPoint;
}

void ScheduleItem::addPoint(double elevation, double azimuth)
{
	addPoint(Direction{ elevation, azimuth });
}
void ScheduleItem::addPoint(const Direction &direction)
{
	m_directions.push_back(direction);
}

void ScheduleItem::write(std::ostream &stream) const
{
	for (size_t i = 0; i < m_directions.size(); ++i)
		for(size_t j=0; j<m_nRaysPerPoint; ++j)
			stream << std::setfill('0') << std::setw(7) << std::setprecision(3) << std::fixed << mapTo360(m_directions[i].azimuth) << std::setw(7) << mapTo360(m_directions[i].elevation) << "\n";
}


StareScheduleItem::StareScheduleItem(size_t nRays, double elevation, double azimuth)
	:ScheduleItem(nRays)
{
	addPoint(elevation, azimuth);
}

void StareScheduleItem::save(std::ostream &stream) const
{
	stream << getRaysPerPoint() << " " << getAzimuth() << " " << getElevation();
}

void StareScheduleItem::load(std::istream &stream)
{
	double azimuth;
	double elevation;
	size_t nRaysPerPoint;
	stream  >> nRaysPerPoint >> azimuth >> elevation;

	*this = StareScheduleItem(nRaysPerPoint, elevation, azimuth);
}

RhiScheduleItem::RhiScheduleItem(size_t raysPerPoint, size_t nElevations, double intitialAzimuth, double initialElevation, double finalElevation)
	: ScheduleItem(raysPerPoint)
{
	if (nElevations < 2)
		throw("A RHI scan must have at least 2 elevations.");
	double elevationInterval = (finalElevation - initialElevation) / double(nElevations - 1);
	for (size_t i = 0; i < nElevations; ++i)
		addPoint(initialElevation + i*elevationInterval, intitialAzimuth);
}

void RhiScheduleItem::save(std::ostream &stream) const
{
	stream << getRaysPerPoint() << " " << getNPoints() << " " << getInitialAzimuth() << " " << getInitialElevation() << " " << getFinalElevation();
}

void RhiScheduleItem::load(std::istream &stream)
{
	double initialAzimuth;
	double initialElevation;
	double finalElevation;
	size_t nPoints;
	size_t raysPerPoint;
	stream >> raysPerPoint >> nPoints >> initialAzimuth >> initialElevation >> finalElevation;

	*this = RhiScheduleItem( 1, nPoints, initialAzimuth, initialElevation, finalElevation);
}


VadScheduleItem::VadScheduleItem(size_t raysPerPoint, double elevation, size_t nAzimuths, double initialAzimuth)
	: ScheduleItem(raysPerPoint)
{
	if (nAzimuths < 2)
		throw("A VAD scan must have at least 2 azimuths.");
	double azimuthInterval = 360.0 / double(nAzimuths);
	for (size_t i = 0; i < nAzimuths; ++i)
		addPoint(elevation, initialAzimuth + i*azimuthInterval);
}

void VadScheduleItem::save(std::ostream &stream) const
{
	stream << getRaysPerPoint() << " " << getNPoints() << " " << getInitialAzimuth() << " " << getElevation();
}

void VadScheduleItem::load(std::istream &stream)
{
	double initialAzimuth;
	double elevation;
	size_t nPoints;
	size_t raysPerPoint;
	stream >> raysPerPoint >> nPoints >> initialAzimuth >> elevation;

	*this = VadScheduleItem((size_t)1, elevation, nPoints, initialAzimuth);
}