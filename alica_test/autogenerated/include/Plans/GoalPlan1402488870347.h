#ifndef GoalPlan_H_
#define GoalPlan_H_

#include "DomainCondition.h"
#include "engine/BasicUtilityFunction.h"
#include "engine/UtilityFunction.h"
#include "engine/DefaultUtilityFunction.h"
/*PROTECTED REGION ID(incl1402488870347) ENABLED START*/
//Add inlcudes here
/*PROTECTED REGION END*/
using namespace alica;

namespace alicaAutogenerated
{
/*PROTECTED REGION ID(meth1402488870347) ENABLED START*/
//Add other things here
/*PROTECTED REGION END*/
class UtilityFunction1402488870347 : public BasicUtilityFunction
{
  shared_ptr<UtilityFunction> getUtilityFunction(Plan* plan);
};

class PreCondition1402489131988 : public DomainCondition
{
  bool evaluate(shared_ptr<RunningPlan> rp);
};

class RunTimeCondition1403773741874 : public DomainCondition
{
  bool evaluate(shared_ptr<RunningPlan> rp);
};

class TransitionCondition1402489174338 : public DomainCondition
{
  bool evaluate(shared_ptr<RunningPlan> rp);
};

class TransitionCondition1402489206278 : public DomainCondition
{
  bool evaluate(shared_ptr<RunningPlan> rp);
};

class TransitionCondition1402489218027 : public DomainCondition
{
  bool evaluate(shared_ptr<RunningPlan> rp);
};

} /* namespace alica */

#endif
