/*
 * FailurePoint.h
 *
 *  Created on: Mar 5, 2014
 *      Author: Stephan Opfer
 */

#ifndef FAILUREPOINT_H_
#define FAILUREPOINT_H_

#include "TerminalState.h"

namespace Alica {

class FailureState: public TerminalState {
public:
	FailureState();
	virtual ~FailureState();
};

} /* namespace Alica */

#endif /* FAILUREPOINT_H_ */
