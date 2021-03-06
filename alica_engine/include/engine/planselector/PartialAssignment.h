/*
 * PartialAssignment.h
 *
 *  Created on: Jul 4, 2014
 *      Author: Stefan Jakob
 */

#ifndef PARTIALASSIGNMENT_H_
#define PARTIALASSIGNMENT_H_

//#define SUCDEBUG


#include <vector>
#include <list>
#include <limits>
#include <sstream>
#include <string>
#include <algorithm>
#include <memory>
#include <math.h>

#include "engine/IAssignment.h"
#include "engine/collections/AssignmentCollection.h"

using namespace std;
namespace alica
{

	class EpByTaskComparer;
	class EntryPoint;
	class Plan;
	class SuccessCollection;
	class UtilityFunction;
	class DynCardinality;
	class SimplePlanTree;
	class PartialAssignmentPool;

	class PartialAssignment : virtual public IAssignment
	{
	public:
		PartialAssignment(PartialAssignmentPool* pap);
		virtual ~PartialAssignment();
		void clear();
		static void reset(PartialAssignmentPool* pap); // has to be called before calculating the task assignment
		static PartialAssignment* getNew(PartialAssignmentPool* pap, shared_ptr<vector<int> > robots, Plan* plan, shared_ptr<SuccessCollection> sucCol);
		static PartialAssignment* getNew(PartialAssignmentPool* pap, PartialAssignment* oldPA);
		short getEntryPointCount();
		int totalRobotCount();
		shared_ptr<vector<int> > getRobotsWorking(EntryPoint* ep);
		shared_ptr<vector<int> > getRobotsWorking(long epid);
		shared_ptr<list<int> > getRobotsWorkingAndFinished(EntryPoint* ep);
		shared_ptr<list<int> > getRobotsWorkingAndFinished(long epid);
		shared_ptr<list<int> > getUniqueRobotsWorkingAndFinished(EntryPoint* ep);
		bool addIfAlreadyAssigned(shared_ptr<SimplePlanTree> spt, int robot);
		bool assignRobot(int robot, int index);
		shared_ptr<list<PartialAssignment*> > expand();
		bool isValid();
		bool isGoal();
		static bool compareTo(PartialAssignment* thisPa, PartialAssignment* newPa);
		string toString();
		AssignmentCollection* getEpRobotsMapping();
		Plan* getPlan();
		shared_ptr<UtilityFunction> getUtilFunc();
		shared_ptr<SuccessCollection> getEpSuccessMapping();
		string assignmentCollectionToString();
		shared_ptr<vector<EntryPoint*> > getEntryPoints();
		int getHash();
		void setHash(int hash);
		bool isHashCalculated();
		void setHashCalculated(bool hashCalculated);
		void setMax(double max);
		shared_ptr<vector<int>> getRobots();
		int hash = 0;


	private:
		const int INFINIT = numeric_limits<int>::max();
		static int pow(int x, int y);

	protected:
		PartialAssignmentPool* pap;
		static EpByTaskComparer epByTaskComparer;
		// UtilityFunction
		shared_ptr<UtilityFunction> utilFunc;
		AssignmentCollection* epRobotsMapping;
		shared_ptr<vector<int>> robots;
		vector<shared_ptr<DynCardinality>> dynCardinalities;
		Plan* plan;
		const long PRECISION = 1073741824;
		long compareVal = 0;
		bool hashCalculated;

		shared_ptr<SuccessCollection> epSuccessMapping;


	};

} /* namespace alica */

namespace std
{
    template<>
    struct hash<alica::PartialAssignment>
    {
        typedef alica::PartialAssignment argument_type;
        typedef std::size_t result_type;

        result_type operator()(argument_type & pa) const
        {
        	if(pa.isHashCalculated())
        	{
        		return pa.hash;
        	}
        	int basei = pa.getEpRobotsMapping()->getSize() + 1;
        	shared_ptr<vector<int>> robots;
        	for(int i = 0; i < pa.getEpRobotsMapping()->getSize(); ++i)
        	{
        		robots = pa.getEpRobotsMapping()->getRobots(i);
        		for(int robot : *robots)
        		{
        			for (int idx = 0; idx < pa.getRobots()->size(); idx++) {
        				if (pa.getRobots()->at(idx) == robot)
        				{
        					pa.setHash(pa.hash + (i + 1) * pow(basei, idx));
        				}
        			}

        		}
        	}
            pa.setHashCalculated(true);
            return pa.hash;
        }
    };
}

#endif /* PARTIALASSIGNMENT_H_ */
