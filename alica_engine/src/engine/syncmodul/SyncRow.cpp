/*
 * SyncRow.cpp
 *
 *  Created on: Aug 27, 2014
 *      Author: Stefan Jakob
 */

#include <engine/syncmodul/SyncRow.h>
#include <algorithm>
#include <iostream>
#include <engine/containers/SyncData.h>

namespace alica
{

	SyncRow::SyncRow()
	{
		this->syncData = nullptr;
	}

	SyncRow::SyncRow(SyncData* sd)
	{
		this->syncData = sd;
	}

	SyncRow::~SyncRow()
	{
		delete syncData;
	}

	vector<int>& SyncRow::getReceivedBy()
	{
		sort(this->receivedBy.begin(), this->receivedBy.end());
		return receivedBy;
	}

	void SyncRow::setReceivedBy(vector<int> receivedBy)
	{
		this->receivedBy = receivedBy;
	}

	SyncData* SyncRow::getSyncData()
	{
		return syncData;
	}

	void SyncRow::setSyncData(SyncData* syncData)
	{
		if(this->syncData != nullptr)
		{
			delete this->syncData;
		}
		this->syncData = syncData;
	}
	void SyncRow::toString()
	{
		cout << "SyncRow" << endl;
		cout << "ReceivedBy: ";
		for(int i : receivedBy)
		{
			cout << i << " ";
		}
		cout << endl;
		this->syncData->toString();
	}

} /* namespace alica */
