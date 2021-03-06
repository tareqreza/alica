/*
 * ConstraintQuery.cpp
 *
 *  Created on: Oct 17, 2014
 *      Author: Philipp
 */
#include "engine/constraintmodul/ConstraintQuery.h"
//#define CQ_DEBUG

#include "engine/AlicaEngine.h"
#include "engine/BasicBehaviour.h"
#include "engine/IAlicaClock.h"
#include "engine/ITeamObserver.h"
#include "engine/RunningPlan.h"
#include "engine/collections/RobotEngineData.h"
#include "engine/constraintmodul/ConstraintCall.h"
#include "engine/constraintmodul/ConstraintDescriptor.h"
#include "engine/constraintmodul/ConstraintStore.h"
#include "engine/constraintmodul/IConstraintSolver.h"
#include "engine/constraintmodul/SolverTerm.h"
#include "engine/constraintmodul/SolverVariable.h"
#include "engine/model/Condition.h"
#include "engine/model/State.h"
#include "engine/model/Parametrisation.h"
#include "engine/model/PlanType.h"
#include "engine/model/Variable.h"

#include <iostream>

namespace alica
{
	ConstraintQuery::ConstraintQuery(AlicaEngine* ae) : ae(ae)
	{
		store = make_shared<UniqueVarStore>();
	}


	void ConstraintQuery::addVariable(Variable* v)
	{
		queriedStaticVariables.push_back(v);
	}

	void ConstraintQuery::addVariable(int robot, string ident)
	{
		queriedDomainVariables.push_back(
				this->ae->getTeamObserver()->getRobotById(robot)->getSortedVariable(
						ident));
	}

	void ConstraintQuery::clearDomainVariables()
	{
		queriedDomainVariables.clear();
	}

	void ConstraintQuery::clearStaticVariables()
	{
		queriedStaticVariables.clear();
	}

	bool ConstraintQuery::existsSolution(int solverType, shared_ptr<RunningPlan> rp)
	{
		IConstraintSolver* solver = this->ae->getSolver(solverType);

		vector<shared_ptr<ConstraintDescriptor>> cds = vector<shared_ptr<ConstraintDescriptor>>();
		vector<Variable*> relevantVariables;
		int domOffset;
		if(!collectProblemStatement(rp, relevantVariables, cds, solver, domOffset)) {
			return false;
		}
		return solver->existsSolution(relevantVariables, cds);
	}

	bool ConstraintQuery::collectProblemStatement(shared_ptr<RunningPlan> rp, vector<Variable*>& relevantVariables,
													vector<shared_ptr<ConstraintDescriptor>>& cds,
													IConstraintSolver* solver, int& domOffset)
	{
#ifdef CQ_DEBUG
		long time = rp->getAlicaEngine()->getIAlicaClock()->now();
#endif
		store->clear();
		relevantStaticVariables.clear();
		relevantDomainVariables.clear();
		calls.clear();
		relevantStaticVariables.insert(relevantStaticVariables.end(), queriedStaticVariables.begin(),
										queriedStaticVariables.end());
		relevantDomainVariables.insert(relevantDomainVariables.end(), queriedDomainVariables.begin(),
										queriedDomainVariables.end());
		for (Variable* v : relevantStaticVariables)
		{
			store->add(v);
		}
#ifdef CQ_DEBUG
		cout << "CQ: Starting Query with static Vars:";
		for (Variable* v : relevantStaticVariables)
		{
			cout << " " << v->getName();
		}
		cout << endl;
#endif
		while (rp != nullptr && (relevantStaticVariables.size() > 0 || relevantDomainVariables.size() > 0))
		{
#ifdef CQ_DEBUG
			cout << "CQ: Query at " << rp->getPlan()->getName() << endl;
#endif
			rp->getConstraintStore()->acceptQuery(shared_from_this(), rp);
			if (rp->getPlanType() != nullptr)
			{
				//process bindings for plantype
				vector<Variable*> tmpVector = vector<Variable*>();
				for (Parametrisation* p : rp->getPlanType()->getParametrisation())
				{
					if (p->getSubPlan() == rp->getPlan()
							&& find(relevantStaticVariables.begin(), relevantStaticVariables.end(), p->getSubVar())
									!= relevantStaticVariables.end())
					{
						tmpVector.push_back(p->getVar());
						store->addVarTo(p->getSubVar(), p->getVar());
					}
				}
				relevantStaticVariables = tmpVector;
			}


			shared_ptr<RunningPlan> parent;
			if (!rp->getParent().expired())
			{
				parent = rp->getParent().lock();
			}
			if (parent && parent->getActiveState() != nullptr)
			{
				vector<Variable*> tmpVector = vector<Variable*>();
				for (Parametrisation* p : parent->getActiveState()->getParametrisation())
				{
					if ((p->getSubPlan() == rp->getPlan() || p->getSubPlan() == rp->getPlanType())
							&& find(relevantStaticVariables.begin(), relevantStaticVariables.end(), p->getSubVar())
									!= relevantStaticVariables.end())
					{
						tmpVector.push_back(p->getVar());
						store->addVarTo(p->getSubVar(), p->getVar());
					}
				}
				relevantStaticVariables = tmpVector;
			}
			rp = parent;
		}
		//now we have a list of ConstraintCalls in calls ready to be queried together with a store of unifications
		if (calls.size() == 0)
		{
#ifdef CQ_DEBUG
			cout << "CQ: Empty Query!" << endl;
#endif
			return false;
		}
		for (shared_ptr<ConstraintCall> c : calls)
		{

			vector<Variable*> conditionVariables = vector<Variable*>(c->getCondition()->getVariables());

			auto varr = make_shared<vector<shared_ptr<SolverVariable>>>(conditionVariables.size());
			for (int j = 0; j < conditionVariables.size(); ++j)
			{
				if (store->getRep(conditionVariables[j])->getSolverVar() == nullptr)
				{
					store->getRep(conditionVariables[j])->setSolverVar(
							solver->createVariable(store->getRep(conditionVariables[j])->getId()));
				}
				varr->at(j) = store->getRep(conditionVariables[j])->getSolverVar();
			}
			auto sortedVars = make_shared<vector<shared_ptr<vector<shared_ptr<vector<shared_ptr<SolverVariable>>> >> >>();
			auto agentsInScope = make_shared<vector<shared_ptr<vector<int>>> >();
			for (int j = 0; j < c->getSortedVariables()->size(); ++j)
			{
				auto ll = make_shared<vector<shared_ptr<vector<shared_ptr<SolverVariable>>> >>();
				agentsInScope->push_back(c->getAgentsInScope()->at(j));
				sortedVars->push_back(ll);
				for (auto dvarr : c->getSortedVariables()->at(j))
				{
					auto dtvarr = make_shared<vector<shared_ptr<SolverVariable>>>(dvarr.size());
					for (int i = 0; i < dtvarr->size(); ++i)
					{
						if(dvarr.at(i)->getSolverVar() == nullptr /*!dvarr.at(i)->getSolverVar().operator bool()*/)
						{
							dvarr.at(i)->setSolverVar(solver->createVariable(dvarr.at(i)->getId()));
						}

						dtvarr->at(i) = dvarr.at(i)->getSolverVar();
					}
					ll->push_back(dtvarr);
				}
			}
			shared_ptr<ConstraintDescriptor> cd = make_shared<ConstraintDescriptor>(varr, sortedVars);
			cd->setAgentsInScope(agentsInScope);
			c->getCondition()->getConstraint(cd, c->getRunningPlan());
			cds.push_back(cd);
		}
		relevantVariables = store->getAllRep();
		domOffset = relevantVariables.size();
		relevantVariables.insert(relevantVariables.end(), relevantDomainVariables.begin(),
									relevantDomainVariables.end());


#ifdef CQ_DEBUG
		cout << "CQ: PrepTime: "
				<< (ae->getIAlicaClock()->now() - time) / 10000.0 << endl;
#endif
		return true;
	}

