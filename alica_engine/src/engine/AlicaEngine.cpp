/*
 * AlicaEngine.cpp
 *
 *  Created on: Mar 3, 2014
 *      Author: Stephan Opfer
 */
#define AE_DEBUG

using namespace std;

#include "engine/AlicaEngine.h"
#include "engine/PlanRepository.h"
#include "engine/parser/PlanParser.h"
#include "engine/behaviourpool/BehaviourPool.h"
#include "engine/model/RoleSet.h"
#include "engine/ISyncModul.h"
#include "engine/IRoleAssignment.h"
#include "engine/allocationauthority/AuthorityManager.h"
#include "engine/IEngineModule.h"
#include "engine/planselector/PlanSelector.h"
#include "engine/PlanBase.h"
#include "engine/teamobserver/TeamObserver.h"
#include "engine/logging/Logger.h"
#include "engine/roleassignment/RoleAssignment.h"
#include "engine/UtilityFunction.h"
#include "engine/model/Plan.h"
#include "engine/syncmodul/SyncModul.h"


namespace alica
{
	/**
	 * The main class.
	 */
	AlicaEngine::AlicaEngine()
	{
		this->stepCalled = false;
		this->planBase = nullptr;
		this->planner = nullptr;
		this->planSelector = nullptr;
		this->communicator = nullptr;
		this->alicaClock = nullptr;
		this->syncModul = nullptr;
		this->sc = supplementary::SystemConfig::getInstance();
		this->terminating = false;
		this->teamObserver = nullptr;
		this->roleAssignment = nullptr;
		this->behaviourPool = nullptr;
		this->syncModul = nullptr;
		this->roleSet = nullptr;
		this->masterPlan = nullptr;
		this->planParser = nullptr;
		this->log = nullptr;
		this->planRepository = nullptr;
		this->auth = nullptr;
		this->roleSet = nullptr;
		this->stepEngine = false;
		this->maySendMessages = false;

//		TODO: MODULELOADER CASTOR
//		string modName[] = (*this->sc)["Alica"]->get<string>("Alica", "Extensions", "LoadModule", NULL);
//		if (modName->size() > 0)
//		{
//			for (string name : modName)
//			{
//				ModuleLoader<IEngineModule> l = ModuleLoader<IEngineModule> .Load(name, null, true, true);
//				mods.Add(l.GetInstance(null, null));
//				Console.WriteLine("AE: Loaded Module " + name);
//			}
//		}
		this->maySendMessages = !(*sc)["Alica"]->get<bool>("Alica.SilentStart", NULL);

#ifdef AE_DEBUG
		cout << "AE: Constructor finished!" << endl;
#endif
	}

	AlicaEngine::~AlicaEngine()
	{
	}

	/**
	 * The method for getting the singleton instance.
	 * @return A pointer to the AlicaEngine object, you must not delete.
	 */
	AlicaEngine * AlicaEngine::getInstance()
	{
		static AlicaEngine instance;
		return &instance;
	}

	bool AlicaEngine::init(IBehaviourCreator* bc, string roleSetName, string masterPlanName, string roleSetDir,
	bool stepEngine)
	{
		this->stepEngine = stepEngine;
		if (this->planRepository == nullptr)
		{
			this->planRepository = new PlanRepository();
		}
		if (this->planParser == nullptr)
		{
			this->planParser = new PlanParser(this->planRepository);
		}
		if (this->masterPlan == nullptr)
		{
			this->masterPlan = this->planParser->parsePlanTree(masterPlanName);
		}
		if (this->roleSet == nullptr)
		{
			this->roleSet = this->planParser->parseRoleSet(roleSetName, roleSetDir);
		}
		if (this->behaviourPool == nullptr)
		{
			this->behaviourPool = new BehaviourPool();
		}
		if (this->teamObserver == nullptr)
		{
			this->teamObserver = new TeamObserver();
		}
		if (this->roleAssignment == nullptr)
		{
			this->roleAssignment = new RoleAssignment();
		}
		if (this->syncModul == nullptr)
		{
			this->syncModul = new SyncModul();
		}
		this->stepCalled = false;
		bool everythingWorked = true;
		everythingWorked &= this->behaviourPool->init(bc);
		this->auth = new AuthorityManager();
		this->teamObserver->init();
		this->log = new Logger();
		this->roleAssignment->init();
		this->planSelector = new PlanSelector();
		//TODO
//		ConstraintHelper.Init(this.cSolver);
		this->auth->init();
		this->planBase = new PlanBase(this->masterPlan);
		UtilityFunction::initDataStructures();
		this->syncModul->init();
		return everythingWorked;
	}

