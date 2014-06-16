/*
 * SuccessMarks.h
 *
 *  Created on: Jun 16, 2014
 *      Author: stefan
 */

#ifndef SUCCESSMARKS_H_
#define SUCCESSMARKS_H_

using namespace std;
#include <map>
#include <memory>
#include <unordered_set>
#include <list>

namespace alica
{

	class AbstractPlan;
	class EntryPoint;

	class SuccessMarks
	{
	public:
		SuccessMarks();
		virtual ~SuccessMarks();


		//TODO uses ICollection in C# so far only unordered_set needed
		void limitToPlans(unique_ptr<unordered_set<AbstractPlan*> > active);
		map<AbstractPlan*, list<EntryPoint*> > getSuccesMarks() const;
		void setSuccesMarks(map<AbstractPlan*, list<EntryPoint*> > succesMarks);
		void clear();

	protected:
		map<AbstractPlan*,list<EntryPoint*> > succesMarks;
	};

} /* namespace alica */

#endif /* SUCCESSMARKS_H_ */