	vector<Variable*> ConstraintQuery::getRelevantStaticVariables()
	{
		return relevantStaticVariables;
	}

	void ConstraintQuery::setRelevantStaticVariables(vector<Variable*> value)
	{
		relevantStaticVariables = value;
	}

	vector<Variable*> ConstraintQuery::getRelevantDomainVariables()
	{
		return relevantDomainVariables;
	}

	void ConstraintQuery::setRelevantDomainVariables(vector<Variable*> value)
	{
		relevantDomainVariables = value;
	}

	void ConstraintQuery::addConstraintCalls(vector<shared_ptr<ConstraintCall>>& l)
	{
		calls.insert(calls.end(), l.begin(), l.end());
	}

	ConstraintQuery::UniqueVarStore::UniqueVarStore()
	{
		store = vector<vector<Variable*>>();
	}

	void ConstraintQuery::UniqueVarStore::clear()
	{
		store.clear();
	}

	void ConstraintQuery::UniqueVarStore::add(Variable* v)
	{
		vector<Variable*> l = vector<Variable*>();
		l.push_back(v);
		store.push_back(l);
	}

	Variable* ConstraintQuery::UniqueVarStore::getRep(Variable* v)
	{
		for (vector<Variable*> l : store)
		{
			for (Variable* s : l)
			{
				if (s == v)
				{
					return l.front();
				}
			}
		}
		add(v);
		return v;
	}

	void ConstraintQuery::UniqueVarStore::addVarTo(Variable* representing, Variable* toAdd)
	{
		for (vector<Variable*>& l : store)
		{
			for (Variable*& cv : l)
			{
				if (representing == cv)
				{
					l.insert(l.begin(), toAdd);
					return;
				}
			}
		}
		vector<Variable*> nl = vector<Variable*>();
		nl.insert(nl.begin(), representing);
		nl.insert(nl.begin(), toAdd);
		store.push_back(nl);
	}

	vector<Variable*> ConstraintQuery::UniqueVarStore::getAllRep()
	{
		vector<Variable*> ret = vector<Variable*>();
		for (vector<Variable*> l : store)
		{
			ret.push_back(l.front());
		}
		return ret;
	}

	int ConstraintQuery::UniqueVarStore::getIndexOf(Variable* v)
	{
		for (int i = 0; i < store.size(); ++i)
		{
			for (Variable* c : store[i])
			{
				if (c == v)
				{
					return i;
				}
			}
		}
		return -1;
	}
} /* namespace alica */
