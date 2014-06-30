/*
 * Characteristic.cpp
 *
 *  Created on: Mar 5, 2014
 *      Author: Stephan Opfer
 */

#include "engine/model/Characteristic.h"

namespace alica
{

	Characteristic::Characteristic()
	{

	}

	Characteristic::~Characteristic()
	{
	}

	string Characteristic::toString()
	{
		stringstream ss;
		ss << "#Characteristic " << endl;
		ss << "\t Capability: " << capability->getName() << endl;
		ss << "\t CapValue: " << capValue->getName() << endl;
		ss << "\t Weight: " << weight << endl;
		return ss.str();
	}

//================= Getter and Setter =============================

	const Capability* Characteristic::getCapability() const
	{
		return capability;
	}

	void Characteristic::setCapability(Capability* capability)
	{
		this->capability = capability;
	}

	const CapValue* Characteristic::getCapValue() const
	{
		return capValue;
	}

	void Characteristic::setCapValue(CapValue* capValue)
	{
		this->capValue = capValue;
	}

	double Characteristic::getWeight() const
	{
		return weight;
	}

	void Characteristic::setWeight(double weight)
	{
		this->weight = weight;
	}

} /* namespace Alica */

