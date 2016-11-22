/*
 * Validator.h
 *
 *  Created on: Nov 22, 2016
 *      Author: Kevin Winata
 */

#ifndef TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_
#define TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_

#include "TFC/Components/ComponentBase.h"
#include <vector>

namespace TFC {
namespace Components {
namespace Validators {

class Validator {
public:
	Validator(ComponentBase* component);
	virtual ~Validator();
	bool Validate();
	std::string const& ErrorMessage;
protected:
	ComponentBase* const component;
	bool validationResult;
	virtual bool ValidateInternal() = 0;
private:
	std::string errorMessage;
};

class ValidatorList {
public:
	ValidatorList();
	~ValidatorList();
	void AddValidator(Validator* validator);
	bool ValidateAll();
	const std::string& LastValidationMessage;
private:
	std::vector<Validator*> validators;
	bool lastValidationResult;
	std::string lastValidationMessage;
};

}
}
}



#endif /* TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_ */
