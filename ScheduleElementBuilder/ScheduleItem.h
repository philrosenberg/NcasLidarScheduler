#pragma once
#include<vector>

struct Direction
{
	double elevation;
	double azimuth;
};

class ScheduleItem
{
public:
	ScheduleItem(size_t nRaysPerPoint = 1);
	void write(std::ostream &stream) const;
	size_t getNPoints() const { return m_directions.size(); }
	size_t getRaysPerPoint() const { return m_nRaysPerPoint; }
	Direction getDirection(size_t index) const { return m_directions[index]; }
	virtual void save(std::ostream &stream) const = 0;
	virtual void load(std::istream &stream) = 0;
	virtual std::string getName() const = 0;
protected:
	void addPoint(double elevation, double azimuth);
	void addPoint(const Direction &direction);
private:
	std::vector<Direction> m_directions;
	size_t m_nRaysPerPoint;
};

class StareScheduleItem : public ScheduleItem
{
public:
	StareScheduleItem(size_t nRays, double elevation, double azimuth);
	double getElevation() const { return getDirection(0).elevation; }
	double getAzimuth() const { return getDirection(0).azimuth; }
	void save(std::ostream &stream) const override;
	void load(std::istream &stream) override;
	std::string getName() const override { return "Stare"; };
};

class RhiScheduleItem : public ScheduleItem
{
public:
	RhiScheduleItem(size_t raysPerPoint, size_t nElevations, double intitialAzimuth, double initialElevation = 0.0, double finalElevation = 180.0);
	double getInitialAzimuth() const { return getDirection(0).azimuth; }
	double getInitialElevation() const { return getDirection(0).elevation; }
	double getFinalElevation() const { return getDirection(getNPoints() - 1).elevation; }
	void save(std::ostream &stream) const override;
	void load(std::istream &stream) override;
	std::string getName() const override { return "RHI"; };
};

class VadScheduleItem : public ScheduleItem
{
public:
	VadScheduleItem(size_t raysPerPoint, double elevation, size_t nAzimuths, double initialAzimuth = 0.0);
	double getElevation() const { return getDirection(0).elevation; }
	double getInitialAzimuth() const { return getDirection(0).azimuth; }
	void save(std::ostream &stream) const override;
	void load(std::istream &stream) override;
	std::string getName() const override { return "VAD"; };
};