	bool AlicaEngine::shutdown()
	{
		bool everythingWorked = true;
		this->maySendMessages = false;
		delete this->planRepository;
		this->planRepository = nullptr;
		delete this->planParser;
		this->planParser = nullptr;
		// roleSet is an Element in the elements set of the model factory and is cleaned there already
		//delete this->roleSet;
		this->roleSet = nullptr;
		delete this->behaviourPool;
		this->behaviourPool = nullptr;
		delete this->teamObserver;
		this->teamObserver = nullptr;
		delete this->planBase;
		this->planBase = nullptr;
		this->masterPlan = nullptr;
		this->syncModul = nullptr;
		return everythingWorked;
	}

	void AlicaEngine::iterationComplete()
	{
		//TODO:
	}

	void AlicaEngine::start()
	{
		this->planBase->start();
		cout << "AE: Engine started" << endl;
	}
	void AlicaEngine::setStepCalled(bool stepCalled)
	{
		this->stepCalled = stepCalled;
	}
	bool AlicaEngine::getStepCalled() const
	{
		return this->stepCalled;
	}

	bool AlicaEngine::getStepEngine()
	{
		return this->stepEngine;
	}
	void AlicaEngine::doStep()
	{
		this->stepCalled = true;
		this->planBase->getStepModeCV()->notify_one();
	}
	PlanRepository * AlicaEngine::getPlanRepository()
	{
		return this->planRepository;
	}

	IPlanSelector* AlicaEngine::getPlanSelector()
	{
		return this->planSelector;
	}
	IAlicaClock* AlicaEngine::getIAlicaClock()
	{
		return this->alicaClock;
	}

	IBehaviourPool * AlicaEngine::getBehaviourPool()
	{
		return this->behaviourPool;
	}

	ITeamObserver * AlicaEngine::getTeamObserver()
	{
		return this->teamObserver;
	}

	void AlicaEngine::setIAlicaClock(IAlicaClock* clock)
	{
		this->alicaClock = clock;
	}

	void AlicaEngine::setTeamObserver(ITeamObserver* teamObserver)
	{
		this->teamObserver = teamObserver;
	}

	ISyncModul * AlicaEngine::getSyncModul()
	{
		return syncModul;
	}

	void AlicaEngine::setSyncModul(ISyncModul* syncModul)
	{
		this->syncModul = syncModul;
	}

	AuthorityManager * AlicaEngine::getAuth()
	{
		return auth;
	}

	void AlicaEngine::setAuth(AuthorityManager* auth)
	{
		this->auth = auth;
	}

	IRoleAssignment * AlicaEngine::getRoleAssignment()
	{
		return roleAssignment;
	}

	void AlicaEngine::setRoleAssignment(IRoleAssignment* roleAssignment)
	{
		this->roleAssignment = roleAssignment;
	}

	IPlanParser * AlicaEngine::getPlanParser()
	{
		return planParser;
	}

	RoleSet * AlicaEngine::getRoleSet()
	{
		return roleSet;
	}

	void AlicaEngine::setStepEngine(bool stepEngine)
	{
		this->stepEngine = stepEngine;
	}

	void AlicaEngine::abort(string msg)
	{
		cerr << "ABORT: " << msg << endl;
		exit(EXIT_FAILURE);
	}

	string AlicaEngine::getRobotName() const
	{
		return sc->getHostname();
	}

	Logger * AlicaEngine::getLog()
	{
		return log;
	}

	void AlicaEngine::setLog(Logger* log)
	{
		this->log = log;
	}

	bool AlicaEngine::isTerminating() const
	{
		return terminating;
	}
	bool AlicaEngine::isMaySendMessages() const
	{
		return maySendMessages;
	}
	void AlicaEngine::setMaySendMessages(bool maySendMessages)
	{
		this->maySendMessages = maySendMessages;
	}

	void AlicaEngine::setTerminating(bool terminating)
	{
		this->terminating = terminating;
	}

	IAlicaCommunication * AlicaEngine::getCommunicator()
	{
		return communicator;
	}

	void AlicaEngine::setCommunicator(IAlicaCommunication * communicator)
	{
		this->communicator = communicator;
		this->roleAssignment->setCommunication(communicator);
	}
	IPlanner* AlicaEngine::getPlanner()
	{
		return planner;
	}

} /* namespace Alica */

